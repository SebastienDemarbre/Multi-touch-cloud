#include "testApp.h"
//--------------------------------------------------------------

void testApp::setup() {



    ajout_bouton("bouton1", 100, 100, 200, 200, 147, 0, 255, 255, 0, 255, "button-gris.png", "button-gris-over.png", true);
    ajout_bouton("bouton2", 450, 100, 50, 100, 160, 82, 45, 128, 0, 0, "button-gris.png", "button-gris-over.png");
    ajout_bouton("bouton3", 650, 100, 200, 200, 47, 79, 79, 0, 0, 0, "button-gris.png", "button-gris-over.png");
    ajout_bouton("bouton4", 100, 450, 750, 200, 255, 69, 0, 220, 20, 60, "button-gris.png", "button-gris-over.png");

    //ofSetLogLevel(OF_LOG_VERBOSE);

    for (int i = 0; i < 640 * 480; i++) {
        points.push_back(new ofVec3f());
    }

    kinect.setRegistration(true);

    kinect.init();

    kinect.open(); // opens first available kinect

    bThreshWithOpenCV = false;
    ofSetFrameRate(200);

    grayImage.allocate(kinect.width, kinect.height);

    //	 start from the front
    bDrawPointCloud = true;

    filterP1.x = -250;
    filterP1.y = -155;
    filterP1.z = 1055;
    filterP2.x = 250;
    filterP2.y = 155;
    filterP2.z = 1085;

    normal();


    verdana.loadFont("verdana.ttf", 14, true, true);
    verdana.setLineHeight(50.0f);
    verdana.setLetterSpacing(1.037);


    kinect.setCameraTiltAngle(3);
}

//--------------------------------------------------------------

void testApp::normal() { // normalisation filtre
    float pixhmax = 0, pixlmax = 0;

    pixhmax = tan((21.5 / 180) * PI) * filterP2.z; //0.39= tan 43/2 opposé/profondeur
    pixlmax = tan((28.5 / 180) * PI) * filterP2.z; //0.54= tan 57/2 opposé/profondeur

    pixh = (240.f / pixhmax) * filterP2.y;
    pixl = (320.f / pixlmax) * filterP2.x;

    pixh = pixh * 2;
    pixl = pixl * 2;




}

void testApp::update() {

    int w = 640;
    int h = 480;
    int i = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            ofVec3f * v = points[ i ];
            if (kinect.getDistanceAt(x, y) > 0) {
                v->set(kinect.getWorldCoordinateAt(x, y));
                v->x *= -1;
                v->y *= -1;
            } else {
                v->set(0, 0, 0);
            }
            i++;
        }

    }


    unsigned char * pix = grayImage.getPixels();
    int numPixels = grayImage.getWidth() * grayImage.getHeight();
    for (int i = 0; i < numPixels; i++) {
        ofVec3f b = *(points[ i ]);
        if (compare_grand(b, filterP1) && compare_petit(b, filterP2)) {
            pix[i] = 190 + ((b.z - filterP1.z) / (filterP2.z - filterP1.z))*65;
        } else {
            pix[i] = 0;
        }
    }
    int temp = 189, ID = 0;
    for (int i = 0; i < numPixels; i++) {
        if (pix[i] > temp) {
            ID = i;
            temp = pix[i];
        }
    }
    grayImage.flagImageChanged();

    contourFinder.findContours(grayImage, 10, 1000, 20, false);

    kinect.update();

    // reset_boutons();

    //int height = ofGetWindowHeight();
    //int width = ofGetWindowWidth();

    blobs.clear();
    for (int i = 0; i < contourFinder.blobs.size(); i++) {
        ofxCvBlob b = contourFinder.blobs[i];
        //b.draw(0,0);
        //ofCircle(b.centroid.x, b.centroid.y, 10);
        blobs.push_back(ofVec2f(
                (b.centroid.x - ((640 - pixl) / 2)) / pixl,
                (b.centroid.y - ((480 - pixh) / 2)) / pixh
                ));
        cout << "centroid x" << b.centroid.x << endl;
        cout << "points" << points[ID]->x << endl;
        hitTest(blobs[i].x * ofGetWidth(), blobs[i].y * ofGetHeight());
        //ofCircle(blobs[i].x,blobs[i].y, 50);
    }

    if (blobs.size() == 0) {
        btracker.actif = false;
    } else if (!btracker.actif) {
        btracker.vNorm = blobs[0];
        btracker.actif = true;
        btracker.firstTime = true;
    } else {
        float d = 0.05;
        int bId = -1;
        for (int i = 0; i < blobs.size(); i++) {
            float tmp = ofDist(btracker.vNorm.x, btracker.vNorm.y, blobs[i].x, blobs[i].y);
            if (tmp < d) {
                d = tmp;
                bId = i;
            }

        }
        if (bId != -1) {
            btracker.vNorm = blobs[bId];

        } else {
            btracker.actif = false;
        }
    }
    renderblobtracker(btracker);
    hitTest(mouseX, mouseY);

    for (int i = 0; i < boutons.size(); i++) {
        bouton* tempb = boutons[i];
        if (!tempb->visible)
            continue;
        if (tempb->active && tempb->delay > 0) {
            tempb->delay--;
            cout << tempb->delay << endl;
            break;
        }
        if (tempb->active && tempb->delay == 0) {
            tempb->active = false;
            cout << "Coucou" << endl;
            if (tempb->nom == "bouton1") {
                deactivate_all();
                activate_bouton("bouton2");
            } else if (tempb->nom == "bouton2") {
                deactivate_all();
                activate_bouton("bouton3");
            } else if (tempb->nom == "bouton3") {
                deactivate_all();
                activate_bouton("bouton4");
            } else if (tempb->nom == "bouton4") {
                deactivate_all();
                activate_bouton("bouton1");
            }
        }
    }
}

