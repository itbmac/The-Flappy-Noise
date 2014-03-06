
#include "testApp.h"
#include "ofMesh.h"
#include "ofxCsv.h"
#include "ofxUI.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    //camera.disableMouseInput();
    
    // GUI things
    gui = new ofxUICanvas();
    
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    gui->loadSettings("settings.xml");
    
    vector<string> modeType;
    gui->addLabel("Mode Select", OFX_UI_FONT_MEDIUM);
    modeType.push_back("Freq vs. Dist (Tight)");
    modeType.push_back("Freq vs. Dist (Wide)");
    modeType.push_back("Taps vs. Frame");
    modeType.push_back("First Tap");
    modeType.push_back("First 20 Taps (Lines)");
    modeType.push_back("First 20 Taps (Combined)");
    modeType.push_back("Last Taps");
    modeType.push_back("Best Plyers: 1st 60 Taps");
    modeType.push_back("Best Plyers: Diff B/w Taps");
    modeType.push_back("Best Plyers: Diff B/w Taps (C)");
    ofxUIRadio *typeRadio = gui->addRadio("VR", modeType, OFX_UI_ORIENTATION_VERTICAL);
    typeRadio->activateToggle(modeTypeName);
    
    gui->autoSizeToFitWidgets();
    
    // ========================================================
    
    csv.loadFile(ofToDataPath("jumps.csv"), "	");
    
    int flapEntries = csv.numRows;
    int flapFields = csv.data[0].size();
    string lastAttemptId = "";
    string currAttemptId = "";
    vector<int> currSequence;
    int currFrame, lastFrame;
    int currFrameCount = 0;
    int freq = 0;
    int bestRunLen = 0;
    int bestRunId = 0;
    int bestRunId2 = 0;
    
    flapMesh.setMode(OF_PRIMITIVE_POINTS);
    
    for(int i = 0; i < 60; i++) {
        lstFrmCntsBest[i] = 0;
    }
    
	for (int i = 1; i < flapEntries; i++) {
        if (csv.data[i][0] != "")
        {
            currAttemptId = csv.getString(i, 0);
            
            currFrame = csv.getInt(i, 2);
            
            if (currAttemptId != lastAttemptId)
            {
                if (currFrameCount > 1)
                    freq = lastFrame/(currFrameCount-1);
                
                flap currentflap = {
                    currAttemptId,
                    currSequence,
                    freq,
                };
                
                if (bestRunLen < currSequence.size())
                {
                    bestRunId2 = bestRunId;
                    bestRunId = plyrCount;
                    bestRunLen = currSequence.size();
                }
                
                flaps.push_back(currentflap);
                plyrCount++;
                
                if ((currFrameCount > 0) && (currFrameCount < 20000))
                    lstFrmCnts[currFrameCount - 1]++;
                
                if ((currFrameCount > 0) && (currFrameCount < 20000) && (currFrameCount > bestLevelLimit))
                    lstFrmCntsBest[currFrameCount - 1]++;
                
                lastAttemptId = currAttemptId;
                currSequence.clear();
                currFrameCount = 0;
                freq = 0;
                firstFlap[currFrame]++;
            }
            else
            {
                if (currFrameCount < 60)
                    distSurvived[currFrameCount]++;
            }
            
            if (currFrameCount < 30)
                flapCounts[currFrameCount][currFrame]++;
            
            ofVec3f currentflapPoint(currFrameCount * 60, -.1 * currFrame + 850, 0);
            flapMesh.addVertex(currentflapPoint);
            
            currSequence.push_back(currFrame);
            currFrameCount++;
            lastFrame = currFrame;
        }
    }
    
    //=========================================
    
    for(int i = 0; i < 30; i++) {
        if (lstFrmCnts[i] > maxEnd) {
            maxEnd = lstFrmCnts[i];
        }
    }
    
    for (int j = 1; j < flaps.size(); j++) {
        if (flaps[j].sequence.size() > bestLevelLimit)
        {
            for(int i = 1; i < MIN(60,flaps[j].sequence.size()); i++) {
                flapCountsBest[i][flaps[j].sequence[i]]++;
                diffBetweenFlaps[i][flaps[j].sequence[i] - flaps[j].sequence[i-1]]++;
            }
            numTop++;
        }
    }
    
    for(int i = 1; i < 60; i++) {
        int maxForI = 0;
        int maxForIVal = 0;
        for (int j = 0; j < 2000; j++)
        {
            /*
            if (i == 1)
            {
                if (flapCountsBest[i][j] > maxForI)
                {
                    maxForI = flapCountsBest[i][j];
                    maxForIVal = j;
                }
            }
            else
            {
                if (diffBetweenFlaps[i][j] > maxForI)
                {
                    maxForI =diffBetweenFlaps[i][j];
                    maxForIVal = j;
                }
            }
             */
            maxForI += diffBetweenFlaps[i][j];
            maxForIVal += j * diffBetweenFlaps[i][j];
        }
        
        /*
        if (i == 1)
            bestMove[i] = maxForIVal;
        else
            bestMove[i] = bestMove[i-1]+ maxForIVal;
         */
        
        bestMove[i] = maxForIVal/maxForI;
        
      //  cout << ", " << bestMove[i] << endl;
        
    }
    
    for(int i = 0; i < 60; i++) {
        if (lstFrmCntsBest[i] > maxEndBest) {
            maxEndBest = lstFrmCntsBest[i];
        }
    }
    
    for(int i = 0; i < flaps[bestRunId2].sequence.size(); i++) {
        //cout << ", " << flaps[bestRunId2].sequence[i] << endl;
    }
    
    int avgDiff = 0;
    for(int i = 0; i < 60; i++) {
        avgDiff += bestMove[i];
    }
    
   // cout << "AVERAGE: " << avgDiff / 60  << " ;;; AVERAGE: " << avgDiff / 59 << endl;
}

