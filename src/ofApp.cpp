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
    
    std::ifstream infile(ofToDataPath("point_file").c_str());
    
    vector< vector<float> > output;
    read_fdata("/Users/itg/Desktop/mysql_wiki_order_id.fdata", output);

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

        
        v = ofVec3f(0, 0, r);
        v.rotate(output[0][i], ofVec3f(-1, 0, 0));
        v.rotate(output[1][i], ofVec3f(0, 1, 0));
//        v.rotate(output[1][i], output[0][i], 0);
//        points[i] = v;

    }
    
//    std::string line;
//    int i = 0;
//    float x, y;
//    int d = 180;
//    float m = 2;
//    while (std::getline(infile, line)) {
//        vector<string> parts = ofSplitString(line, "|");
//        if (parts.size() < 2) break;
//        x = ofToFloat(trim(parts[0]));
//        y = ofToFloat(trim(parts[1]));
//        if (x > -d && x < d && y > -d && y < d) {
//            points.push_back(ofVec2f(x*m, y*m));
////            names.push_back(trim(parts[2]));
//        }
//
////        if (i++ >= 100000) break;
//        // process pair (a,b)
//    }
    cout << "read file " << points.size() << endl;
    n = points.size();
//    cout << points.size() << " points" << endl;
//    for (auto v : points) {
//        mesh.add
//    }

    
//    ofRectangle bounds = ofRectangle(-d*m, -d*m, d*m*2, d*m*2);
    
    int pointCount = 255;
    int seed = 33;
    

    cout << "computed voronoi" << endl;
    
//    vbo.setUsage(GL_STATIC_DRAW);
    //mesh.addVertices(points);
//    for(int i=0; i<cells.size(); i++) {
//        for(int j=0; j<cells[i].pts.size(); j++) {
//            ofPoint lastPt = cells[i].pts[cells[i].pts.size()-1];
//            if(j > 0) {
//                lastPt = cells[i].pts[j-1];
//            }
//            ofPoint thisPt = cells[i].pts[j];
//            
//            if(!isBorder(lastPt) || !isBorder(thisPt)) {
////                ofDrawLine(lastPt, thisPt);
//                mesh.addVertex(lastPt);
//                mesh.addVertex(thisPt);
//            }
//        }
//    }

//    vbo.setMode(OF_PRIMITIVE_POINTS);
    cout << "made mesh" << endl;
    
    cam.setPosition(0, 0, 700);
//    cam.setPosition(-192.449539, 84.9406509, 84.6877899);
    cam.setTranslationKey('t');
    
    ofSetVerticalSync(true);
//    ofSetFrameRate(60);
  
    vbo.setVertexData(&points[0], points.size(), GL_STATIC_DRAW);
    cols.resize(points.size());
    for (int i = 0; i < points.size(); i++) {
//        vbo.add
//        vbo.addVertex(points[counter++]);
        cols[i] = ofFloatColor::fromHsb(i/n, 0.8, 0.6, 0.5);
    }
    vbo.setColorData(&cols[0], cols.size(), GL_STATIC_DRAW);
    
    panel.setup("gui");
    panel.add(step.set("step", 100, 10, 500));
    drawGui = true;
    
    shader.setupShaderFromFile(GL_FRAGMENT_SHADER, ofToDataPath("shader.frag"));
    shader.setupShaderFromFile(GL_VERTEX_SHADER, ofToDataPath("shader.vert"));
    shader.linkProgram();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (counter < 0) return;

//    const int STEP = step;

    counter+= step;
    counter = MIN(counter, points.size());
//    if (counter < points.size()) {
//        for (int i = 0; i < STEP; i++) {
//            mesh.addVertex(points[counter++]);
//            mesh.addColor(ofFloatColor::fromHsb(counter/n , 1.0, 1.0, 0.4));
//        }
//
//    }

    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    if (counter < 0) {
        counter+= 10;
        return;
    }


    
    ofEnableAlphaBlending();

    ofEnableBlendMode(ofBlendMode(blendMode));
//    voronoi.draw();
    
    // Or feel free to draw the voronoi diagram yourself:
        
//    ofSetLineWidth(0);
//    ofFill();
//    
//    // Draw bounds
//    ofSetColor(220);
//    ofRect(bounds);
//
    
//    ofBackground(255);
    cam.begin();

    shader.begin();

    
    shader.setUniform2f("offset", mouseX, mouseY);
//    ofSetColor(0);

//    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
//    if (ofGetKeyPressed()) {
////        vbo.draw(OF_MESH_POINTS);
//    }
//    else {
//        mesh.draw(OF_MESH_WIREFRAME);
//        vbo.draw(GL_POINTS, 0, int((float(mouseX) / ofGetWidth()) * points.size()));
    vbo.draw(GL_POINTS, 0, counter);
//        mesh.drawInstanced(OF_MESH_POINTS, int((mouseX / ofGetWidth()) * points.size()));
//    }
    
    shader.end();
    cam.end();

//    ofDrawBitmapString(names[counter], 10, 10);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(counter) + " : " + ofToString(points.size()) , 10, 20);
    
//    for(int i=0; i<cells.size(); i++) {
//        // Draw cell borders
////        ofSetColor(120);
//        for(int j=0; j<cells[i].pts.size(); j++) {
//            ofPoint lastPt = cells[i].pts[cells[i].pts.size()-1];
//            if(j > 0) {
//                lastPt = cells[i].pts[j-1];
//            }
//            ofPoint thisPt = cells[i].pts[j];
//            
//            if(!isBorder(lastPt) || !isBorder(thisPt)) {
//                ofDrawLine(lastPt, thisPt);
//            }
//        }
//        
//        // Draw cell points
////        ofSetColor(180, 0, 0);
////        ofFill();
////        ofCircle(cells[i].pt, 2);
//    }

    if (drawGui) panel.draw();
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == 'r') {
        counter = -500;
//        mesh.clear();
    }
    
    if (key < '9' && key >= '0') {
        blendMode = key - '0';
    }

    if (key == ' ') drawGui = !drawGui;
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
    cols.clear();
    cols.resize(points.size());
    for (int i = 0; i < points.size(); i++) {
        //        vbo.add
        //        vbo.addVertex(points[counter++]);
        cols[i] = ofFloatColor::fromHsb(i/n, 0.8, 0.6, float(mouseX)/ofGetWidth());
    }
    vbo.setColorData(&cols[0], cols.size(), GL_STATIC_DRAW);

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
