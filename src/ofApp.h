#pragma once

#include "ofMain.h"

#include "ofxGui.h"

class Cam {
public:
    
    typedef enum { X, Y } RotationDirection;
    
    Cam(): zoom(0), rotation(0), firstDrag(false) {
        ofRegisterMouseEvents(this);
    }
    
    void mouseMoved(ofMouseEventArgs &args) {}
    void mouseDragged(ofMouseEventArgs &args) {
        
        mouseDiff = lastMousePos - args;
        
        if (firstDrag) {
            if (abs(mouseDiff.x) > abs(mouseDiff.y)) {
                rotationDirection = X;
            }
            else {
                rotationDirection = Y;
            }
            //rotation = ofVec2f();
            firstDrag = false;
        }
        
        if (rotationDirection == X) {
            rotation.y-= mouseDiff.x;
            lastMousePos.x = args.x;
        }
        else if (rotationDirection == Y) {
            rotation.x-= mouseDiff.y;
            lastMousePos.y = args.y;
        }
        
        
    }
    void mousePressed(ofMouseEventArgs &args) {
        lastMousePos.x = args.x;
        lastMousePos.y = args.y;
        
        firstDrag = true;
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
        glTranslatef(ofGetWidth() * 0.5, ofGetHeight() * 0.5, zoom);
        
//        if (rotationDirection == X) {
            glRotatef(rotation.y, 0, 1, 0);
            glRotatef(rotation.x, 1, 0, 0);
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
    ofVec2f rotation, lastMousePos, mouseDiff;
    RotationDirection rotationDirection;
    bool firstDrag;
};

class ofApp : public ofBaseApp{
private:

    vector<ofVec3f> points;
    vector<ofFloatColor> cols;
    vector<string> names;

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofVbo vbo;
    Cam cam;
    int blendMode;
    
    bool drawGui;
    ofxPanel panel;
    ofParameter<int> step, sphereAlpha;
    ofParameter<float> q, radius, t;
    
    ofShader shader;
};