//--------------------------------------------------------------
void testApp::update(){
    int zoom = 1;
    if (zoomRating)
        zoom = 15;
    
    flapMesh.clear();
    for(int i = 0; i != flaps.size(); i++) {
        flap p = flaps[i];
        
        if (modeTypeName == "Freq vs. Dist (Tight)")
        {
            if (p.sequence.size() > 1)
            {
                ofVec3f currentflapPoint(p.tapFreq + ofGetWidth()/2, -.23 * p.sequence[p.sequence.size()-1] + 870, 250);
                flapMesh.addVertex(currentflapPoint);
                ofColor c(0,0,255);
                flapMesh.addColor(c);
            }
        }
        if (modeTypeName == "Freq vs. Dist (Wide)")
        {
            if (p.sequence.size() > 1)
            {
                ofVec3f currentflapPoint(p.tapFreq * 10, -.23 * p.sequence[p.sequence.size()-1] + 870, 250);
                flapMesh.addVertex(currentflapPoint);
                ofColor c(0,0,255);
                flapMesh.addColor(c);
            }
        }
        if (modeTypeName == "Taps vs. Frame")
        {
            for (int j = 0; j < p.sequence.size(); j++)
            {
                ofVec3f currentflapPoint(j * 5 + 200, -.1 * p.sequence[j] + 850, zoom * 1);
                flapMesh.addVertex(currentflapPoint);
                ofColor c(0,0,255);
                flapMesh.addColor(c);
            }
        }
        
        /*
         for (int j = 0; j < p.sequence.size(); j++)
         {
         ofVec3f currentflapPoint(j * 5, -.1 * p.sequence[j] + 850, zoom * 1);
         
         // if (zoomPrice)
         //     currentflapPoint.set(h.longitude, h.latitude, (1/(zoom * zoom)) * h.price);
         
         //    if (zoomRating || zoomPrice)
         flapMesh.addVertex(currentflapPoint);
         
         ofColor c(0,0,255);
         
         //     if (h.stars != -1)
         //         c.set(255 - h.stars * 51, h.stars * 51, 0);
         
         flapMesh.addColor(c);
         }
         */
    }
    
    if (modeTypeName == "First Tap")
    {
        for (int j = 0; j < 70; j++)
        {
            ofVec3f currentflapPoint(j + ofGetWidth()/2, -.01 * firstFlap[j] + 950, zoom * 1);
            flapMesh.addVertex(currentflapPoint);
            ofColor c(255,255,255);
            flapMesh.addColor(c);
        }
    }
    
    for(int i = 0; i < 20; i++) {
        if (modeTypeName == "First 20 Taps (Lines)")
        {
            if (i < 20)
            {
                for (int j = 0; j < 20000; j++)
                {
                    ofVec3f currentflapPoint(j + 450, -.005 * flapCounts[i][j] + 65 + i * 50, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEnd - lstFrmCnts[i]) * 200)/maxEnd, ((maxEnd - lstFrmCnts[i]) * 150)/maxEnd, 0);
                    flapMesh.addColor(c);
        
                    lines[i].addVertex(currentflapPoint);
                    lines[i].close();
                }
            }
        }
    }
    
    if (modeTypeName == "First 20 Taps (Combined)")
    {
        for(int i = 0; i < 30; i++) {
            if (i < 30)
            {
                for (int j = 0; j < 20000; j++)
                {
                    ofVec3f currentflapPoint(j * 2 + 0, -.01 * flapCounts[i][j] + 565, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEnd - lstFrmCnts[i]) * 200)/maxEnd, ((maxEnd - lstFrmCnts[i]) * 150)/maxEnd, 0);
                    flapMesh.addColor(c);

                    paths[i].addVertex(currentflapPoint);
                    paths[i].close();
                }
            }
        }
    }
    
    if (modeTypeName == "Best Plyers: 1st 60 Taps")
    {
        for(int i = 0; i < 60; i++) {
            if (i < 20)
            {
                ofVec3f currentBestPoint(bestMove[i] * 2 + 0, -3 * flapCountsBest[i][bestMove[i]] + 365 + 1 * 200, zoom * 1);
                flapMesh.addVertex(currentBestPoint);
                ofColor c(0,0,255);
                flapMesh.addColor(c);
                linesBestFit[0].addVertex(currentBestPoint);
             //   linesBestFit[0].close();
                
                for (int j = 0; j < 20000; j++)
                {
                    ofVec3f currentflapPoint(j * 2 + 0, -3 * flapCountsBest[i][j] + 365 + 1 * 200, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF20[i].addVertex(currentflapPoint);
                    linesBestF20[i].close();
                }
            }
            else if (i < 40)
            {
                ofVec3f currentBestPoint(bestMove[i] * 2 - 1580, -3 * flapCountsBest[i][bestMove[i]] + 365 + 2 * 200, zoom * 1);
                flapMesh.addVertex(currentBestPoint);
                ofColor c(0,0,255);
                flapMesh.addColor(c);
                linesBestFit[1].addVertex(currentBestPoint);
                //linesBestFit[1].close();
                
                for (int j = 0; j < 20000; j++)
                {
                    ofVec3f currentflapPoint(j * 2 - 1580, -3 * flapCountsBest[i][j] + 365 + 2 * 200, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF20[i].addVertex(currentflapPoint);
                    linesBestF20[i].close();
                }
            }
            else if (i < 60)
            {
                ofVec3f currentBestPoint(bestMove[i] * 2 - 3160, -3 * flapCountsBest[i][bestMove[i]] + 365 + 3 * 200, zoom * 1);
                flapMesh.addVertex(currentBestPoint);
                ofColor c(0,0,255);
                flapMesh.addColor(c);
                linesBestFit[2].addVertex(currentBestPoint);
                //linesBestFit[2].close();
                
                for (int j = 0; j < 20000; j++)
                {
                    ofVec3f currentflapPoint(j * 2 -3160, -3 * flapCountsBest[i][j] + 365 + 3 * 200, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF20[i].addVertex(currentflapPoint);
                    linesBestF20[i].close();
                }
            }
        }
        
        linesBestFit[0].close();
        linesBestFit[1].close();
        linesBestFit[2].close();
    }

    if (modeTypeName == "Best Plyers: Diff B/w Taps")
    {
        for(int i = 0; i < 60; i++) {
            if (i < 20)
            {
                for (int j = 0; j < 2000; j++)
                {
                    ofVec3f currentflapPoint(j * 1 + i * 84 + 0, -1 * diffBetweenFlaps[i][j] + 365 + 1 * 200, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF60Diff[i].addVertex(currentflapPoint);
                    linesBestF60Diff[i].close();
                }
            }
            else if (i < 40)
            {
                for (int j = 0; j < 2000; j++)
                {
                    // ofVec3f currentflapPoint(j * 20, -1 * diffBetweenFlaps[i][j] + 365 + 2 * 200, zoom * 1); // <- This is wrong but makes cool looking things :P
                    ofVec3f currentflapPoint(j * 1 + (i%20) * 84 + 25, -1 * diffBetweenFlaps[i][j] + 365 + 2 * 200, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF60Diff[i].addVertex(currentflapPoint);
                    linesBestF60Diff[i].close();
                }
            }
            else if (i < 60)
            {
                for (int j = 0; j < 2000; j++)
                {
                    ofVec3f currentflapPoint(j * 1 + (i%20) * 84 + 25, -1 * diffBetweenFlaps[i][j] + 365 + 3 * 200, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF60Diff[i].addVertex(currentflapPoint);
                    linesBestF60Diff[i].close();
                }
            }
        }
    }
    
    
    if (modeTypeName == "Best Plyers: Diff B/w Taps (C)")
    {
        for(int i = 0; i < 60; i++) {
            if (i < 60)
            {
                for (int j = 0; j < 2000; j++)
                {
                    ofVec3f currentflapPoint(j * 10, -3.25 * diffBetweenFlaps[i][j] + 965, zoom * 1);
                    flapMesh.addVertex(currentflapPoint);
                    ofColor c(255,255,255);
                    c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                    flapMesh.addColor(c);
                    
                    linesBestF60DiffC[i].addVertex(currentflapPoint);
                    linesBestF60DiffC[i].close();
                }
            }
        }
    }
    
    if (modeTypeName == "Last Taps")
    {
        for (int j = 0; j < 1000; j++)
        {
            ofVec3f currentflapPoint(j + 450, -.001 * lstFrmCnts[j] + 950, zoom * 1);
            flapMesh.addVertex(currentflapPoint);
            ofColor c(255,255,255);
            flapMesh.addColor(c);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofColor centerColor = ofColor(38, 105, 117);
    ofColor edgeColor = ofColor(28, 27, 43);
    ofBackground(0x000000);
    
    camera.begin();
    camera.setScale(1, -1, 1);
    ofPushMatrix();
    ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
    if ((modeTypeName != "First 20 Taps (Lines)") && (modeTypeName != "First 20 Taps (Combined)") && (modeTypeName != "Best Plyers: 1st 60 Taps"))
        flapMesh.draw();
    ofPopMatrix();
    camera.end();
    
    if (modeTypeName == "First 20 Taps (Lines)")
    {
        for(int i = 0; i < 20; i++) {
            if (i < 20)
            {
                ofDrawBitmapStringHighlight("Flap: " + ofToString(i+1) + ". Last Flap For: " + ofToString(lstFrmCnts[i]), 200, 65 + i * 50);
                cout << "i: " << i << " ;;; " << lstFrmCnts[i] << endl;
            }
        }
    }
    
    if (modeTypeName == "First 20 Taps (Lines)")
    {
        for(int i = 0; i < 30; i++) {
            if (i < 30)
            {
                ofColor c(255,255,255);
                if (distSurvived[i] == 0)
                    c.set(255 - ((maxEnd - lstFrmCnts[i]) * 150)/maxEnd, ((maxEnd - lstFrmCnts[i]) * 150)/maxEnd, 0);
                else
                    c.set(255 - ((distSurvived[i] - lstFrmCnts[i]) * 200)/distSurvived[i], ((distSurvived[i] - lstFrmCnts[i]) * 150)/distSurvived[i], 0);
                flapMesh.addColor(c);
                ofSetColor(c);
                
                lines[i].draw();
            }
        }
    }
    
    if (modeTypeName == "First 20 Taps (Combined)")
    {
        for(int i = 0; i < 30; i++) {
            if (i < 30)
            {
                ofColor c(255,255,255);
                c.set(255 - ((maxEnd - lstFrmCnts[i]) * 200)/maxEnd, ((maxEnd - lstFrmCnts[i]) * 150)/maxEnd, 0);
                flapMesh.addColor(c);
                ofSetColor(c);
                
                paths[i].draw();
            }
        }
    }
    
    if (modeTypeName == "Best Plyers: 1st 60 Taps")
    {
        for(int i = 0; i < 60; i++) {
            if (i < 60)
            {
                ofColor c(255,255,255);
                c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                flapMesh.addColor(c);
                ofSetColor(c);
                
                linesBestF20[i].draw();
            }
            
            if (i == 0) // || (i == 20) || (i == 40))
                ofDrawBitmapStringHighlight(ofToString(i+1), 55 + int(i%20) * 84,  580 + int(i/20) * 200);
            else if (i < 20)
                ofDrawBitmapStringHighlight(ofToString(i+1), 25 + int(i%20) * 84,  580 + int(i/20) * 200);
            else
                ofDrawBitmapStringHighlight(ofToString(i+1), 55 + int(i%20) * 84,  580 + int(i/20) * 200);
            
            
            ofDrawBitmapStringHighlight("Num Top: " + ofToString(numTop), 200, 65 + 50);
        }
        
        ofColor cB(0,0,255);
        ofSetColor(cB);
        
       // for(int i = 0; i < 3; i++)
           // linesBestFit[i].draw();
    }
    
    if (modeTypeName == "Best Plyers: Diff B/w Taps")
    {
        for(int i = 0; i < 60; i++) {
            if (i < 60)
            {
                ofColor c(255,255,255);
                c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                flapMesh.addColor(c);
                ofSetColor(c);
                
                linesBestF60Diff[i].draw();
            }
            
            if (i == 0) // || (i == 20) || (i == 40))
                ofDrawBitmapStringHighlight(ofToString(i+1), 55 + int(i%20) * 84,  580 + int(i/20) * 200);
            else if (i < 20)
                ofDrawBitmapStringHighlight(ofToString(i+1), 25 + int(i%20) * 84,  580 + int(i/20) * 200);
            else
                ofDrawBitmapStringHighlight(ofToString(i+1), 55 + int(i%20) * 84,  580 + int(i/20) * 200);
            
            
            ofDrawBitmapStringHighlight("Num Top: " + ofToString(numTop), 200, 65 + 50);
        }
    }
    
    if (modeTypeName == "Best Plyers: Diff B/w Taps (C)")
    {
        for(int i = 0; i < 60; i++) {
            if (i < 60)
            {
                ofColor c(255,255,255);
                c.set(255 - ((maxEndBest - lstFrmCntsBest[i]) * 200)/maxEndBest, ((maxEndBest - lstFrmCntsBest[i]) * 150)/maxEndBest, 0);
                flapMesh.addColor(c);
                ofSetColor(c);
                
                linesBestF60DiffC[i].draw();
            }
            
            ofDrawBitmapStringHighlight("Num Top: " + ofToString(numTop), 200, 65 + 50);
        }
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == OF_KEY_RIGHT) {
        camera.move(5, 0, 0);
    }
    else if (key == OF_KEY_LEFT) {
        camera.move(-5, 0, 0);
    }
    else if (key == OF_KEY_UP) {
        camera.move(0, 5, 0);
    }
    else if (key == OF_KEY_DOWN) {
        camera.move(0, -5, 0);
    }
    else if (key == 'x') {
        camera.move(0, 0, -5);
    }
    else if (key == 'z') {
        camera.move(0, 0, 5);
    }
    else if (key == 'h') {
        gui->toggleVisible();
    }
    else if (key == 'c') {
        camera.reset();
    }
    else {
        //zoomRating = true;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	zoomRating = false;
    zoomPrice = false;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	
}

//--------------------------------------------------------------
void testApp::exit()
{
    gui->saveSettings("settings.xml");
    delete gui;
    
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    int on = 1;
    
    if (kind == OFX_UI_WIDGET_TOGGLE) {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cout << name << "\t value: " << toggle->getValue() << endl;
        //hotelTypeLayer = true;
        modeTypeName = name;
    }
}

