#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void renderScene();
		void mouseScrolled(ofMouseEventArgs& mouse);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		vector<shared_ptr<ofLight>> lights;

		// objects
		ofxAssimpModelLoader model;
		ofMesh buddhaMesh;
		ofMesh boxMesh;

		// materials
		ofMaterial baseMaterial;
		ofMaterial bgMaterial;

		// orbit cam
		ofCamera camera;
		ofEventListener listener;
		float orbitLon;
		float orbitLat;
		float orbitRadius;
		bool isOrbiting = false;
		glm::vec2 lastMousePos;
};
