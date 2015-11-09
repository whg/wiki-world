#include "ofApp.h"

#include "fdata.h"

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


int counter = 0;

float HC = 180.0/PI;
float QP = PI/4.0;
float QC = PI / 360.0;

float lat2y(float a) {
    return HC * log(tan(QP + a * QC));
}




//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(255);
    
    int maj, min;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);
    printf("OpenGL version %d.%d\n", maj, min);
    cout << glGetString(GL_VERSION) << endl;
//    ofExit();
    
    std::ifstream infile(ofToDataPath("point_file").c_str());
    
    read_fdata("/Users/itg/Desktop/mysql_wiki_order_id_with_id.fdata", mainData);
//    read_fdata("/Users/itg/Desktop/wiki_pages_pg.fdata", mainData);
    
    
    int num = mainData[0].size();
    points.resize(num);
    ofVec3f v;
    float r = 180;
    for (int i = 0; i < num; i++) {

        if (mainData[0][i] < -180 && mainData[0][i] > 180) continue;
        
        points[i].x = mainData[1][i];
        points[i].y = (mainData[0][i]);
//        points[i].x = mainData[0][i];
//        points[i].y = (mainData[1][i]);
    }
    
    cam.setPosition(0, 0, 700);
//    cam.setPosition(-192.449539, 84.9406509, 84.6877899);
//    cam.setTranslationKey('t');
    
    ofSetVerticalSync(true);
//    ofSetFrameRate(60);
  
    vbo.setVertexData(&points[0], points.size(), GL_STATIC_DRAW);
    cols.resize(points.size());
    float n = points.size();
    for (int i = 0; i < points.size(); i++) {
//        vbo.add
//        vbo.addVertex(points[counter++]);
        cols[i] = ofFloatColor::fromHsb(i/n, 0.8, 0.6, 0.5);
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
    blendMode = 1;
    shader.setupShaderFromFile(GL_FRAGMENT_SHADER, ofToDataPath("shader.frag"));
    shader.setupShaderFromFile(GL_VERTEX_SHADER, ofToDataPath("shader.vert"));
    shader.linkProgram();
    
    cam.zoom = 382;
    cam.translation.y = 20;
    blendMode = OF_BLENDMODE_ADD;
    
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA32F);
    fbo.begin();
    ofClear(0);
    fbo.end();
    
//    ofSetBackgroundAuto(false);
//    ofBackground(0);
    lastPoint = points[0];
    viewMode = CUMULATIVE;
    
    
    timeline.setup();
    timeline.setDurationInSeconds(100);
    timeline.setFrameBased(false);
    timeline.setFrameRate(60);
    timeline.addCurves("counter", ofRange(0, points.size()), 0);
    timeline.addCurves("x trans", ofRange(-100, 100), 0);
    timeline.addCurves("y trans", ofRange(-25, 25), 0);
    timeline.addCurves("zoom", ofRange(300, 500));
    timeline.addCurves("blackScreen", ofRange(0, 300));
    timeline.addCurves("sphereness", ofRange(0.0, 1.0));
    timeline.addCurves("y rot", ofRange(0, 360));
    timeline.addCurves("x rot", ofRange(-20, 20));
    
    timeline.setSpacebarTogglePlay(true);
    timeline.play();
    timeline.hide();

    counter = 0;
    
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
        counter+= 10;
        return;
    }
    
    //ofEnableDepthTest();
    
//    fbo.begin();
    
    ofEnableAlphaBlending();
    ofEnableBlendMode(ofBlendMode(blendMode));

    cam.begin();

    
    shader.begin();
    //shader.setUniform2f("offset", mouseX, mouseY);
    shader.setUniform2f("window", ofGetWidth(), ofGetHeight());
    shader.setUniform1f("zoom", timeline.getValue("zoom"));
    
    shader.setUniform1f("blackScreen", timeline.getValue("blackScreen"));
    
    shader.setUniform1f("q", timeline.getValue("sphereness"));
    shader.setUniform1f("radius", 100);
    
//    counter = int(ofMap(mouseX, 0, ofGetWidth(), 0, points.size(), true));
    
    //int levels = 50;
    


    
//    ofSetColor(255, mouseX);
//    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    counter = timeline.getValue("counter");
    
    
    if (viewMode == NEW) {
        for (int i = 0; i < levels; i++) {
     //       ofSetColor(255, 0, 150, i/float(levels)*255);
            vbo.draw(GL_POINTS, MIN(MAX(counter - i * levelAmount, 0), points.size()),  levelAmount);//counter);
        }
    }
    shader.setUniform2f("offset", mouseX, 35);

    if(viewMode == CUMULATIVE) {
        vbo.draw(GL_POINTS, 0, counter);
    }
    
    
    shader.end();
    
    
    int start = counter;
    int end = counter+levelAmount;
    
    vector<ofVec3f> part;
    part.resize(levelAmount);
    copy(points.begin()+counter, points.begin()+counter+levelAmount, part.begin());
    sort(part.begin(), part.end(), ofVec3fSort);
    
    ofVec3f sum;
    for (int i = start; i < end; i++) {
        sum+= points[i];
    }
    
    sum /= levelAmount;
    ofSetColor(255, 0, 150, mouseX);
