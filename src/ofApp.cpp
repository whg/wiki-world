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
    
    vector< vector<float> > output;
    read_fdata("/Users/itg/Desktop/mysql_wiki_order_id.fdata", output);
//    read_fdata("/Users/itg/Desktop/wiki_pages_pg.fdata", output);

    int num = output[0].size();
    points.resize(num);
    ofVec3f v;
    float r = 180;
    for (int i = 0; i < num; i++) {
//        points[i].x = output[0][i];
//        points[i].y = output[1][i];
//        points[i].y = output[1][i];
//        points[i].x = (output[0][i]);

        if (output[0][i] < -180 && output[0][i] > 180) continue;
        
        points[i].x = output[1][i];
        points[i].y = (output[0][i]);
//        points[i].x = output[0][i];
//        points[i].y = (output[1][i]);

        
//        v = ofVec3f(0, 0, r);
//        v.rotate(output[0][i], ofVec3f(-1, 0, 0));
//        v.rotate(output[1][i], ofVec3f(0, 1, 0));
//        v.rotate(output[1][i], output[0][i], 0);
//        points[i] = v;

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
    panel.add(step.set("step", 100, 10, 500));
    panel.add(levelAmount.set("level amount", 100, 10, 500));
    panel.add(levels.set("levels", 100, 10, 200));

    panel.add(q.set("q", 0, 0, 1));
    panel.add(t.set("t", 0, 0, 1));
    panel.add(radius.set("radius", 0, 0, 300));
    panel.add(sphereAlpha.set("sphere alpha", 0, 0, 255));
    panel.add(blackScreen.set("screen", 600, 0, 600));

    drawGui = true;
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
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (counter < 0) return;

//    const int STEP = step;

    counter+= step;
    counter = MIN(counter, points.size());


    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //cam.rotation.y-= 0.5;
}

bool ofVec3fSort (ofVec3f &i, ofVec3f &j) {
    if (i.x == j.x) return i.y < j.y;
    else return i.x < i.x;
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
    
//    glColor4f(0, 0, 0, ofMap(mouseX, 0, ofGetWidth(), 0, 1));
//    ofSetColor(255, 2);
//    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    cam.begin();
    cout << cam.zoom << " " << mouseY << endl;
    
//    ofSetColor(255, sphereAlpha);
//    ofDrawRectangle(0, 0, 0, ofGetWidth(), ofGetHeight());
//    ofDrawSphere(0, 0, 0, radius);
    
    shader.begin();
    shader.setUniform2f("offset", mouseX, blackScreen);
    shader.setUniform2f("window", ofGetWidth(), ofGetHeight());
    shader.setUniform1f("zoom", cam.zoom);
    

    shader.setUniform1f("q", q);
    shader.setUniform1f("radius", radius);
    
//    counter = int(ofMap(mouseX, 0, ofGetWidth(), 0, points.size(), true));
    
    //int levels = 50;
    

//    else if(viewMode == CUMULATIVE) {
        vbo.draw(GL_POINTS, 0, counter);
//    }
    
//    ofSetColor(255, mouseX);
//    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    
    if (viewMode == NEW) {
        for (int i = 0; i < levels; i++) {
     //       ofSetColor(255, 0, 150, i/float(levels)*255);
            vbo.draw(GL_POINTS, MIN(MAX(counter - i * levelAmount, 0), points.size()),  levelAmount);//counter);
        }
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
    cout << part.size() << endl;
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
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == 'r') {
        counter = -500;
//        mesh.clear();
        medians.clear();
    }
    
    if (key < '9' && key >= '0') {
        blendMode = key - '0';
    }

    if (key == ' ') drawGui = !drawGui;
    
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
