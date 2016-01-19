#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include <vector>

using namespace std;

struct bouton {
    float largeur,hauteur,x,y;
    bool active,visible;
    int c1,c2,c3,c4,c5,c6;
    ofImage imgDefault ;
    ofImage imgOver;
    string nom;
    int delay;
};

struct blobtracker{
    ofVec2f vNorm,vEcran;
    bool actif=false;
    bool firstTime=false;
    
};

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
        void keyPressed(int key);
	bool compare_grand(ofVec3f t,ofVec3f v);
        bool compare_petit(ofVec3f t,ofVec3f v);
        void ajout_bouton(string nom,float x,float y,float largeur,float hauteur,int c1,int c2,int c3,int c4,int c5,int c6,string,string,bool visible=false);
        void hitTest(float x,float y);
        void reset_boutons();
        void normal();
        void renderblobtracker(blobtracker &bt);
        void deactivate_all();
        void activate_bouton(string);
        
        ofxKinect kinect;
        vector< ofVec3f *> points;
        ofVec3f filterP1,filterP2;
	ofVec2f pixel;
        
        ofxCvGrayscaleImage grayImage;
        
        ofxCvContourFinder contourFinder;
        
	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
        
        ofSoundPlayer  son;
        ofTrueTypeFont	verdana;
        
        vector<ofVec2f> blobs; 
        
        vector<bouton*> boutons;
        
        int pixh ;
        int pixl ;
        
        blobtracker btracker;
        
};
