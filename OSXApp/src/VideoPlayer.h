//
//  VideoPlayer.h
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#pragma once



#include "UI3DProject.h"

#include "UI3DGrid.h"
#include "UIMap.h"


//#define USE_GSTREAMER
#ifndef USE_GSTREAMER
#include "ofxAVFVideoPlayer.h"
#endif


struct FrameRecord{
    float           sec;
    
    ofQuaternion    attitude;
    ofVec3f         acceleration;
    Location        location;
    double          altitud;
};

class VideoPlayer : public UI3DProject {
public:
    
    string getSystemName(){ return "VideoPlayer";}
    
    void selfSetupGuis();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfBegin();
    
    void selfSetup();
    void selfUpdate();
    
    void selfDraw();
    void selfDrawOverlay();
    
	void selfEnd();
    void selfExit();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void sphereMake();
    void spherePoint(int _x, int _y);
    
protected:
    UI3DGrid grid;

#ifdef USE_GSTREAMER
    ofVideoPlayer   player;
#else
    ofxAVFVideoPlayer   player;
#endif
    
    ofVboMesh       sphereMesh;
    int             sphereDefinition;
    float           sphereRadio;
    bool            bPlaying;
    
    vector<FrameRecord> sensor;
    ofQuaternion    attitude;
    float           xOffSet,yOffSet,zOffSet,wOffSet;
    float           time,timeOffSet;
    int             frame;
};