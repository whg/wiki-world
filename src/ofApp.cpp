#include "ofApp.h"

#include "fdata.h"


//--------------------------------------------------------------
void ofApp::setup(){

    
//    read_fdata("/Users/itg/Desktop/pg_wiki_order_id_with_id.fdata", mainData);
    read_fdata("/Users/itg/Desktop/mysql_wiki_order_id_with_id_primary.fdata", mainData);
//    read_fdata("/Users/itg/Desktop/mysql_wiki_order_gt_id_with_id.fdata", mainData);
    
    
    
    int num = mainData[0].size();
    points.resize(num);
    ofVec3f v;
    float r = 180;
    for (int i = 0; i < num; i++) {

        if (mainData[0][i] < -180 && mainData[0][i] > 180) continue;
        
        points[i].x = mainData[1][i];
        points[i].y = (mainData[0][i]);

    }
    

    ofSetVerticalSync(true);
//    ofSetFrameRate(60);
  
    vbo.setVertexData(&points[0], points.size(), GL_STATIC_DRAW);
    cols.resize(points.size());
    float n = points.size();
    for (int i = 0; i < points.size(); i++) {
        cols[i] = ofFloatColor::fromHsb(i/n, 1.0, 1.0);
    }
    vbo.setColorData(&cols[0], cols.size(), GL_STATIC_DRAW);
    
    panel.setup("gui");
    panel.add(step.set("step", 2, 1, 100));
    panel.add(levelAmount.set("level amount", 100, 10, 500));
    panel.add(levels.set("levels", 100, 10, 200));

    panel.add(q.set("q", 0, 0, 1));
    panel.add(t.set("t", 0, 0, 1));
    panel.add(radius.set("radius", 0, 0, 300));
    panel.add(sphereAlpha.set("sphere alpha", 0, 0, 255));
    panel.add(blackScreen.set("screen", 600, 0, 600));

    drawGui = false;
    shader.setupShaderFromFile(GL_FRAGMENT_SHADER, ofToDataPath("shader.frag"));
    shader.setupShaderFromFile(GL_VERTEX_SHADER, ofToDataPath("shader.vert"));
    shader.linkProgram();
    
    cam.zoom = 382;
    cam.translation.y = 20;
    blendMode = OF_BLENDMODE_ALPHA;
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F);
    fbo.begin();
    ofClear(0);
    fbo.end();
    

    lastPoint = points[0];
    viewMode = CUMULATIVE;
    
    
    timeline.setup();
    timeline.setDurationInSeconds(105);
    timeline.setFrameBased(false);
    timeline.setFrameRate(60);
    timeline.addCurves("counter", ofRange(0, points.size()), 0);
    timeline.addCurves("crossfader", ofRange(0, 0.7), 0.7);
    timeline.addCurves("x trans", ofRange(-100, 100), 0);
    timeline.addCurves("y trans", ofRange(-25, 25), 0);
    timeline.addCurves("zoom", ofRange(300, 500));
    timeline.addCurves("blackScreen", ofRange(0, 300));
    timeline.addCurves("sphereness", ofRange(0.0, 1.0));
    timeline.addCurves("y rot", ofRange(0, 360));
    timeline.addCurves("x rot", ofRange(-20, 20));
    timeline.addCurves("general alpha", ofRange(0, 1));
    timeline.addCurves("blend mode", ofRange(0, 3));
    timeline.addFlags("asdf");
    
    
    timeline.setSpacebarTogglePlay(true);

    timeline.hide();
    
    loadAudio();
    
    //.load("/Users/itg/Desktop/oggs/wavs/44.1k/Zh-Hunan.wav");
    ofSoundStreamSetup(2, 0, 44100, 512, 2);
}

