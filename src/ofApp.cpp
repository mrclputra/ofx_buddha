#include "ofApp.h"
#include "ofGLProgrammableRenderer.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(ofFloatColor(0.0f));

	// configure scene objects
	boxMesh = ofMesh::box(1, 1, 1, 24, 24, 24);

	baseMaterial.setDiffuseColor(ofFloatColor(0.8f));
	baseMaterial.setShininess(60);
	baseMaterial.setSpecularColor(ofFloatColor(1));

	// configure camera
	camera.setPosition(150, 0, 0);
	camera.lookAt(ofVec3f(0, 0, 0));

	// configure lights
	// - light 1: directional area light
	auto areaLight = std::make_shared<ofLight>();
	areaLight->enable();
	areaLight->setDirectional();
	areaLight->setPosition(500, 500, 500);
	areaLight->lookAt(glm::vec3(0, 0, 0));
	areaLight->setDiffuseColor(ofFloatColor(1, 1, 1));
	areaLight->setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
	areaLight->getShadow().setStrength(1.0f);
	areaLight->getShadow().setNearClip(100);
	areaLight->getShadow().setFarClip(2000);
	lights.push_back(areaLight);

	// - light 2: spotlight
	auto spotLight = std::make_shared<ofLight>();
	spotLight->enable();
	spotLight->setSpotlight(60, 20);
	spotLight->getShadow().setNearClip(200);
	spotLight->getShadow().setFarClip(2000);
	spotLight->getShadow().setStrength(0.6f);
	spotLight->setPosition(210, 330.0, 750);
	spotLight->setAmbientColor(ofFloatColor(0.4));
	// orient
	glm::quat xq = glm::angleAxis(glm::radians(-30.0f), glm::vec3(1, 0, 0));
	glm::quat yq = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
	spotLight->setOrientation(yq * xq);
	lights.push_back(spotLight);

	// configure shadows
	ofShadow::enableAllShadows();
	ofShadow::setAllShadowBias(0.007);
	ofShadow::setAllShadowNormalBias(-4.f);
	ofShadow::setAllShadowTypes(OF_SHADOW_TYPE_PCF_HIGH);
	ofShadow::setAllShadowSampleRadius(10.0f); // softness
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	// enable perspective
	ofEnableDepthTest();

	for (int i = 0; i < lights.size(); i++) {
		auto& light = lights[i];
		// query light to see if it has a depth pass
		if (light->shouldRenderShadowDepthPass()) {
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

	// do camera stuff
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

	camera.end();

	ofDisableDepthTest();
}

void ofApp::renderScene() {
	float etimef = ofGetElapsedTimef();

	// draw base cube, rotate over time
	baseMaterial.begin();
	ofPushMatrix(); {
		ofTranslate(0, 0, 0);
		ofRotateZDeg(ofWrapDegrees((etimef * 0.04) * 360));
		ofRotateXDeg(ofWrapDegrees((etimef * 0.06) * 360));
		ofScale(50, 50, 50);
		boxMesh.draw();
	} ofPopMatrix();
	baseMaterial.end();
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