void testApp::deactivate_all() {
    for (int i = 0; i < boutons.size(); i++) {
        bouton* tempb = boutons[i];

        tempb->active = false;
        tempb->visible = false;

    }
};

void testApp::activate_bouton(string nom) {
    for (int i = 0; i < boutons.size(); i++) {
        bouton* tempb = boutons[i];
        if (tempb->nom == nom) {
            tempb->active = false;
            tempb->visible = true;
            return;
        }
    }
}

//--------------------------------------------------------------

void testApp::renderblobtracker(blobtracker &bt) { //tracker 
    if (!bt.firstTime) {
        bt.vEcran.x += ((bt.vNorm.x * ofGetWidth()) - bt.vEcran.x)*0.3;
        bt.vEcran.y += ((bt.vNorm.y * ofGetHeight()) - bt.vEcran.y)*0.3;
    } else {
        bt.vEcran.x = bt.vNorm.x * ofGetWidth();
        bt.vEcran.y = bt.vNorm.y * ofGetHeight();
        bt.firstTime = false;
    }

}

bool testApp::compare_petit(ofVec3f t, ofVec3f v) {
    if (t.x <= v.x && t.y <= v.y && t.z <= v.z) {
        return true;
    } else {
        return false;
    }
}

bool testApp::compare_grand(ofVec3f t, ofVec3f v) {
    if (t.x >= v.x && t.y >= v.y && t.z >= v.z) {
        return true;
    } else {
        return false;
    }
}

