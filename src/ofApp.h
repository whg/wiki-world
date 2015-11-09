#pragma once

#include "ofMain.h"

#include "ofxGui.h"

#include "ofxTimeline.h"
#include "ofxMaxim.h"

class Cam {
public:
    
    typedef enum { X, Y } RotationDirection;
    
    Cam(): zoom(0), rotation(0), firstDrag(false) {
        //ofRegisterMouseEvents(this);
    }
    
    void mouseMoved(ofMouseEventArgs &args) {}
    void mouseDragged(ofMouseEventArgs &args) {
        
        mouseDiff =  args - lastMousePos;
        
        if (inTranslate) {
            translation+= mouseDiff;
        }
        
//        if (firstDrag) {
//            if (abs(mouseDiff.x) > abs(mouseDiff.y)) {
//                rotationDirection = X;
//            }
//            else {
//                rotationDirection = Y;
//            }
//            //rotation = ofVec2f();
//            firstDrag = false;
//        }
        
        else {
            rotation.y+= mouseDiff.x; 
            rotation.x-= mouseDiff.y;
        }
        
        lastMousePos = ofVec2f(args);
        
    }
    void mousePressed(ofMouseEventArgs &args) {
        lastMousePos.x = args.x;
        lastMousePos.y = args.y;
        
        firstDrag = true;
        
        if (args.button == 1) inTranslate = true;
        else inTranslate = false;
    }
    void mouseReleased(ofMouseEventArgs &args) {}
    void mouseScrolled(ofMouseEventArgs &args) {
        zoom+= args.y;
    }
    void mouseEntered(ofMouseEventArgs &args) {}
    void mouseExited(ofMouseEventArgs &args) {}
    
    void setPosition(float x, float y, float z) {
        
    }
    
    void begin() {
        glPushMatrix();
        glTranslatef(ofGetWidth() * 0.5 + translation.x, ofGetHeight() * 0.5 + translation.y, zoom);
        
//        if (rotationDirection == X) {
        glRotatef(rotation.x, 1, 0, 0);

            glRotatef(rotation.y, 0, 1, 0);
//        }
//        else if (rotationDirection == Y) {
//            glRotatef(rotation.x, 1, 0, 0);
//            glRotatef(rotation.y, 0, 1, 0);
//        }
        
        glScalef(1, -1, 1);
        
    }
    void end() {
        glPopMatrix();
    }
    
    float zoom;
    ofVec2f rotation, translation, lastMousePos, mouseDiff;
    RotationDirection rotationDirection;
    bool firstDrag, inTranslate;
};

inline bool ofVec3fSort (ofVec3f &i, ofVec3f &j) {
    if (i.x == j.x) return i.y < j.y;
    else return i.x < i.x;
}


class ofApp : public ofBaseApp{
private:

    vector<ofVec3f> points;
    vector<ofFloatColor> cols;
    vector<string> names;
    
    typedef enum { CUMULATIVE, NEW } ViewMode;

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioOut( float * output, int bufferSize, int nChannels );

    
    vector< vector<float> > mainData;

    
    ofVbo vbo;
    Cam cam;
    int blendMode;
    
    bool drawGui;
    ofxPanel panel;
    ofParameter<int> step, sphereAlpha, levelAmount, levels;
    ofParameter<float> q, radius, t, blackScreen;
    
    ofShader shader;
    
    ofFbo fbo;
    
//    deque<ofVec3f> medians;
    ofVec3f lastPoint;
    ofMesh medians;
    ViewMode viewMode;
    
    ofxTimeline timeline;
    
    map<string, ofxMaxiSample*> audioSamples;
    map<int, string> soundIds;
    map<int, ofxMaxiSample*> samplesToPlay;
    
    struct Sound {
        ofxMaxiSample *sample;
        string name;
        float pan;
    };
    
    deque<Sound> sampleQueue;

    void loadAudio();
    
    ofMutex audioMutex;
};
