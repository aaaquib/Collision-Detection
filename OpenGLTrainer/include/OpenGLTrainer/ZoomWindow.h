/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief ZoomWindow.h defines the ZoomWindow class.

TO DO LIST:
\todo considered removing deprecated conditionally compiled code

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] Donald Hearn and M. Pauline Baker.  Computer Graphics with OpenGL: Third Edition.

\internal
*/
#ifndef ZOOM_WINDOW_H
#define ZOOM_WINDOW_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <OpenGLTrainer/OpenGLTrainer.h>

namespace ITCS4120
{
namespace OpenGLTrainer
{


/**
\brief ZW_DRAW_TEXTURE_QUAD is a ZW_DRAW_MODE option, that uses
OGL texture quad to draw the capture image into the ZoomWindow.
*/
#define ZW_DRAW_TEXTURE_QUAD 0

/**
\brief ZW_DRAW_DRAWPIXELS is a ZW_DRAW_MODE option, that uses
OGL DrawPixels command to draw the captured image into the ZoomWindow.
*/
#define ZW_DRAW_DRAWPIXELS   1

/**
\brief ZW_DRAW_MODE determines how captured pixels are drawn. Set
to either ZW_DRAW_TEXTURE_QUAD or ZW_DRAW_DRAWPIXELS.

Circa 2011, ZW_DRAW_TEXTURE_QUAD is generally faster.
*/
#define ZW_DRAW_MODE ZW_DRAW_TEXTURE_QUAD 


#if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD 
/**
\def ZW_USE_COPY_TEXTURE
\brief ZW_USE_COPY_TEXTURE is a compilation switch that enables using a shared texture object 
that transfers framebuffer image using only the GPU (instead of using glReadPixels and CPU RAM) to transfer 
the captured framebuffer.

Note, ZW_USE_COPY_TEXTURE mode does NOT work under Windows Vista and beyond if the ZoomWindow is 
configured to capture the entire screen/desktop instead of capturing a specific GLUT Window's
contents. This is due to changes in Window's Vista to using a 'compositing window manager'.
As a result, the current ZW_USE_COPY_TEXTURE implementation's trick do not work for capturing the 
entire framebuffer under Vista.
*/
//#define ZW_USE_COPY_TEXTURE
#endif


/**
\page ZoomWindow_Page ZoomWindow

ZoomWindow is GLUT window that shows a zoomed in view of another
GLUT window.  

\section ZoomWindow_API API

The simplest way to integrate ZoomWindow into another application:

1) Call ZoomWindow::create() in 'main' to create the ZoomWindow.

2) Call ZoomWindow::captureFramebuffer() from the target GLUT window's display callback 
after the display function completes its OGL drawing--but before any glutSwapBuffer if 
the target window is double-buffered .

The above API only supports one ZoomWindow that captures from a single other GLUT window.
(A singleton ZoomWindow is internally created).

\todo create alternative API allowing creation of multiple ZoomWindow instances.

\section ZoomWindow_USER_INTERFACE USER INTERFACE

- Left-click dragging the mouse pans the contents of the zoom window.  
- Right-click dragging up/down zooms in and out using the original mouse position as the center 
of room.
- Middle-click brings up pop-up menu.  The 'Capture' item enables/disables active capturing of 
the target window/screen.

*/

/**
\class ZoomWindow
\brief ZoomWindow is GLUT window that shows a zoomed in view of another
GLUT window.  See \ref ZoomWindow_Page.
*/
class OPENGLTRAINER_CLASS ZoomWindow : public Window
    {
    public:
    ZoomWindow();
    ZoomWindow(int fullScreenWidth, int fullScreenHeight);
    ~ZoomWindow();


    private:
    enum {
	/* default window width */
	WIDTH=500,
	/* default window height */
	HEIGHT=500,
	/* byte alignment for OGL pixel transfer */
	ALIGNMENT=1};

   #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD 
    /*
    \brief ViewWindow represents a 2D rectangular view window that is
    axis-aligned in world space.
    */
    struct ViewWindow
	{
	/* lower left corner of view window in world coordinates */
	float lowerLeft [2];
	/* upper right corner of view window in world coordinates */
	float upperRight [2];
	} viewWindow;

