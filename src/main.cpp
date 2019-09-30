#include "ofMain.h"
#include "testApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    
    ofAppGLFWWindow window;
//    ofGLWindowSettings settings;
//    settings.glVersionMajor = 3;
//    settings.glVersionMinor = 3;



//    settings.windowMode = OF_WINDOW;
//    ofCreateWindow(settings);


//    ofInit();
//    std::shared_ptr<ofAppGLFWWindow> windowPtr = std::shared_ptr<ofAppGLFWWindow>(&window, std::function<void(ofAppGLFWWindow *)>(noopDeleter<ofAppGLFWWindow>));
//    ofGLWindowSettings settings = window.getSettings();
//    settings.setSize(scrw, scrh);
//    settings.glVersionMajor = 3;
//    settings.glVersionMinor = 3;
//    settings.windowMode = OF_WINDOW;
//    ofGetMainLoop()->addWindow(windowPtr);
//    window.setup(settings);


    ofSetupOpenGL(&window, scrw,scrh, OF_WINDOW);            // <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());
    
}
