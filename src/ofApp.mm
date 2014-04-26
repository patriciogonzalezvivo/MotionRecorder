#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofSetCircleResolution(36);
    ofxiOSDisableIdleTimer();
    
    //  WebSockets
    //
    webSocket = [[SRWebSocket alloc] initWithURLRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"ws://ec2-54-86-66-121.compute-1.amazonaws.com:8080"]]];
    [webSocket open];
    
    //  GPS
    //
    coreLocation = new ofxiOSCoreLocation();
	hasGPS = coreLocation->startLocation();
    lat = lon = 0;
    
    //  Motion
    //
    motionManager = [[CMMotionManager alloc] init];
    float updateFrequency = 1.0f/30.0f;
    [motionManager setDeviceMotionUpdateInterval: updateFrequency];
    referenceAttitude = nil;
    referenceFrameType = [motionManager attitudeReferenceFrame];
    [motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXTrueNorthZVertical];
    
    //  Buffer
    //
    bRecording = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    CMDeviceMotion *deviceMotion = motionManager.deviceMotion;

    // User acceleration
    //
    accel.x = deviceMotion.userAcceleration.x;
    accel.y = deviceMotion.userAcceleration.y;
    accel.z = deviceMotion.userAcceleration.z;
    
    CMAttitude *attitude = deviceMotion.attitude;
    if (referenceAttitude != nil ) {
        [attitude multiplyByInverseOfAttitude:referenceAttitude];
    }
    
    // Attitude
    //
    CMQuaternion quat = attitude.quaternion;
    attitudeQuat.set(quat.x, quat.y, quat.z, quat.w);
    
    //  GPS Pos
    //
    if(hasGPS){
        lat = coreLocation->getLatitude();
        lon = coreLocation->getLongitude();
        alt = coreLocation->getAltitude();
    }
    
    
    //  If web socket open -> send
    //
    if(webSocket.readyState == SR_OPEN){
        string string = "{  \"event\":\"update\",\
                            \"attitud\":{\
                                            \"x\":"+ofToString(quat.x)+",\
                                            \"y\":"+ofToString(quat.y)+",\
                                            \"z\":"+ofToString(quat.z)+",\
                                            \"w\":"+ofToString(quat.w)+"\
                                        },\
                            \"acceleration\":{\
                                                \"x\":"+ofToString(accel.x)+",\
                                                \"y\":"+ofToString(accel.y)+",\
                                                \"z\":"+ofToString(accel.z)+"\
                                            },\
                            \"position\":{\
                                            \"lat\":"+ofToString(lat)+",\
                                            \"lon\":"+ofToString(lon)+",\
                                            \"alt\":"+ofToString(alt)+"\
                                        },\
                            \"recording\":"+ofToString(bRecording)+"\
                        }";
        NSString *message = [[NSString stringWithUTF8String:string.c_str()] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];;
        [webSocket send:message];
    }
    
    //  If recording record
    //
    if(bRecording){
        string line =   ofGetTimestampString() + "," +
        ofToString(quat.x) + "," +
        ofToString(quat.y) + "," +
        ofToString(quat.z) + "," +
        ofToString(quat.w) + "," +
        ofToString(accel.x)+ "," +
        ofToString(accel.y)+ "," +
        ofToString(accel.z)+ "," +
        ofToString(lat)+ "," +
        ofToString(lon)+ "," +
        ofToString(alt)+ "\n";
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
    
    ofSetColor(255,0,255);
    ofLine(ofPoint(0,0,0), accel*-100);
    
    ofSetColor(255);
    // 1) quaternion rotations
    float angle;
    ofVec3f axis;//(0,0,1.0f);
    attitudeQuat.getRotate(angle, axis);
    ofRotate(angle, axis.x, -axis.y, axis.z); // rotate with quaternion
    
    ofNoFill();
    ofSetColor(100,50);
	ofDrawBox(0, 0, 0, 100);
    ofSetColor(255,200);
    ofDrawAxis(100);
    
    ofSetColor(255);
    ofDrawBitmapString("N", ofPoint(110,0,0));
    ofDrawBitmapString("NE", ofPoint(80,80,0));
    ofDrawBitmapString("E", ofPoint(0,110,0));
    ofDrawBitmapString("SE", ofPoint(-80,80,0));
    ofDrawBitmapString("S", ofPoint(-110,0,0));
    ofDrawBitmapString("SW", ofPoint(-80,-80,0));
    ofDrawBitmapString("W", ofPoint(0,-110,0));
    ofDrawBitmapString("NW", ofPoint(80,-80,0));
    
    ofSetColor(0, 0, 255,100);
    ofCircle(0, 0, 125);
    
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    ofSetColor(0, 255, 0,100);
    ofCircle(0, 0, 125);
    ofPopMatrix();
    
    ofPushMatrix();
    ofRotate(90, 0, 1, 0);
    ofSetColor(255, 0, 0,100);
    ofCircle(0, 0, 125);
    ofPopMatrix();
    
    ofPopMatrix();
    
    ofSetColor(255);
    ofDrawBitmapString("Lat: "+ofToString(lat), ofPoint(10,30,0));
    ofDrawBitmapString("Lon: "+ofToString(lon), ofPoint(10,45,0));
    ofDrawBitmapString("Alt: "+ofToString(alt), ofPoint(10,60,0));
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
