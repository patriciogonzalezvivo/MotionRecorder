#pragma once

#include "ofMain.h"
#include "ofxiOS.h"
#include "ofxiOSExtras.h"

#import <CoreMotion/CMMotionManager.h>

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

    CMMotionManager* motionManager;
	CMAttitude* referenceAttitude;
    CMAttitudeReferenceFrame referenceFrameType;
    
    ofQuaternion attitudeQuat;
    ofVec3f userAcceleration;
    ofVec3f gravity;
    
    ofBuffer buffer;
    
    bool    bRecording;
};


