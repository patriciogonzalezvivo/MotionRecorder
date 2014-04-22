#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    
    motionManager = [[CMMotionManager alloc] init];
    float updateFrequency = 1.0f/30.0f;
    [motionManager setDeviceMotionUpdateInterval: updateFrequency];
    referenceAttitude = nil;
    referenceFrameType = [motionManager attitudeReferenceFrame];
    [motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXMagneticNorthZVertical];
    
    bRecording = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    CMDeviceMotion *deviceMotion = motionManager.deviceMotion;
    
    // gravity
    gravity.x = deviceMotion.gravity.x;
    gravity.y = deviceMotion.gravity.y;
    gravity.z = deviceMotion.gravity.z;
    
    // user acceleration
    userAcceleration.x = deviceMotion.userAcceleration.x;
    userAcceleration.y = deviceMotion.userAcceleration.y;
    userAcceleration.z = deviceMotion.userAcceleration.z;
    
    CMAttitude *attitude = deviceMotion.attitude;
    if (referenceAttitude != nil ) {
        [attitude multiplyByInverseOfAttitude:referenceAttitude];
    }
    
    // attitude quaternion
    CMQuaternion quat = attitude.quaternion;
    attitudeQuat.set(quat.x, quat.y, quat.z, quat.w);
    
    string line =   ofGetTimestampString() + "," +
                    ofToString(quat.x) + "," +
                    ofToString(quat.y) + "," +
                    ofToString(quat.z) + "," +
                    ofToString(quat.w) + "\n";
    
    if(bRecording){
        buffer.append(line);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (bRecording) {
        ofBackground(abs(sin(ofGetElapsedTimef()))*255.0,0,0);
    } else {
        ofBackground(0);
    }
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    ofSetColor(255,0,0);
    ofLine(ofPoint(0,0,0), userAcceleration*100);
    
    ofSetColor(255);
    // 1) quaternion rotations
    float angle;
    ofVec3f axis;//(0,0,1.0f);
    attitudeQuat.getRotate(angle, axis);
    ofRotate(angle, axis.x, -axis.y, axis.z); // rotate with quaternion
    
    ofNoFill();
	ofDrawBox(0, 0, 0, 220); // OF 0.74: ofBox(0, 0, 0, 220);
    ofDrawAxis(100);
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    if(bRecording){
        string fileName = "../Documents/"+ofGetTimestampString()+".csv";
        ofBufferToFile(fileName, buffer);
        buffer.clear();
        bRecording = false;
    } else {
        bRecording = true;
    }
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}