void ofApp::exit() {
    
    ofSoundStreamClose();
    
    for (auto &pair : audioSamples) {
        delete pair.second;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (counter < 0) return;

//    const int STEP = step;

    counter+= step;
    counter = MIN(counter, points.size());


    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //cam.rotation.y-= 0.5;
    
    cam.rotation.y = timeline.getValue("y rot");
    cam.rotation.x = timeline.getValue("x rot");
    cam.translation.x = timeline.getValue("x trans");
    cam.translation.y = timeline.getValue("y trans");
    cam.zoom = timeline.getValue("zoom");

}


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    if (counter < 0) {
        counter+= 5;
        return;
    }
    

    
    ofEnableAlphaBlending();
    ofEnableBlendMode(ofBlendMode(int(timeline.getValue("blend mode"))));

    cam.begin();

    
    float crossfader = timeline.getValue("crossfader"); //ofMap(mouseX, 0, ofGetWidth(), 0, 1);
    
    float faderOne = (0.5 - MIN(crossfader, 0.5)) * 2.0;
    float faderTwo = (MAX(crossfader, 0.5) - 0.5) * 2.0;
    cout << faderOne << " : " << faderTwo << endl;
    
    if (faderOne > 0) {
        shader.begin();
        shader.setUniform2f("window", ofGetWidth(), ofGetHeight());
        shader.setUniform1f("zoom", timeline.getValue("zoom"));
        shader.setUniform1f("blackScreen", timeline.getValue("blackScreen"));
        shader.setUniform1f("q", timeline.getValue("sphereness"));
        shader.setUniform1f("radius", 100);
        
        counter = timeline.getValue("counter");
        
        
        for (int i = 0; i < levels; i++) {
            ofSetColor(255, 0, 150, i/float(levels)*255);
            vbo.draw(GL_POINTS, MIN(MAX(counter - i * levelAmount, 0), points.size()),  levelAmount);//counter);
        }
        shader.setUniform2f("offset", mouseX, faderOne);

        shader.end();
        
    }
    
    if (faderTwo > 0) {
        shader.begin();
        shader.setUniform2f("window", ofGetWidth(), ofGetHeight());
        shader.setUniform1f("zoom", timeline.getValue("zoom"));
        shader.setUniform1f("blackScreen", timeline.getValue("blackScreen"));
        shader.setUniform1f("q", timeline.getValue("sphereness"));
        shader.setUniform1f("radius", 100);
        
        counter = timeline.getValue("counter");

        shader.setUniform2f("offset", mouseX, faderTwo);
        
        vbo.draw(GL_POINTS, 0, counter);
        
        shader.end();
    }
    
//    int start = counter;
//    int end = counter+levelAmount;
//    
//    vector<ofVec3f> part;
//    part.resize(levelAmount);
//    copy(points.begin()+counter, points.begin()+counter+levelAmount, part.begin());
//    sort(part.begin(), part.end(), ofVec3fSort);
//    
//    ofVec3f sum;
//    for (int i = start; i < end; i++) {
//        sum+= points[i];
//    }
//    
//    sum /= levelAmount;
//    ofSetColor(255, 0, 150, mouseX);
////    cout << part.size() << endl;
//    ofVec3f median = part[part.size()/2];
//    
//    float mindist = 10000;
//    ofVec3f closest;
//    for (auto &p : part) {
//        float d = lastPoint.distance(p);
//        if (d < mindist) {
//            closest = p;
//            mindist = d;
//        }
//    }
//    

    cam.end();
    

    ofSetColor(200);
    ofDrawBitmapString(ofToString(counter/1000) + string("k") , 10, ofGetHeight()-10);

    ofDisableDepthTest();
    if (drawGui) panel.draw();
    
    ofSetColor(255);
    timeline.draw();
    
    static int lastId = 0, lastCounter = 0;
    
    audioMutex.lock();
    
    vector<float> &ids = mainData[2];
    int currentId = ids[counter];
    
    
    static float timeNow, addedAt = 0;
    timeNow = ofGetElapsedTimef();
    
    int tried = 0, placed = 0;
    for (auto &pair : soundIds) {
        int id = pair.first;
        if (id < currentId && id > lastId) {
            if (audioSamples.count(pair.second) > 0 && samplesToPlay.count(id) == 0) {
//                if (timeNow - addedAt > 0.4) {
//                    samplesToPlay[id] = audioSamples[pair.second];
//                    //cout << timeNow << " - " << addedAt << endl;
//                    placed++;
//                    addedAt = timeNow;
//                    //cout << timeNow << " - " << addedAt << endl;
//
//                }
//                
                Sound sound;
                sound.sample = audioSamples[pair.second];
                sound.name = pair.second;
                sound.pan = 0.5;
                for (int i = lastCounter; i < counter; i++) {
                    if (ids[i] == id) {
                        // do a hard pan!
                        sound.pan = ofMap(mainData[1][i], -180, 180, -0.5, 1.5);// / 360.0f + 0.5;
                        break;
                    }
                }
                if (sound.pan == 10) {
                    cout << "shit\n";
                }
                sampleQueue.push_back(sound);
                tried++;
            }
        }
    }
    
    lastId = currentId;
    lastCounter = counter;
    vector<int> toErase;
    for (auto &pair : samplesToPlay) {
        
        if (pair.second == NULL || pair.second->hasFinished) {
            toErase.push_back(pair.first);
        }
    }
    
    for (int id : toErase) {
        samplesToPlay.erase(id);

    }
    
    if (sampleQueue.size()) {
        if (sampleQueue.begin()->sample->hasFinished) {
            sampleQueue.pop_front();

        }
    }
    
    
    audioMutex.unlock();
    
    
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == 'r') {
        counter = -500;
        medians.clear();
        for (auto &pair : audioSamples) {
            pair.second->reset();
        }
    }
    
    if (key < '9' && key >= '0') {
        blendMode = key - '0';
    }

    if (key == 't') {
        timeline.toggleShow();

    }
    if (key == 'g') drawGui = !drawGui;
    
    if (key == 'z') viewMode = CUMULATIVE;
    else if (key == 'x') viewMode = NEW;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    

    if (button == 2) {
        cols.clear();
        cols.resize(points.size());
        float n = points.size();
        
        for (int i = 0; i < points.size(); i++) {
             cols[i] = ofFloatColor::fromHsb(i/n, 0.8, 0.6, float(mouseX)/ofGetWidth());
        }
        vbo.setColorData(&cols[0], cols.size(), GL_STATIC_DRAW);
    }
    

}

