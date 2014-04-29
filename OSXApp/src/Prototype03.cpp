//
//  Prototype03.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "Prototype03.h"

void Prototype03::selfSetup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    player.loadMovie(getDataPath()+"escena01.mov");
    player.setLoopState(OF_LOOP_NORMAL);

    bPlaying = false;
    sphereDefinition = 360;
    
    client.connect("patriciogonzalezvivo.com", 8080);
    client.addListener(this);
}

void Prototype03::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    cameraSet(new UIGameCam());
    guiAdd(grid);
}

void Prototype03::selfGuiEvent(ofxUIEventArgs &e){
    
}

void Prototype03::selfSetupSystemGui(){
    sysGui->addSlider("Sphere_radius", 1, 1000, &sphereRadio);
    sysGui->addIntSlider("Sphere_deff", 36, 360, &sphereDefinition);
}

//---------------------------------------------------

void Prototype03::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    sphereMake();
}

void Prototype03::sphereMake(){
    sphereMesh.clear();
    sphereMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int x = 0; x < sphereDefinition; x ++) {
        const int next_x = x + 1;
        int map_next_x = next_x;
        if (map_next_x >= sphereDefinition)
            map_next_x -= sphereDefinition;
        
        const unsigned int endHeight = (sphereDefinition/2) - 1;
        
        for (unsigned int y = 0; y < (sphereDefinition/2); y ++) {
            const int next_y = y + 1;
            
            spherePoint(next_x, y);
            spherePoint(x, y);
            spherePoint(x, next_y);
            
            spherePoint(x, next_y);
            spherePoint(next_x, next_y);
            spherePoint(next_x, y);
            
        }
    }
}

void Prototype03::selfBegin(){
    sphereMake();
}

void Prototype03::spherePoint(int _x, int _y){
    float rad_azimuth = _x / (float) (sphereDefinition - 1.0f) * TWO_PI;
    float rad_elevation = _y / (float) ((sphereDefinition/2) - 1.0f) * PI;
    
    //Calculate the cartesian position of this vertex (if it was at unit distance)
    ofPoint xyz;
    xyz.x = sin(rad_elevation) * sin(rad_azimuth);
    xyz.y = sin(rad_elevation) * cos(rad_azimuth);
    xyz.z = cos(rad_elevation);
    float distance = sphereRadio;
    
    ofPoint normal;
    normal = xyz;
    normal.normalize();
    
    ofVec2f texCoord = ofVec2f((_x/(float)sphereDefinition)*player.getWidth(),
                               (_y/(float)(sphereDefinition/2))*player.getHeight());
    xyz *= distance;
    
    sphereMesh.addNormal(normal);
    sphereMesh.addTexCoord(texCoord);
    sphereMesh.addVertex(xyz);
}

void Prototype03::selfUpdate(){
    player.update();
    
    {
        string message = "{ \"event\":\"get\" }";
        client.send(message);
    }
}

void Prototype03::selfDraw(){
    materials["MATERIAL 1"]->begin();
    
    ofSetColor(255);
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    grid.draw();
    ofPopMatrix();
    
    float sensorScale = 10;
    
    if (sensors.size()>0){
        ofPushMatrix();
        ofScale(1, -1, 1);
        for (std::map<string,SimpleSensor>::iterator it=sensors.begin(); it!=sensors.end(); ++it){
            ofPushMatrix();
            ofTranslate(0,0,it->second.altitud);
            float angle;
            ofVec3f axis;//(0,0,1.0f);
            it->second.attitude.getRotate(angle, axis);
            ofRotate(angle, axis.x, -axis.y, axis.z); // rotate with quaternion
            
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
            
            ofSetColor(255);
            player.getTextureReference().bind();
            sphereMesh.draw();
            player.getTextureReference().unbind();
            
            ofPopMatrix();
        }
        ofPopMatrix();
    } else {
        ofSetColor(255);
        player.getTextureReference().bind();
        sphereMesh.draw();
        player.getTextureReference().unbind();
    }

    materials["MATERIAL 1"]->end();
}

void Prototype03::selfEnd(){
}

void Prototype03::selfExit(){
    
}

void Prototype03::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        
        bPlaying = !bPlaying;
        
        if(bPlaying){
            sphereMake();
            player.play();
        } else {
            player.stop();
        }
    }
}

void Prototype03::selfKeyReleased(ofKeyEventArgs & args){
	
}

void Prototype03::selfMouseMoved(ofMouseEventArgs& data){
	
}

void Prototype03::selfMousePressed(ofMouseEventArgs& data){

}

void Prototype03::selfMouseDragged(ofMouseEventArgs& data){

}

void Prototype03::selfMouseReleased(ofMouseEventArgs& data){

}

//--------------------------------------------------------------
void Prototype03::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void Prototype03::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void Prototype03::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void Prototype03::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void Prototype03::onMessage( ofxLibwebsockets::Event& args ){
    if(args.json.size()!=sensors.size()){
        sensors.clear();
    }
    
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
        }
    }
}

//--------------------------------------------------------------
void Prototype03::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;
}