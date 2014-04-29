//
//  Prototype02.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "Prototype02.h"

void Prototype02::selfSetup(){
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
    
    bLoadSV = false;
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_POINTS);
}

void Prototype02::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    guiAdd(grid);
    guiAdd(map);
}

void Prototype02::selfGuiEvent(ofxUIEventArgs &e){
    
}

void Prototype02::selfSetupSystemGui(){
    sysGui->addSlider("Sensor_Size", 0, 1, &sensorScale);

}

//---------------------------------------------------

void Prototype02::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
}

void Prototype02::selfUpdate(){
    map.update();
    gsv.update();
    
    {
        string message = "{ \"event\":\"get\" }";
        client.send(message);
    }
    
    if(!bLoadSV && firstLocation.lon != 0 && firstLocation.lat != 0){
        gsv.setLocation(firstLocation);
        gsv.setZoom(1);
        bLoadSV = true;
    }
    
    if (gsv.isTextureLoaded() && gsv.getPanoId() != actualPanoId){
        Location newLocation = gsv.getLocation();
        
        ofPoint newPosition = map.locationPoint(newLocation);
        ofPoint firstPosition = map.locationPoint(firstLocation);
        ofPoint diff = newPosition-firstPosition;
        
        float angle = atan2(diff.y,diff.x);
        double dist = firstLocation.getDistanceTo(newLocation);
        
        ofPoint actualPosition;
        actualPosition.x = dist*cos(angle);
        actualPosition.y = dist*sin(-angle);
        
        addLook(actualPosition);
        actualPanoId = gsv.getPanoId();
        gsv.setLocation(map.getCenter());
    }
}

void Prototype02::addLook(ofPoint _center){
    int mapWidth = gsv.getDepthMapWidth();
    int mapHeight = gsv.getDepthMapHeight();
    
    ofQuaternion ang_offset;
    ang_offset.makeRotate(180-gsv.getDirection(), 0, 0, 1);
    float tiltAngle = gsv.getTiltYaw()*DEG_TO_RAD;
    ofQuaternion tilt_offset;
    tilt_offset.makeRotate(gsv.getTiltPitch(), cos(tiltAngle), sin(-tiltAngle), 0);
    
    ofPixels panoPixels;
    gsv.getTextureReference().readToPixels(panoPixels);
    
    for (int x = 0; x < mapWidth; x++) {
        for (unsigned int y = 0; y < mapHeight; y++) {
            
            float rad_azimuth = x / (float) (mapWidth - 1.0f) * TWO_PI;
            float rad_elevation = y / (float) (mapHeight - 1.0f) * PI;
            
            ofPoint pos;
            pos.x = sin(rad_elevation) * sin(rad_azimuth);
            pos.y = sin(rad_elevation) * cos(rad_azimuth);
            pos.z = cos(rad_elevation);
            
            int index = y*mapWidth+x;
            int depthMapIndex = gsv.depthmapIndices[index];
            if (depthMapIndex != 0){
                DepthMapPlane plane = gsv.depthmapPlanes[depthMapIndex];
                double distance = -plane.d/(plane.x * pos.x + plane.y * pos.y + -plane.z * pos.z);
                float brigtness = panoPixels.getColor(x,y).getBrightness();
                
                if(distance>3){
                    ofPoint vertex;
                    vertex = ang_offset * tilt_offset * pos;
                    vertex *= distance;
                    vertex += _center+ofPoint(0,0,gsv.getGroundHeight()+gsv.getElevation());
                    
                    mesh.addColor(panoPixels.getColor(((float)x/(float)mapWidth)*panoPixels.getWidth(),
                                                      ((float)y/(float)mapHeight)*panoPixels.getHeight()));
                    mesh.addNormal(ofPoint(plane.x,plane.y,plane.z));
                    mesh.addVertex(vertex);
                }
            }
        }
    }
}

void Prototype02::selfDraw(){
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

void Prototype02::selfDrawOverlay(){
    
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

void Prototype02::selfEnd(){
}

void Prototype02::selfExit(){
    
}

//--------------------------------------------------------------
void Prototype02::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void Prototype02::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void Prototype02::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void Prototype02::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void Prototype02::onMessage( ofxLibwebsockets::Event& args ){    
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
void Prototype02::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;
}

void Prototype02::selfKeyPressed(ofKeyEventArgs & args){

}

void Prototype02::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Prototype02::selfMouseMoved(ofMouseEventArgs& data){
	
}

void Prototype02::selfMousePressed(ofMouseEventArgs& data){

}

void Prototype02::selfMouseDragged(ofMouseEventArgs& data){

}

void Prototype02::selfMouseReleased(ofMouseEventArgs& data){

}