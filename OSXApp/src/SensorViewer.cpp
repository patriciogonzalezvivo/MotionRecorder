//
//  SensorViewer.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "SensorViewer.h"

void SensorViewer::selfSetup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofSetFullscreen(false);
    ofSetCircleResolution(36);
    
    client.connect("patriciogonzalezvivo.com", 8080);
    client.addListener(this);
    
    map.allocate(320, 240);
    map.loadMap();
    map.setZoom(15);
    
    firstLocation.lat = 0;
    firstLocation.lon = 0;
    
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_POINTS);
}

void SensorViewer::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(grid);
    guiAdd(map);
}

void SensorViewer::selfGuiEvent(ofxUIEventArgs &e){
    
}

void SensorViewer::selfSetupSystemGui(){
    sysGui->addSlider("Sensor_Size", 0, 1, &sensorScale);

}

//---------------------------------------------------

void SensorViewer::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
}

void SensorViewer::selfUpdate(){
    map.update();
    
    {
        string message = "{ \"event\":\"get\" }";
        client.send(message);
    }
}

void SensorViewer::selfDraw(){
    materials["MATERIAL 1"]->begin();
    
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    grid.draw();
    ofPopMatrix();

    mesh.drawVertices();
    
    ofPushMatrix();
    ofScale(1, -1, 1);
    for (std::map<string,Sensor>::iterator it=sensors.begin(); it!=sensors.end(); ++it){
        ofPushMatrix();
        ofTranslate(0,0,it->second.altitud);
        float angle;
        ofVec3f axis;//(0,0,1.0f);
        it->second.attitude.getRotate(angle, axis);
        ofRotate(angle, axis.x, axis.y, axis.z); // rotate with quaternion
        
        ofNoFill();
        ofSetColor(255,200);
        ofDrawAxis(100*sensorScale);
        
        ofSetColor(0, 0, 255,100);
        ofCircle(0, 0, 125*sensorScale);
        
        ofPushMatrix();
        ofRotate(90, 1, 0, 0);
        ofSetColor(0, 255, 0,100);
        ofCircle(0, 0, 125*sensorScale);
        ofPopMatrix();
        
        ofPushMatrix();
        ofRotate(90, 0, 1, 0);
        ofSetColor(255, 0, 0,100);
        ofCircle(0, 0, 125*sensorScale);
        ofPopMatrix();
        
        if(it->second.bRecording){
            ofFill();
            ofSetColor(255,0,0,abs(sin(ofGetElapsedTimef()))*255);
            ofDrawSphere(0, 0, 50*sensorScale);
        }
        
        ofPopMatrix();
    }
    ofPopMatrix();

    materials["MATERIAL 1"]->end();
}

void SensorViewer::selfDrawOverlay(){
    
    if(bDebug){
        ofPushMatrix();
        ofTranslate(ofGetWidth()-map.getWidth()-10, +10);
        map.draw();
        
        for (std::map<string,Sensor>::iterator it=sensors.begin(); it!=sensors.end(); ++it){
            ofPoint sensorPos = map.locationPoint(it->second.location);
            
            ofPolyline sensorPath;
            for (int i = 0; i < it->second.path.size(); i++) {
                sensorPath.addVertex(map.locationPoint(it->second.path[i]));
            }
            
            ofPushStyle();
            ofSetColor(255, 0, 0);
            ofNoFill();
            ofCircle(sensorPos, 5);
            sensorPath.draw();
            
            ofFill();
            ofSetColor(255, 0, 0,abs(sin(ofGetElapsedTimef()))*200);
            ofCircle(sensorPos, 3);
        
            ofPopStyle();
        }
        
        ofPopMatrix();
    }
}

void SensorViewer::selfEnd(){
}

void SensorViewer::selfExit(){
    
}

//--------------------------------------------------------------
void SensorViewer::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void SensorViewer::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void SensorViewer::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void SensorViewer::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void SensorViewer::onMessage( ofxLibwebsockets::Event& args ){    
    if(args.json.size()!=sensors.size()){
        sensors.clear();
    }
    
    bool bMapCenterUploaded = false;
    
    for(int i = 0; i < args.json.size(); i++){
        if(args.json[i]["attitud"] != NULL){
            string id = args.json[i]["id"].asString();
            sensors[id].attitude.set(args.json[i]["attitud"]["x"].asFloat(),
                                                                args.json[i]["attitud"]["y"].asFloat(),
                                                                args.json[i]["attitud"]["z"].asFloat(),
                                                                args.json[i]["attitud"]["w"].asFloat());
            sensors[id].acceleration.set(args.json[i]["acceleration"]["x"].asFloat(),
                                                                    args.json[i]["acceleration"]["y"].asFloat(),
                                                                    args.json[i]["acceleration"]["z"].asFloat());
            sensors[id].location.lat = args.json[i]["position"]["lat"].asDouble();
            sensors[id].location.lon = args.json[i]["position"]["lon"].asDouble();
            sensors[id].altitud = args.json[i]["position"]["alt"].asDouble()*0.1;
            sensors[id].bRecording = args.json[i]["recording"].asBool();
            
            if(!bMapCenterUploaded){
                map.setCenter(sensors[id].location);
                bMapCenterUploaded = true;
            }
            
            bool addLocToPath = false;
            if(sensors[id].path.size()>0){
                if (sensors[id].path[sensors[id].path.size()-1] == sensors[id].location){
                    
                } else {
                    addLocToPath = true;
                }
            }
            sensors[id].path.push_back(sensors[id].location);
            
            if(firstLocation.lat == 0 && firstLocation.lon == 0){
                firstLocation = sensors[args.json[i]["id"].asString()].location;
            }
        }
    }
}

//--------------------------------------------------------------
void SensorViewer::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;
}

void SensorViewer::selfKeyPressed(ofKeyEventArgs & args){

}

void SensorViewer::selfKeyReleased(ofKeyEventArgs & args){
	
}

void SensorViewer::selfMouseMoved(ofMouseEventArgs& data){
	
}

void SensorViewer::selfMousePressed(ofMouseEventArgs& data){

}

void SensorViewer::selfMouseDragged(ofMouseEventArgs& data){

}

void SensorViewer::selfMouseReleased(ofMouseEventArgs& data){

}