void ofApp::audioOut( float * output, int bufferSize, int nChannels ) {
    
    ofScopedLock lock(audioMutex);
    
    static float v;
    static maxiMix mixer;
    static double p[2];
    
    
    for (int i = 0; i < bufferSize; i++) {
      //  v = sample.playOnce();
        v = 0;

        if (sampleQueue.size()) {
            v = sampleQueue.begin()->sample->playOnce();
            mixer.stereo(v*0.7, p, sampleQueue.begin()->pan);
        }
        
        output[i*nChannels] = p[0];
        output[i*nChannels+1] = p[1];
        
    }
}

#include <dirent.h>

void ofApp::loadAudio() {

    
    ifstream infile("/Users/itg/Desktop/sound_ids2");
    string line;
    while(getline(infile, line)) {
        vector<string> parts = ofSplitString(line, ",");
        int id = ofToInt(parts[1]);
        soundIds[id] = parts[0];
    }
    
    string path = "/Users/itg/Desktop/oggs/wavs/44.1k/short/normalised/";


    for (auto &pair : soundIds) {
        string file = pair.second;
        
        string filepath = path + file;
        ifstream infile(filepath);
        
        if (infile.good()) {
            ofxMaxiSample *sample = new maxiSample();
            sample->load(path + file);
            audioSamples[file] = sample;
        }
    }
    

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
