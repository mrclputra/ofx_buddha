#include "ofApp.h"
#include "ofGLProgrammableRenderer.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetBackgroundColor(ofFloatColor(0.0f));

	// check if shadows supported
	if (!ofShadow::areShadowsSupported()) {
		ofLogError() << "Shadows are not supported on this platform!";
	}

	// configure lights
	// - light 1: directional area light
	auto light1 = std::make_shared<ofLight>();
	light1->enable();
	light1->setDirectional();
	light1->setPosition(200, 230, 200);
	light1->lookAt(glm::vec3(0, 0, 0));
	light1->setDiffuseColor(ofFloatColor(1, 1, 1));
	light1->setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
	light1->getShadow().setStrength(0.6f);
	//light1->getShadow().setNearClip(50);
	//light1->getShadow().setFarClip(500);
	lights.push_back(light1);

	// - light 2: directional area light
	auto light2 = std::make_shared<ofLight>();
	light2->enable();
	light2->setDirectional();
	light2->setPosition(-300, 200, 0);
	light2->lookAt(glm::vec3(0, 0, 0));
	light2->setDiffuseColor(ofFloatColor(.5, .5, .5));
	light2->setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
	light2->getShadow().setStrength(0.3f);
	lights.push_back(light2);

	// - light 2: spotlight
	//auto light2 = std::make_shared<ofLight>();
	//light2->enable();
	//light2->setSpotlight(60, 20);
	//light2->getShadow().setNearClip(200);
	//light2->getShadow().setFarClip(2000);
	//light2->getShadow().setStrength(0.6f);
	////light2->setPosition(210, 330.0, 750);
	//light2->setPosition(100,100,100);
	//light2->setAmbientColor(ofFloatColor(0.4));
	//// orient
	///*glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
	//glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
	//spotLight->setOrientation(yq * xq);*/
	//light2->lookAt(ofVec3f(0.0f, 0.0f, 0.0f));
	//lights.push_back(light2);

	// configure shadows
	ofShadow::enableAllShadows();
	ofShadow::setAllShadowBias(0.002f);
	ofShadow::setAllShadowNormalBias(-4.0f);
	ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_HIGH);
	ofShadow::setAllShadowSampleRadius(4.0f); // softness
#ifndef TARGET_OPENGLES
	ofShadow::setAllShadowDepthResolutions(1024, 1024);
#endif


	// configure scene objects
	boxMesh = ofMesh::box(1, 1, 1, 24, 24, 24);

	model.loadModel("happy_buddha_stanford.glb");
	model.disableMaterials();

	baseMaterial.setAmbientColor(ofFloatColor(0.85, 0.16, 0.43) * 0.8);
	baseMaterial.setDiffuseColor(ofFloatColor(0.34, 0.58, 0.38, 1.0));
	baseMaterial.setSpecularColor(ofFloatColor(ofFloatColor(1.0, 1.0, 1.0, 1.0)));
	baseMaterial.setShininess(40);

	bgMaterial.setDiffuseColor(ofFloatColor(0.15));
	bgMaterial.setShininess(1.0);
	bgMaterial.setSpecularColor(ofFloatColor(0.25));

	// configure camera
	camera.setPosition(150, 0, 0);
	camera.lookAt(ofVec3f(0, 0, 0));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofEnableDepthTest();

	for (int i = 0; i < lights.size(); i++) {
		auto& light = lights[i];
		// query light to see if it has a depth pass
		if (light->shouldRenderShadowDepthPass()) {
			ofLog() << "Rendering shadow depth pass for light " << i;
			// Get the number of passes required.
			// By default the number of passes is 1. And we could just call beginShadowDepthPass() or beginShadowDepthPass(0);
			// It will be more than one pass if it is a pointlight with setSingleOmniPass set to false
			// or a platform that does not support geometry shaders.
			// Most likely it will be a single pass, but we get the number of passes to be safe.
			int numShadowPasses = light->getNumShadowDepthPasses();
			for (int j = 0; j < numShadowPasses; j++) {
				light->beginShadowDepthPass(j);
				// Shadows have the following gl culling enabled by default
				// this helps reduce z fighting by only rendering the rear facing triangles to the depth map
				// enables face culling
				//glEnable(GL_CULL_FACE);
				// sets the gl triangle winding order, default for ofShadow is GL_CW
				//glFrontFace(mGlFrontFaceWindingOrder);
				// tells OpenGL to cull front faces
				//glCullFace(GL_FRONT);

				// the culling can be disabled by calling
				//light->getShadow().setGlCullingEnabled(false);
				// or the culling winding order can be changed by calling
				//light->getShadow().setFrontFaceWindingOrder(GL_CCW); // default is GL_CW
				renderScene();
				light->endShadowDepthPass(j);
			}
		}
	}

	// main rendering pass
	camera.begin();
	if (!ofIsGLProgrammableRenderer()) {
		ofEnableLighting();
	}

	// cull back faces of geometry for rendering
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	renderScene();
	glDisable(GL_CULL_FACE);

	if (!ofIsGLProgrammableRenderer()) {
		ofDisableLighting();
	}

	for (int i = 0; i < lights.size(); i++) {
		auto& light = lights[i];

		ofSetColor(light->getDiffuseColor());
		/*if (light->getType() == OF_LIGHT_POINT) {
			ofDrawSphere(light->getPosition(), 12);
		}
		else {
			light->draw();
		}*/
		ofDrawSphere(light->getPosition(), 4);
		if (light->getIsEnabled() && light->getShadow().getIsEnabled()) {
			light->getShadow().drawFrustum();
		}
	}

	camera.end();
	ofDisableDepthTest();

	stringstream ss;

	if (!ofIsGLProgrammableRenderer()) {
		ss << endl << "SHADOWS ONLY WORK WITH PROGRAMMABLE RENDERER!" << endl;
	}
	else if (!ofShadow::areShadowsSupported()) {
		ss << endl << "SHADOWS NOT SUPPORTED ON THIS PLATFORM!" << endl;
	}

	ofDrawBitmapStringHighlight(ss.str(), 20, 20);

	ofSetColor(255);
	ofEnableAlphaBlending();
}

void ofApp::renderScene() {
	//float etimef = ofGetElapsedTimef();

	// draw base cube, rotate over time
	/*baseMaterial.begin();
	ofPushMatrix(); {
		ofTranslate(0, 0, 0);
		ofRotateZDeg(ofWrapDegrees((etimef * 0.04) * 360));
		ofRotateXDeg(ofWrapDegrees((etimef * 0.06) * 360));
		ofScale(50, 50, 50);
		boxMesh.draw();
	} ofPopMatrix();
	baseMaterial.end();*/

	baseMaterial.begin();
	ofPushMatrix(); {
		ofTranslate(0, 0, 0);
		ofRotateDeg(180, 1, 0, 0);
		ofRotateY(180);
		ofScale(0.3f, 0.3f, 0.3f);
		model.drawFaces();
	} ofPopMatrix();
	baseMaterial.end();

	/*bgMaterial.begin();
	ofPushMatrix(); {
		ofTranslate(0, -70, 0);
		ofScale(200, 4, 200);
		boxMesh.draw();
	} ofPopMatrix();
	bgMaterial.end();*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
