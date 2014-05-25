//
//  VideoPlayer.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "VideoPlayer.h"

#ifdef USE_GSTREAMER
#include "ofGstVideoPlayer.h"
#endif

void VideoPlayer::selfSetup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    
#ifdef USE_GSTREAMER
    player.setPlayer(ofPtr<ofGstVideoPlayer>(new ofGstVideoPlayer));
#endif
    
    player.loadMovie(getDataPath()+"escena01.mov");
    player.setLoopState(OF_LOOP_NORMAL);

    bPlaying = false;
    sphereDefinition = 360;
    
    float initMs = -1;
    float initSc = -1;
    float initMn = -1;
    float initHr = -1;
    
    ofBuffer buffer = ofBufferFromFile(getDataPath()+"sensor01.csv");
    while (!buffer.isLastLine()) {
		string line = buffer.getNextLine();
		
		//Split line into strings
		vector<string> values = ofSplitString(line, ",");
		
        if(values.size()>1){
            FrameRecord s;
            
            vector<string> time = ofSplitString(values[0], "-");
//            2014-05-09-13-12-31-672
//            0    1  2  3  4  5  6
//
            float Ms,Sc,Mn,Hr;
            Ms = ofToFloat(time[6]);
            Sc = ofToFloat(time[5]);
            Mn = ofToFloat(time[4]);
            Hr = ofToFloat(time[3]);
            
            if(initMs == -1){
                initMs = Ms;
                initSc = Sc;
                initMn = Mn;
                initHr = Hr;
            }
            
            s.sec = (Hr-initHr)*3600.0+
                    (Mn-initMn)*60.0+
                    (Sc-initSc)+
                    (Ms-initMs)*0.001;
            
            s.attitude.set(ofToFloat(values[1]),
                           ofToFloat(values[2]),
                           ofToFloat(values[3]),
                           ofToFloat(values[4]));
            
            sensor.push_back(s);
        }
	}
    
    frame = 0;
}

void VideoPlayer::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
//    cameraSet(new UIGameCam());
    guiAdd(grid);
}

void VideoPlayer::selfGuiEvent(ofxUIEventArgs &e){
    
}

void VideoPlayer::selfSetupSystemGui(){
    sysGui->addLabel("Sphere");
    sysGui->addSlider("Sphere_radius", 1, 1000, &sphereRadio);
    sysGui->addIntSlider("Sphere_deff", 36, 360, &sphereDefinition);
    sysGui->addLabel("Offset");
    sysGui->addSlider("Seconds", 0, 1200, &timeOffSet);
    sysGui->addSlider("x", -1, 1, &xOffSet);
    sysGui->addSlider("y", -1, 1, &yOffSet);
    sysGui->addSlider("z", -1, 1, &zOffSet);
    sysGui->addSlider("w", -1, 1, &wOffSet);
    
}

//---------------------------------------------------

void VideoPlayer::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name.find("Sphere") == 0){
        sphereMake();
    } else if (name == "Seconds"){
        frame = 0;
    }
}

void VideoPlayer::sphereMake(){
    sphereMesh.clear();
    sphereMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for (int x = 0; x < sphereDefinition; x ++) {
        const int next_x = x + 1;
        int map_next_x = next_x;
        if (map_next_x >= sphereDefinition)
            map_next_x -= sphereDefinition;
        
        const unsigned int endHeight = (sphereDefinition/2);
        
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

void VideoPlayer::selfBegin(){
    sphereMake();
}

void VideoPlayer::spherePoint(int _x, int _y){
    float rad_azimuth = _x / (float) (sphereDefinition) * TWO_PI;
    float rad_elevation = _y / (float) ((sphereDefinition/2)) * PI;
    
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

void VideoPlayer::selfUpdate(){
    player.update();
    
    {
        time = player.getDuration()*player.getPosition()+timeOffSet;
        
        if (frame == sensor.size()-1) {
            frame = 0;
        }
        
        for (int i = frame; i < sensor.size(); i++) {
            if(sensor[i].sec > time){
                attitude = sensor[i].attitude;
                frame = i-1;
                break;
            } else {
                frame = i;
            }
        }
    }
}

void VideoPlayer::selfDraw(){
    materials["MATERIAL 1"]->begin();
    
    ofSetColor(255);
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    grid.draw();
    ofPopMatrix();

    ofSetColor(255);
    
    float angle;
    ofVec3f axis;
    attitude.getRotate(angle, axis);
    
    ofPushMatrix();
    ofRotate(angle, axis.x, axis.y, axis.z);
    player.getTextureReference().bind();
    sphereMesh.draw();
    player.getTextureReference().unbind();
    ofPopMatrix();

    materials["MATERIAL 1"]->end();
}

void VideoPlayer::selfDrawOverlay(){
    if(bDebug){
        int start = frame-ofGetWidth()*0.5;
        
        ofSetColor(255);
        ofLine(ofGetWidth()*0.5,0,
               ofGetWidth()*0.5,200);
        ofDrawBitmapString(ofToString(time), ofGetWidth()*0.5-30,215);
        
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(0, ofGetHeight()*0.1);
        
        int x = 0;
        if(start < 0){
            ofTranslate(-start, 0);
            start = 0;
        }
        
        float amp = 0.1;
        int end = start+ofGetWidth();
        
        ofPolyline roll,pitch,yaw;
        for (int i = start; i < end && i < sensor.size(); i++ ) {
            ofVec3f euler = sensor[i].attitude.getEuler();
            
            roll.addVertex(ofPoint(x,euler.x*amp - 50));
            pitch.addVertex(ofPoint(x,euler.y*amp ));
            yaw.addVertex(ofPoint(x,euler.z*amp + 50));
            
            x++;
        }
        
        ofSetColor(255, 0, 0);
        roll.draw();
        
        ofSetColor(0, 255, 0);
        pitch.draw();
        
        ofSetColor(0, 0, 255);
        yaw.draw();
        
        ofPopStyle();
        ofPopMatrix();
    }
}


void VideoPlayer::selfEnd(){
}

void VideoPlayer::selfExit(){
    
}

void VideoPlayer::selfKeyPressed(ofKeyEventArgs & args){
    if(args.key == ' '){
        
        bPlaying = !bPlaying;
        
        if(bPlaying){
            sphereMake();
            player.setPaused(false);
            player.play();
        } else {
            player.setPaused(true);
        }
    }
}

void VideoPlayer::selfKeyReleased(ofKeyEventArgs & args){
	
}

void VideoPlayer::selfMouseMoved(ofMouseEventArgs& data){
	
}

void VideoPlayer::selfMousePressed(ofMouseEventArgs& data){

}

void VideoPlayer::selfMouseDragged(ofMouseEventArgs& data){

}

void VideoPlayer::selfMouseReleased(ofMouseEventArgs& data){

}