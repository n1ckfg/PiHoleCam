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
    width = settings.getValue("settings:width", 160);
    height = settings.getValue("settings:height", 120);
    halfWidth = width / 2;
    centerZone = width / 8;
    lBoundary = halfWidth - centerZone;
    rBoundary = halfWidth + centerZone;
    ofSetFrameRate(framerate);

    cam.setup(width, height, false); // color/gray;
    imitate(previous, cam);
    imitate(diff, cam);

    planeResX = settings.getValue("settings:plane_res_x", 128);
    planeResY = settings.getValue("settings:plane_res_y", 128);

    // ~ ~ ~   cam settings   ~ ~ ~
    camSharpness = settings.getValue("settings:sharpness", 0);
    camContrast = settings.getValue("settings:contrast", 0);
    camBrightness = settings.getValue("settings:brightness", 50);
    camIso = settings.getValue("settings:iso", 300);
    camExposureMode = settings.getValue("settings:exposure_mode", 0);
    camExposureCompensation = settings.getValue("settings:exposure_compensation", 0);
    camShutterSpeed = settings.getValue("settings:shutter_speed", 0);

    cam.setSharpness(camSharpness);
    cam.setContrast(camContrast);
    cam.setBrightness(camBrightness);
    cam.setISO(camIso);
    cam.setExposureMode((MMAL_PARAM_EXPOSUREMODE_T)camExposureMode);
    cam.setExposureCompensation(camExposureCompensation);
    cam.setShutterSpeed(camShutterSpeed);
    //cam.setFrameRate // not implemented in ofxCvPiCam

    // ~ ~ ~ ~ ~ ~ ~ 

    //ofEnableDepthTest();
    shader.load("shader");
   
    plane.set(ofGetWidth(), ofGetHeight(), planeResX, planeResY);
    //plane.mapTexCoords(movie.getWidth(), movie.getHeight(), 1, 1);
    plane.mapTexCoordsFromTexture(movie.getTextureReference());

    posOffset = vec2(ofGetWidth() / 2, ofGetHeight() / 2);
    pos = vec2(posOffset.x, posOffset.y);

    motionValRaw = vec2(0, 0);
}

//--------------------------------------------------------------
void ofApp::update() {
    frame = cam.grab();

    if (!frame.empty()) {
        columnLast = columnNow;

        // take the absolute difference of prev and cam and save it inside diff
        absdiff(frame, previous, diff);
        diff.update();

        // like ofSetPixels, but more concise and cross-toolkit
        copy(frame, previous);

        // this is the key line: get the average of each column
        columnMean = meanCols(diff);

        motionValRaw = vec2(0,0);

        for (int i = 0; i < columnMean.rows; i++) {
            // Vec3b is one way of storing 24-bit (3 byte) colors
            Vec3b cur = columnMean.at<Vec3b>(i);
            float mean = (cur[0] + cur[1] + cur[2]) / 3.0;
            if (i < lBoundary) {
                motionValRaw.x -= mean;
            } else if (i > rBoundary) {
                motionValRaw.x += mean;
            }
        }

        motionValRaw /= width;
        cout << motionValRaw << endl;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    //if (!movie.isTextureEnabled()) return;

    ofBackground(0);

    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
    movie.getTextureReference().bind();

    shader.begin();

    ofPushMatrix();

    posTarget += motionValRaw * posScaler;

    //float px = ofClamp(ofLerp(pos.x, posTarget.x, posSpeed), -posRange.x, posRange.x);
    //float py = ofClamp(ofLerp(pos.y, posTarget.y, posSpeed), -posRange.y, posRange.y);
    //pos = vec2(px, py);
    pos = glm::clamp(glm::mix(pos, posTarget, posSpeed), -posRange, posRange);

    // translate plane into center screen.
    ofTranslate(pos.x + posOffset.x, pos.y + posOffset.y, zPos);
    //ofScale(1, -1, 1);

    rotTarget += motionValRaw * rotScaler;

    //float rx = ofClamp(ofLerp(rot.x, rotTarget.x, rotSpeed), -rotRange.x, rotRange.x);
    //float ry = ofClamp(ofLerp(rot.y, rotTarget.y, rotSpeed), -rotRange.y, rotRange.y);
    //rot = vec2(rx, ry);
    rot = glm::clamp(glm::mix(rot, rotTarget, rotSpeed), -rotRange, rotRange);

    ofRotateDeg(rot.x + rotOffset.x, 0, 1, 0);
    ofRotateDeg(rot.y + rotOffset.y, 1, 0, 0);

    plane.draw(); //drawWireframe();

    ofPopMatrix();

    posTarget = glm::mix(posTarget, posOffsetOrig, returnSpeed);
    rotTarget = glm::mix(rotTarget, rotOffsetOrig, returnSpeed);

    shader.end();
}
