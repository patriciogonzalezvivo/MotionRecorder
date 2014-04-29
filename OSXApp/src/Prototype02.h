//
//  Prototype02.h
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#pragma once

#include "UI3DProject.h"
#include "ofxLibwebsockets.h"

#include "UI3DGrid.h"

#include "UIMap.h"
#include "StreetView.h"

struct Sensor{
    vector<Location>   path;
    ofQuaternion    attitude;
    ofVec3f         acceleration;
    Location        location;
    double          altitud;
    bool            bRecording;
};

class Prototype02 : public UI3DProject {
public:
    
    string getSystemName(){ return "Prototype02";}
    
    void selfSetupGuis();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
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
    
    void onConnect( ofxLibwebsockets::Event& args );
    void onOpen( ofxLibwebsockets::Event& args );
    void onClose( ofxLibwebsockets::Event& args );
    void onIdle( ofxLibwebsockets::Event& args );
    void onMessage( ofxLibwebsockets::Event& args );
    void onBroadcast( ofxLibwebsockets::Event& args );
    
protected:
  
    ofxLibwebsockets::Client client;
    map<string,Sensor>  sensors;
    
    UIMap       map;
    Location    firstLocation;
    
    void        addLook(ofPoint _center);
    StreetView  gsv;
    ofVboMesh   mesh;
    string      actualPanoId;
    vector<string> panoIds;
    bool        bLoadSV;
    
    UI3DGrid    grid;
    
    float       sensorScale;
};