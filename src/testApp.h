#pragma once

#include "ofMain.h"
#include "ofMesh.h"
#include "ofxCsv.h"
#include "ofxUI.h"

using namespace wng;


struct flap {
    string attemptId;
    vector<int> sequence;
    int tapFreq;
};


class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
    ofxUICanvas *gui;
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    
    ofxCsv csv;
    ofMesh flapMesh;
    ofPolyline lines[20000];
    ofPolyline paths[20000];
    ofPolyline linesBestF20[60000];
    ofPolyline linesBestF60Diff[60000];
    ofPolyline linesBestF60DiffC[60000];
    ofPolyline linesBestFit[3];
    
    ofEasyCam camera;
    bool zoomRating = false, zoomPrice = false, firstPrint = true;
    
    string modeTypeName = "Best Plyers: 1st 60 Taps";
    
    int flapCount = 0;
    int plyrCount = 0;
    vector<flap> flaps;
    
    int firstFlap[100];
    int distSurvived[60];
    int flapCounts[30][20000];
    int flapCountsBest[60][20000];
    int lstFrmCnts[20000];
    int lstFrmCntsBest[20000];
    int maxEnd = 0;
    int maxEndBest = 0;
    int numTop = 0;
    int bestLevelLimit = 40;
    
    int diffBetweenFlaps[60][2000];
    int bestMove[60];
    
};