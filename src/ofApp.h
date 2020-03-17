#pragma once

#include "ofMain.h"
#include "ofxOMXPlayer.h"
#include "ofxXmlSettings.h"

using namespace glm;

class ofApp : public ofBaseApp {

	public:	
		void setup();
		void update();
		void draw();
		
	    ofShader shader;
	    ofPlanePrimitive plane;
		ofxOMXPlayer movie;
		string movieFile;

		vec2 pos = vec2(0, 0);
		vec2 rot = vec2(0, 0);
		vec2 rotRange = vec2(25, 15);
		float rotSpeed = 0.65;
		vec2 rotOffset = vec2(0, 0);
		vec2 rotOffsetOrig = vec2(rotOffset.x, rotOffset.y);
		vec2 rotTarget = vec2(rotOffset.x, rotOffset.y);
		vec2 rotScaler = vec2(1.0, 0.1);

		float posSpeed = 0.65;
		vec2 posRange = vec2(150, 10);
		vec2 posOffset = vec2(-10, 0);
		vec2 posOffsetOrig = vec2(posOffset.x, posOffset.y);
		vec2 posTarget = vec2(posOffset.x, posOffset.y);
		vec2 posScaler = vec2(-1.0, 0.5);

		float returnSpeed = 0.001;
		float zPos = 100;
		int planeResX = 192;
		int planeResY = 108;

		// ~ ~ ~ ~ ~ ~ ~ 

		string compname;
		string host; // hostname;

		bool debug; // draw to local screen, default true

		ofFile file;
		ofxXmlSettings settings;

		vec2 motionValRaw;

		int framerate;

};
