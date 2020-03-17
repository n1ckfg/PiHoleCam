#include "ofApp.h"

using namespace cv;
using namespace ofxCv;
using namespace glm;

//--------------------------------------------------------------
void ofApp::setup() {
    settings.loadFile("settings.xml");

    ofSetVerticalSync(false);
    ofHideCursor();

    //movie.load("Comp_8.mp4");
    //movie.setLoopState(OF_LOOP_NORMAL);
    //movie.play();

    movieFile = settings.getValue("settings:movie_file", "test.mp4");

    string videoPath = ofToDataPath("./" + movieFile, true);
    ofxOMXPlayerSettings omxSettings;
    omxSettings.videoPath = videoPath;
    omxSettings.useHDMIForAudio = true;    //default true
    omxSettings.enableTexture = true;      //default true
    omxSettings.enableLooping = true;      //default true
    omxSettings.enableAudio = false;        //default true, save resources by disabling
    //omxSettings.doFlipTexture = true;        //default false
    
    movie.setup(omxSettings);
    //movie.start();

    framerate = settings.getValue("settings:framerate", 60);
    ofSetFrameRate(framerate);

    debug = (bool)settings.getValue("settings:debug", 1);

    float rx = settings.getValue("settings:rot_scale_x", 1.0);
    float ry = settings.getValue("settings.rot_scale_y", 0.1);
    float px = settings.getValue("settings:pos_scale_x", -1.0);
    float py = settings.getValue("settings.pos_scale_y", 0.5);  
    rotScaler = vec2(rx, ry);
    posScaler = vec2(px, py);
    
    // ~ ~ ~   get a persistent name for this computer   ~ ~ ~
    compname = "RPi";
    file.open(ofToDataPath("compname.txt"), ofFile::ReadWrite, false);
    ofBuffer buff;
    if (file) { // use existing file if it's there
        buff = file.readToBuffer();
        compname = buff.getText();
    }
    else { // otherwise make a new one
        compname += "_" + ofGetTimestampString("%y%m%d%H%M%S%i");
        ofStringReplace(compname, "\n", "");
        ofStringReplace(compname, "\r", "");
        buff.set(compname.c_str(), compname.size());
        ofBufferToFile("compname.txt", buff);
    }
    std::cout << compname << endl;

    // ~ ~ ~ ~ ~ ~ ~ 

    //ofEnableDepthTest();
    shader.load("shader");

    plane.set(ofGetWidth(), ofGetHeight(), planeResX, planeResY);
    //plane.mapTexCoords(movie.getWidth(), movie.getHeight(), 1, 1);
    plane.mapTexCoordsFromTexture(movie.getTextureReference());

    posOffset = vec2(ofGetWidth() / 2, ofGetHeight() / 2);
    pos = vec2(posOffset.x, posOffset.y);
}

//--------------------------------------------------------------
void ofApp::update() {
    //
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);

    movie.getTextureReference().bind();

    shader.begin();

    ofPushMatrix();

    posTarget += motionValRaw * posScaler;

    pos = glm::clamp(glm::mix(pos, posTarget, posSpeed), -posRange, posRange);

    ofTranslate(pos.x + posOffset.x, pos.y + posOffset.y, zPos);
    //ofScale(1, -1, 1);

    rotTarget += motionValRaw * rotScaler;

    rot = glm::clamp(glm::mix(rot, rotTarget, rotSpeed), -rotRange, rotRange);

    ofRotateDeg(rot.x + rotOffset.x, 0, 1, 0);
    ofRotateDeg(rot.y + rotOffset.y, 1, 0, 0);

    plane.draw(); //drawWireframe();

    ofPopMatrix();

    posTarget = glm::mix(posTarget, posOffsetOrig, returnSpeed);
    rotTarget = glm::mix(rotTarget, rotOffsetOrig, returnSpeed);

    shader.end();
}