//    cout << part.size() << endl;
    ofVec3f median = part[part.size()/2];
    
    float mindist = 10000;
    ofVec3f closest;
    for (auto &p : part) {
        float d = lastPoint.distance(p);
        if (d < mindist) {
            closest = p;
            mindist = d;
        }
    }
    
    
    
//    medians.addVertex(closest);
//    
//    medians.setMode(OF_PRIMITIVE_LINE_STRIP);
//    medians.drawWireframe();
    
   // ofDrawCircle(median.x, median.y, 1);
//    ofSetColor(255);
//    vbo.draw(GL_POINTS, MAX(counter, 0),  levelAmount);//counter);


    cam.end();
    
//    fbo.end();
//    fbo.draw(0, 0);

//    ofDrawBitmapString(names[counter], 10, 10);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(counter) + " : " + ofToString(points.size()) , 10, 20);

    ofDisableDepthTest();
    if (drawGui) panel.draw();
    
    ofSetColor(255);
    timeline.draw();
    
    static int lastId = 0, lastCounter = 0;
    
    audioMutex.lock();
    
    vector<float> &ids = mainData[2];
    int currentId = ids[counter];
    
    //printf("looking in between %d : %d\n", lastId, currentId);
    
    static float timeNow, addedAt = 0;
    timeNow = ofGetElapsedTimef();
    
    int tried = 0, placed = 0;
    for (auto &pair : soundIds) {
        int id = pair.first;
        if (id < currentId && id > lastId) {
            if (audioSamples.count(pair.second) > 0 && samplesToPlay.count(id) == 0) {
                if (timeNow - addedAt > 0.4) {
                    samplesToPlay[id] = audioSamples[pair.second];
                    //cout << timeNow << " - " << addedAt << endl;
                    placed++;
                    addedAt = timeNow;
                    //cout << timeNow << " - " << addedAt << endl;

                }
                Sound sound;
                sound.sample = audioSamples[pair.second];
                sound.name = pair.second;
                sound.pan = 0.5;
                for (int i = lastCounter; i < counter; i++) {
                    if (ids[i] == id) {
                        sound.pan = ofMap(mainData[1][i], -180, 180, 0, 1);// / 360.0f + 0.5;
                        break;
                    }
                }
                if (sound.pan == 10) {
                    cout << "shit\n";
                }
                sampleQueue.push_back(sound);
                tried++;
//            cout << "added " << pair.first << endl;
            }
        }
    }
    
    lastId = currentId;
    lastCounter = counter;
    //printf("tried %d, placed %d\n", tried, placed);
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
        else {
            printf("playing %s at %f (%d left)\n", sampleQueue.begin()->name.c_str(), sampleQueue.begin()->pan, int(sampleQueue.size()));
        }
    }
    
    
    audioMutex.unlock();
    
    //cout << samplesToPlay.size() << " samples to play\n";
    
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == 'r') {
        counter = -500;
//        mesh.clear();
        medians.clear();
        for (auto &pair : audioSamples) {
            pair.second->reset();
        }
    }
    
    if (key < '9' && key >= '0') {
        blendMode = key - '0';
    }

    if (key == 't') {
//        drawGui = !drawGui;
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
            //        vbo.add
            //        vbo.addVertex(points[counter++]);
            cols[i] = ofFloatColor::fromHsb(i/n, 0.8, 0.6, float(mouseX)/ofGetWidth());
        }
        vbo.setColorData(&cols[0], cols.size(), GL_STATIC_DRAW);
    }
    
   // sample.reset();

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
        
//        for (auto &pair : samplesToPlay) {
////        if (samplesToPlay.size()) {
////            ofxMaxiSample *sample = samplesToPlay.begin()->second; // pair.second;
//            ofxMaxiSample *sample = pair.second;
//            if (sample != NULL) {
//                v+= sample->playOnce() * 0.5;
//            }
//        }
        //if (samplesToPlay.size() > 0) v = samplesToPlay[0]->playOnce();
        //output[i*nChannels] = output[i*nChannels+1]= v * 0.6;
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
    
    
//    DIR *dir;
//    struct dirent *ent;
//    if ((dir = opendir (path.c_str())) != NULL) {
//                while ((ent = readdir (dir)) != NULL) {
//            string file = ent->d_name;
//                    string filepath = path + file;
//                    ifstream infile(filepath);
//            if(file.find(".wav") !=-1 && infile.good()) {
//                
//                
//                ofxMaxiSample *sample = new maxiSample();
//                sample->load(path + file);
//                audioSamples[file] = sample;
//                
//            }
//        }
//        closedir (dir);
//    } else {
//        cout << "Error, could not load directory" << endl;
//    }
    cout << "loaded "  << soundIds.size() << endl;
    cout << "loaded "  << audioSamples.size() << endl;
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