void testApp::draw() {

    ofBackground(173, 216, 230);

    easyCam.begin();
    // stringstream reportStream;

    ofSetColor(255, 255, 255);
    glBegin(GL_POINTS);
    for (int i = 0; i < 640 * 480; i++) {
        ofVec3f v = *(points[ i ]);
        /* if (compare_grand(v, filterP1) && compare_petit(v, filterP2)) {
             glVertex3f(v.x, v.y, v.z);
         }*/
    }
    glEnd();
    easyCam.end();
    ofSetColor(255, 255, 255);
    //grayImage.draw(0, 0, 640, 480);
    ofSetColor(255, 0, 0);
    ofFill();
    ofRect((640 - pixl) / 2, (480 - pixh) / 2, 5, 5);
    ofRect(((640 - pixl) / 2) + pixl, ((480 - pixh) / 2) + pixh, 5, 5);

    //contourFinder.draw(10,10,400,300);

    for (int i = 0; i < boutons.size(); i++) {
        bouton* tempb = boutons[i];
        if (!tempb->visible)
            continue;
        if (tempb->active) {
            ofSetColor(tempb->c4, tempb->c5, tempb->c6, 255);
            tempb->imgOver.draw(tempb->x, tempb->y, tempb->largeur, tempb->hauteur);
        } else {
            ofSetColor(tempb->c1, tempb->c2, tempb->c3, 255);
            tempb->imgDefault.draw(tempb->x, tempb->y, tempb->largeur, tempb->hauteur);
        }
        //ofRect(tempb->x, tempb->y, tempb->largeur, tempb->hauteur);

    }

    ofSetColor(255, 0, 255);


    for (int i = 0; i < contourFinder.blobs.size(); i++) {
        ofxCvBlob b = contourFinder.blobs[i];
        for (int j = 0; j < b.nPts; j++) {
            // ofRect(b.pts[j].x,b.pts[j].y,2,2);
        }
    }
    ofSetColor(255, 255, 255);

    for (int i = 0; i < blobs.size(); i++) {
        ofCircle(blobs[i].x * ofGetWidth(), blobs[i].y * ofGetHeight(), 5);
        ofDrawBitmapString(ofToString(blobs[i].x) + "/" + ofToString(blobs[i].y), 210, 25 + i * 20);
    }

    if (btracker.actif) {
        ofSetColor(0, 255, 0);
    } else {
        ofSetColor(255, 0, 0);
    }
    ofNoFill();
    ofCircle(btracker.vEcran.x, btracker.vEcran.y, 10);



    //    ofCircle(abs + width / 4, ord + height / 4, 15);


    ofDrawBitmapString("p1 " +
            ofToString(filterP1.x) + "," +
            ofToString(filterP1.y) + "," +
            ofToString(filterP1.z), 50, 50
            );

    ofDrawBitmapString("p2 " +
            ofToString(filterP2.x) + "," +
            ofToString(filterP2.y) + "," +
            ofToString(filterP2.z), 50, 75
            );
    ofDrawBitmapString("pix " +
            ofToString(pixl) + "," +
            ofToString(pixh), 50, 100
            );


}

void testApp::ajout_bouton(string nom, float x, float y, float largeur, float hauteur, int c1, int c2, int c3, int c4, int c5, int c6, string def, string over, bool visible) {
    bouton* tempb = new bouton();
    tempb->nom = nom;
    tempb->x = x;
    tempb->y = y;
    tempb->largeur = largeur;
    tempb->hauteur = hauteur;
    tempb->active = false;
    tempb->c1 = c1;
    tempb->c2 = c2;
    tempb->c3 = c3;
    tempb->c4 = c4;
    tempb->c5 = c5;
    tempb->c6 = c6;
    tempb->imgDefault.loadImage(def);
    tempb->imgOver.loadImage(over);
    tempb->visible = visible;
    tempb->delay = -1;
    boutons.push_back(tempb);
}

void testApp::hitTest(float x, float y) {
    for (int i = 0; i < boutons.size(); i++) {
        bouton* tempb = boutons[i];
        if (!tempb->visible)
            continue;
        if (x >= tempb->x && y >= tempb->y && x <= tempb->largeur + tempb->x && y <= tempb->hauteur + tempb->y) {
            if (!tempb->active) {
                tempb->active = true;
                tempb->delay = 100;
                cout << "blabla " << tempb->nom << endl;
            }
            //return;
        } else if (tempb->active) {
            tempb->active = false;
        }
    }
}

void testApp::reset_boutons() {
    for (int i = 0; i < boutons.size(); i++) {
        boutons[i]->active = false;
    }
}

void testApp::keyPressed(int key) {
    switch (key) {
        case OF_KEY_UP:
            filterP1.z += 5;
            filterP2.z += 5;
            cout << filterP1.z << endl;
            break;
        case OF_KEY_DOWN:
            filterP1.z -= 5;
            filterP2.z -= 5;
            cout << filterP2.z << endl;
            break;
        default:
            cout << key << endl;
            break;
    }

}

//--------------------------------------------------------------

void testApp::exit() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}
