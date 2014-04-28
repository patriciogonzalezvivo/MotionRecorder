#pragma once

#include "ofMain.h"


#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
//#include "ofxiOS.h"
//#include "ofxiOSExtras.h"
#import <CoreMotion/CMMotionManager.h>

#import "SRWebSocket.h"

class ofApp : public ofxiOSApp {
public:
    void setup();
    void update();
    void draw();
    void exit();
	
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    void touchDoubleTap(ofTouchEventArgs & touch);
    void touchCancelled(ofTouchEventArgs & touch);

    void lostFocus();
    void gotFocus();
    void gotMemoryWarning();
    void deviceOrientationChanged(int newOrientation);

    //  WebSockets
    //
    SRWebSocket *webSocket;
    double      lat, lon, alt;
    bool        hasGPS;
    
    //  GPS
    //
    ofxiOSCoreLocation * coreLocation;
    
    //  Attitud (Compaz + Gyroscope + acceler )
    //
    CMMotionManager* motionManager;
	CMAttitude* referenceAttitude;
    CMAttitudeReferenceFrame referenceFrameType;
    ofQuaternion attitudeQuat;
    ofVec3f accel;
    
    //  Buffer
    //
    ofBuffer buffer;
    bool    bRecording;
    
    //  UI
    //
    ofPoint resetButtonPos;
};


