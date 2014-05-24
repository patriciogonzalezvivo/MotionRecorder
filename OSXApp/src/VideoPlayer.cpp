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
    
    ofBuffer buffer = ofBufferFromFile("sensor01.csv");
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
            
            if(initMs == -1 && initSc == -1 && initMn == -1 && initHr == -1){
                initMs = Ms; initSc = Sc; initMn = Mn; initHr = Hr;
            }
            
            s.sec = (Hr-initHr)*3600.0+
                    (Mn-initMn)*60.0+
                    (Sc-initSc)+
                    (Ms-initMs)*0.1;
            
            s.attitude.set(ofToFloat(values[1]),
                           ofToFloat(values[2]),
                           ofToFloat(values[3]),
                           ofToFloat(values[4]));
            
            sensor.push_back(s);
        }
	}
}

void VideoPlayer::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
//    cameraSet(new UIGameCam());
    guiAdd(grid);
}

void VideoPlayer::selfGuiEvent(ofxUIEventArgs &e){
    
}

void VideoPlayer::selfSetupSystemGui(){
    sysGui->addSlider("Sphere_radius", 1, 1000, &sphereRadio);
    sysGui->addIntSlider("Sphere_deff", 36, 360, &sphereDefinition);
}

//---------------------------------------------------

void VideoPlayer::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    sphereMake();
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
}

void VideoPlayer::selfDraw(){
    materials["MATERIAL 1"]->begin();
    
    ofSetColor(255);
    ofPushMatrix();
    ofRotate(90, 1, 0, 0);
    grid.draw();
    ofPopMatrix();

    ofSetColor(255);
    player.getTextureReference().bind();
    sphereMesh.draw();
    player.getTextureReference().unbind();

    materials["MATERIAL 1"]->end();
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
            player.play();
        } else {
            player.stop();
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