    private:
    /**
    \brief 'CaptureDesktop' window is a fullscreen GLUT window.  Nothing is drawn to it, 
    however, we do a screen capture of the framebuffer contents in that fullscreen 
    window.  This data is then copied to the separate zoom window.  Generally, the 
    contents of the framebuffer will be whatever other pixels were drawn by other applications 
    into the framebuffer (at least this is what happens in most OS, but Vista may not
    work this way...)

    \warning This fails under Windows Vista which uses a compositing window manager. 
    Hence use of this class is currently circumvented under Windows. This is unfortunate
    because this trick allowed the fastest update of the ZoomWindow by performing
    all copies directly on the GPU.
    */
    class OPENGLTRAINER_CLASS CaptureDesktop : public ITCS4120::OpenGLTrainer::Window
	{
	public:
	CaptureDesktop ();
	void glutCreateWindow(const char* name);
	virtual void display(void);
	virtual void reshape (int w, int h);
	};
    static CaptureDesktop _gCaptureDesktop;

    /* center of mouse zoom (in window coordinates) */
    int zoomCenter [2];
   #else 
    /** 'zoomLevel' is the zoom level shown in this zoom window */
    GLfloat zoomLevel;

    /** 'lowerLeft' raster position offset of the lower left location of the copied pixel array */
    GLint lowerLeft[2];
   #endif

    /** 'last' last mouse position recorded previous call to motion callback */
    GLint last[2];

    /** 'drag' indicates if UI is in drag mode */
    bool drag;
    /** 'zoom' indiciates if UI is in zoom mode */
    bool zoom;

    /** 'capture' indicates if ZoomWindow is actively capturing new data from the 
        target window
     */
    bool capture;

    /** GLUT window ID of target window that this ZoomWindow is capturing */
    int targetWindowID;

   #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD 
    /** textureID of OGL texture that stores copy of the target window's framebuffer */
    struct Texture
	{	
	/* width of texture */
	int width;
	/* height of texture */
	int height;
	/* OGL texture object ID */
	GLuint ID;
	} texture;
   #endif

    /** copy of target window frame buffer */
    struct Framebuffer
	{              
	/* copy of target window's captured framebuffer

	Note, this is only used under certain compile-time and run-time conditions.
	*/
	GLubyte* image;
       
	/* width of captured framebuffer */
	int width;
	/* height of captured framebuffer */
	int height;
	/* format of data (GL_RGB, etc.) */
	GLint format;
	}framebuffer;

    /* 'initialized' indicates if ZoomWindow has set it initial desire OGL 
       settings */
    bool initialized;

   #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD && defined(ZW_USE_COPY_TEXTURE) 
   #ifdef _WIN32
    /* 'sharedLists' indicates that the 2 OGL contexts of this ZoomWindow and the
       target window are setup to shared texture objects */
    bool sharedLists;
    /* OGL context of this ZoomWindow */
    HGLRC glContext;
   #else
   #error This feature is only supported under Windows
   #endif
   #endif

   #ifdef _WIN32
    void captureScreen();
    /* indicates ZoomWindow should capture screen directly from the OS instead of
       using captureFramebuffer() method */       
    bool captureScreenFromOS;
   #endif

    void captureFramebuffer_();
    void initializeOpenGL();


    /** 
     **  glut display callback member functions (details in .cpp file)
     **/
    virtual void display(void);
    virtual void motion (int x, int y);
    virtual void passiveMotion (int x, int y);
    virtual void mouse (int button, int state, int x, int y);
    virtual void reshape (int w, int h);
    virtual void keyboard(unsigned char key, int x, int y);

    /** singleton instance of ZoomWindow */
    static ZoomWindow zoomWindow;    

    /**
     **	 static member functions
     **/
    private:
    void zoomView(float scale, float windowToWorld [2]);
    void setDisplayResolution(int width, int height);    
    void saveDisplayResolution();
    /*
    original display resolution height and width of display, when ZoomWindow is
    configured as full screen window.
    */
    int originalDisplayHeight;
    int originalDisplayWidth;
    bool fullScreen;

    static void menuCallback(int value);
    public:
    static void captureFramebuffer();
    static void create(int windowID = -1);
    static void create(int fullScreenWidth, int fullScreenHeight);

    static const float MIN_ZOOM;
    };
#endif


};
};