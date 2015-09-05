/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief ZoomWindow.cpp implements the ZoomWindow class.

TO DO LIST:
\todo

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] Donald Hearn and M. Pauline Baker.  Computer Graphics with OpenGL: Third Edition.
- [R2] http://www.geocities.com/krishnapg/screencap.html
\internal
*/
#ifdef _WIN32
#include <windows.h>
#include <Tchar.h>
#endif

#include <OpenGLTrainer/ZoomWindow.h>

#include <assert.h>
#include <iostream>



using namespace ITCS4120::OpenGLTrainer;

/*******************************************************************************
    File Scope Data Types
*******************************************************************************/
// NONE
/*******************************************************************************
    File Scope Functions
*******************************************************************************/
static unsigned int nextPowerOfTwo(unsigned int i);
/*******************************************************************************
    File Scope (static/private) globals
*******************************************************************************/
const float ZoomWindow::MIN_ZOOM = 0.5f;

/*******************************************************************************
    File Scope Macros 
*******************************************************************************/
/** for the zoom window, this determines if double buffer support is compiled
or not */
#define USE_DOUBLE_BUFFER

/*******************************************************************************
    Exported (extern) Globals
*******************************************************************************/
ZoomWindow ZoomWindow::zoomWindow;
ZoomWindow::CaptureDesktop ZoomWindow::_gCaptureDesktop;
/*******************************************************************************
    Exported (extern) Functions
*******************************************************************************/



/**
\brief [EXPERIMENTAL] Construct this ZoomWindow and display as fullscreen as described in
ZoomWindow::create(int fullScreenWidth, int fullScreenHeight)
Note, this mode is only useful in multi-monitor workstations.  
*/
ZoomWindow::ZoomWindow(int fullScreenWidth, int fullScreenHeight)
    {
    /* window initial size */
    fullScreen = true;
    width = fullScreenWidth; 
    height = fullScreenHeight; 
    }

/**
\brief Construct this ZoomWindow
*/
ZoomWindow::ZoomWindow()
    {
    fullScreen = false;
    width = height = 500;

   #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD 
    /* initialize viewWindow to match window coordinates */
    viewWindow.lowerLeft[0] = viewWindow.lowerLeft[1] = 0;
    viewWindow.upperRight[0] = width;
    viewWindow.upperRight[1] = height;
   #elif ZW_DRAW_MODE == ZW_DRAW_DRAWPIXELS
    zoomLevel = 1.0;
    lowerLeft [0] = lowerLeft [1] = 0;
   #endif

    capture = true;

    last [0] = last[1] = -1;
    drag = zoom = false;
    framebuffer.width = framebuffer.height = 0;
    framebuffer.format = GL_RGB;

   #if ZW_DRAW_MODE == ZW_DRAW_DRAWPIXELS || (ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD && !defined(ZW_USE_COPY_TEXTURE))
    framebuffer.image = NULL;
   #endif

   #ifdef _WIN32
    this->captureScreenFromOS = false;
   #endif
    texture.ID = 0;
    texture.width = texture.height = 0;
    initialized = false;

   #if ZW_DRAW_MODE==ZW_DRAW_TEXTURE_QUAD && defined(ZW_USE_COPY_TEXTURE)
   #ifdef _WIN32
    sharedLists = false;
    glContext = NULL;
   #else
    #error This feature is only supported under Windows
   #endif   
   #endif
    }

ZoomWindow::~ZoomWindow()
    {
    if (fullScreen)
	setDisplayResolution(originalDisplayWidth,originalDisplayHeight);
    }

/**
\brief [STATIC] Create GLUT window, the zoom window, that captures the 
right-half of the framebuffer assuming the desktop/framebuffer spans two displays
(i.e. two monitors).

The zoom window itself is set to size (fullScreenWidth, fullScreenHeight), placed
in the first display and the first display's resolution is set to the given size.

Note, this mode is only useful in multi-monitor workstations.  The 
ZoomWindow is created on the first display.

A singleton ZoomWindow instance is initialized and displayed (see \ref ZoomWindow_API).

\todo Do something more robust than capturing the right-half of the framebuffer.
The current implementation is Windows specific, based on how certain windows screen
capture functions capture the whole desktop's framebuffer.
*/
void ZoomWindow::create(int fullScreenWidth, int fullScreenHeight)
    {
    zoomWindow.fullScreen = true;
    zoomWindow.width = fullScreenWidth;
    zoomWindow.height = fullScreenHeight;
    create();
    }
/**
\brief [STATIC] Create GLUT window, the zoom window, that captures the framebuffer from 
another GLUT window 'targetWindowID0'.  If targetWindowID0 == -1, then the 'current'
GLUT window (see glutGetWindow()) is assumed to be the target.  If no glut window 
has currently active, then the entire desktop framebuffer is captured.

A singleton ZoomWindow instance is initialized and displayed (see \ref ZoomWindow_API).
*/
void ZoomWindow::create(int targetWindowID0)
    {
    
    if (zoomWindow.fullScreen)
	{
	zoomWindow.saveDisplayResolution();
	zoomWindow.setDisplayResolution(zoomWindow.width,zoomWindow.height);
	}

    if (targetWindowID0 == -1)
	{
	zoomWindow.targetWindowID = glutGetWindow();
	if (zoomWindow.targetWindowID == 0)
	    {/* no GLUT Window was created or specified, so capture the entire framebuffer */
	   #if (ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD && defined(ZW_USE_COPY_TEXTURE))           
	    _gCaptureDesktop.glutCreateWindow("Fullscreen Capture, Prof. Zachary Wartell, UNCC");
           #else
	    zoomWindow.captureScreenFromOS = true;
           #endif
	    zoomWindow.targetWindowID = -1;
	    }
	}
    else
	zoomWindow.targetWindowID = targetWindowID0;

    glutInitWindowSize(zoomWindow.width,zoomWindow.height);   

   #ifdef USE_DOUBLE_BUFFER
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
   #else
    glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
   #endif
    zoomWindow.glutCreateWindow("Zoom Window - Prof. Zachary Wartell, UNCC");    

    if(zoomWindow.fullScreen)
	glutFullScreen();
   
    glutCreateMenu(OpenGLTrainer::ZoomWindow::menuCallback);
    glutAddMenuEntry(zoomWindow.capture?"Capture (X)":"Capture ( )",0);
    glutAttachMenu(GLUT_MIDDLE_BUTTON);
    }

/**
\brief [STATIC] capture framebuffer of current GLUT window to the default singleton 
ZoomWindow (see \ref ZoomWindow_API).
*/
void ZoomWindow::captureFramebuffer() 
    { 
    zoomWindow.captureFramebuffer_();
    }


/*******************************************************************************
    PRIVATE FUNCTIONS (static functions/private member functions)
*******************************************************************************/

/**
\brief [STATIC] handle selection of ExperimentControl menu selection
*/
void ZoomWindow::menuCallback(int value)
    {
    switch(value)
	{
	case 0:
	    zoomWindow.capture = !zoomWindow.capture ;
	    glutChangeToMenuEntry(1,zoomWindow.capture?"Capture (X)":"Capture ( )",0);
	    if (zoomWindow.targetWindowID>=0)
		{
		glutSetWindow(zoomWindow.targetWindowID);
		glutPostRedisplay();
		}
	    glutSetWindow(zoomWindow.windowID());
	    glutPostRedisplay();
	    break;
	}
    }

/**
\brief call in OGL context of target window

FOOTNOTES:
- [F1] Circa 11/06/07 Many OGL cards still have a performance drop when using
  none power-of-two textures, so bump up the size as needed.
*/
void ZoomWindow::captureFramebuffer_()
    {
    if (!capture)
	return;

   #if ZW_DRAW_MODE == ZW_DRAW_DRAWPIXELS || (ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD && !defined(ZW_USE_COPY_TEXTURE))
    /**
	copy OGL framebuffer pixels into 'framebuffer' object
     **/
    glPixelStorei(GL_PACK_ALIGNMENT,ALIGNMENT);
    int 
	w = glutGet(GLUT_WINDOW_WIDTH),
	h = glutGet(GLUT_WINDOW_HEIGHT);
    if (framebuffer.width != w || framebuffer.height != h)
	{
	delete[] framebuffer.image;
	framebuffer.image = new GLubyte [w*h*3];
	framebuffer.width = w;
	framebuffer.height = h;
	}
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,framebuffer.image);
   #elif ZW_DRAW_MODE==ZW_DRAW_TEXTURE_QUAD && defined(ZW_USE_COPY_TEXTURE)
    /**
	OGL framebuffer pixels OGL texture object shared with ZoomWindow's
	OGL context
     **/

    /* establish texture object shared */
    if (!sharedLists)
	{
	if (!glContext)
	    /* glContext isn't setup yet, so just return (captureFramebuffer_ may be 
	       called a few times before its setup so just wait until glContext is ready)
	     */
	    return;
       #ifdef _WIN32	
	assert_always(wglShareLists(glContext,wglGetCurrentContext()));
       #else
       #error This feature can only be compiled under Windows
       #endif
	sharedLists = true;
	}

    /* create texture object (shared between the two OGL contexts) */
    int 
	w = glutGet(GLUT_WINDOW_WIDTH),
	h = glutGet(GLUT_WINDOW_HEIGHT);
    if (texture.ID==0 || framebuffer.width != w || framebuffer.height != h)
	{/* texture isn't yet created or size needs to be changed, so
	    create new texture object */

	framebuffer.width = w;
	framebuffer.height = h;
	texture.width = nextPowerOfTwo(w);  // see [F1]
	texture.height = nextPowerOfTwo(h);

	/* delete old texture object if it exists */
	if (texture.ID)
	    glDeleteTextures(1,&texture.ID);
	
	/* generate texture object */
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture.ID);
	glBindTexture(GL_TEXTURE_2D, texture.ID);

	/* set texture parameters */
	glPixelStorei(GL_UNPACK_ALIGNMENT,ALIGNMENT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
	    GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
	    GL_NEAREST);
	
	/* copy framebuffer directly into texture image */
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,texture.width,texture.height,0);
	assert(glGetError() == GL_NO_ERROR);
	}
    else
	{/* texture created and ready, so copy */
	/* copy framebuffer directly into texture image */
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,texture.width,texture.height);
	}
   #endif

    glutPostRedisplay();
    }

#ifdef _WIN32
/* hard coded for now for some of my tests */
static const LPCTSTR DISPLAY = _T("\\\\.\\Display0");

/**
\brief Save current display resolution
*/
void ZoomWindow::saveDisplayResolution()
    {
    DEVMODE dm_start;
    /* get current display size */
    ZeroMemory (&dm_start, sizeof (dm_start)); 
    dm_start.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(DISPLAY,ENUM_CURRENT_SETTINGS,&dm_start);

    originalDisplayWidth = dm_start.dmPelsWidth;
    originalDisplayHeight = dm_start.dmPelsHeight;    
    }

/**
\brief Set current display resolution
*/
void ZoomWindow::setDisplayResolution(int width, int height)
    {       
    DEVMODE dm_start;
    /* get current display size */
    ZeroMemory (&dm_start, sizeof (dm_start)); 
    dm_start.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(DISPLAY,ENUM_CURRENT_SETTINGS,&dm_start);


    /* update DISPLAY size if it was changed in config file */
    if (dm_start.dmPelsWidth  != width || 
	dm_start.dmPelsHeight != height )	
	{/* init file specified different DISPLAY settings then current one
	 so try to change DISPLAY settings */
	DEVMODE dm;
	LONG res;
	ZeroMemory (&dm, sizeof (dm)); 
	dm.dmSize = sizeof (dm);
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	dm.dmPelsWidth = width;
	dm.dmPelsHeight = height;
	res = ChangeDisplaySettingsEx (DISPLAY,&dm,NULL,CDS_TEST,NULL); 
	if (res!=DISP_CHANGE_SUCCESSFUL) 
	    printf("Can't changed DISPLAY settings to '%ix%i'",width,height);	       
	else	   
	    ChangeDisplaySettingsEx (DISPLAY,&dm,NULL,0,NULL); 
	}
    }
#endif

#ifdef _WIN32
/**
\brief 'captureScreen' captures the screen image under MS Windows (see [R2])

Note, this is not the fastest solution because it requires transfering framebuffer to CPU RAM
and later back to GPU.  Circa 2011, this is only executed the case case of !defined(ZW_USE_COPY_TEXTURE).
*/
void ZoomWindow::captureScreen()
    {
    HWND    hDesktopWnd = GetDesktopWindow();
    HDC    hDesktopDC = GetDC(hDesktopWnd);

    int    nScreenWidth = GetDeviceCaps(hDesktopDC,HORZRES);
    int    nScreenHeight = GetDeviceCaps(hDesktopDC,VERTRES);
    int    xOffset = 0;

    if (fullScreen)
	{
	/** experimenting with multiple monitor support, right now I want to capture just the
	right half of the desktop (Display1) and display the ZoomWindow on the left (Display0).
	*/
	nScreenWidth = 1280;   // \todo make this dependent on some ZoomWindow API controlled parameters
	nScreenHeight = 1024;
	xOffset = width; // offset of primary monitor size (ZoomWindow size for now)
	}

    /**
    copy desktop to Win32 Bitmap
    */
    HDC  hCaptureDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
    SelectObject(hCaptureDC,hCaptureBitmap);
    BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,hDesktopDC,xOffset,0,SRCCOPY);

    /**
    check resulting Win32 Bitmap size and adjust ZoomWindow::framebuffer
    */
    LPVOID     pBuf=NULL;
    BITMAPINFO bmpInfo;

    ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
    bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

    GetDIBits(hCaptureDC,hCaptureBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
    if(bmpInfo.bmiHeader.biSizeImage<=0)
        bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;
   
    if (framebuffer.width != bmpInfo.bmiHeader.biWidth || 
	framebuffer.height != bmpInfo.bmiHeader.biHeight )
	{
	delete [] framebuffer.image;
	framebuffer.width = bmpInfo.bmiHeader.biWidth;
	framebuffer.height = bmpInfo.bmiHeader.biHeight;
	framebuffer.image = new GLubyte [framebuffer.width * framebuffer.height * 4];
	framebuffer.format = GL_BGRA_EXT;
	}
    bmpInfo.bmiHeader.biCompression=BI_RGB;

    /**
    copy Win32 Bitmap data to ZoomWindow::framebuffer
    */
    GetDIBits(hCaptureDC,hCaptureBitmap,0,bmpInfo.bmiHeader.biHeight,framebuffer.image, &bmpInfo, DIB_RGB_COLORS); 

    /**
    Release device contexts...
    */
    ReleaseDC(hDesktopWnd,hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);
    }
#endif

/**
\brief return the smallest integer that is a power of two that is greater or equal to 'i'
*/
static
unsigned int nextPowerOfTwo(unsigned int i)
    {
    unsigned int 
	ones,
	highbit,
	i1=i;
    for(ones=0,highbit=0;i1;i1 >>= 1,highbit++)
	if (i1 & 0x1) ones++;
    if (!ones || ones == 1)
	return i;
    return 0x1 << highbit;
    }


/**
\brief Set desired initial OpenGL the first time the window is displayed
*/
void ZoomWindow::initializeOpenGL()
    {
    if (initialized)
	return;
    initialized = true;

   #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD 
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   #endif

   #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD && defined(ZW_USE_COPY_TEXTURE)
   #ifdef _WIN32    
    glContext = wglGetCurrentContext();
   #endif
   #endif
    }

/**
\brief 'display' is the GLUT display callback _member_ function for ZoomWindow.
It displays the captured image from the target window.
*/
void ZoomWindow::display(void)
    {
    initializeOpenGL();

    if (fullScreen)
	glClearColor(0.0,0.0,0.0,0.0); // just for testing purposes...
    else
	glClearColor(1.0,1.0,1.0,0.0);

    glClear(GL_COLOR_BUFFER_BIT);  

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
#if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD
    gluOrtho2D(viewWindow.lowerLeft[0],viewWindow.upperRight[0],viewWindow.lowerLeft[1],viewWindow.upperRight[1]);
   #ifndef ZW_USE_COPY_TEXTURE
    if (captureScreenFromOS && capture)
	captureScreen();
    if (texture.ID==0 || texture.width != framebuffer.width || texture.height != framebuffer.height)
	{
	/* delete old texture object if it exists */
	if (texture.ID)
	    glDeleteTextures(1,&texture.ID);
	texture.width = framebuffer.width;
	texture.height = framebuffer.height;

	/* generate texture object */
	glGenTextures(1, &texture.ID);
	glBindTexture(GL_TEXTURE_2D, texture.ID);

	/* set texture parameters */
	glPixelStorei(GL_UNPACK_ALIGNMENT,ALIGNMENT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
	    GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
	    GL_NEAREST);
	

	/* copy image */
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,framebuffer.width,framebuffer.height,0,framebuffer.format,GL_UNSIGNED_BYTE,
	    framebuffer.image);
	}
    else if (texture.ID)
	{	
	/* update image */
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,texture.width,texture.height,framebuffer.format,GL_UNSIGNED_BYTE,framebuffer.image);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);  	glVertex2i(0,0);
	glTexCoord2i(1,0);      glVertex2i(framebuffer.width,0);
	glTexCoord2i(1,1);      glVertex2i(framebuffer.width,framebuffer.height);
	glTexCoord2i(0,1);      glVertex2i(0,framebuffer.height);
	glEnd();
	}
   #else
    if (texture.ID)
	{	
	/* update image */
	assert(glIsTexture(texture.ID));
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	assert(glGetError() == GL_NO_ERROR);
	
	glColor3ub(255,0,0);
	glBegin(GL_QUADS);
	glTexCoord2i(0,0);  	glVertex2i(0,0);
	glTexCoord2f((float)framebuffer.width/(float)texture.width,0);      
				glVertex2i(framebuffer.width,0);
	glTexCoord2f((float)framebuffer.width/(float)texture.width,(float)framebuffer.height/(float)texture.height);      
				glVertex2i(framebuffer.width,framebuffer.height);
	glTexCoord2f(0,(float)framebuffer.height/(float)texture.height);      
				glVertex2i(0,framebuffer.height);
	glEnd();
	}
   #endif

#elif ZW_DRAW_MODE == ZW_DRAW_DRAWPIXELS
    gluOrtho2D(lowerLeft[0],lowerLeft[0]+(float)width,lowerLeft[1],lowerLeft[1]+(float)height);
    GLint rasterPos[2];
    
    bool empty = false;
    GLint lowerLeftPixel[2];

    if (lowerLeft[0] > 0)
	{
	lowerLeftPixel[0]=lowerLeft[0]/zoomLevel;
	if (lowerLeftPixel[0] >= framebuffer.width) empty = true;
	rasterPos[0]=lowerLeft[0];
	}
    else
	{
	lowerLeftPixel[0]=0;
	rasterPos[0]=0;
	}

    if (lowerLeft[1] > 0)
	{
	lowerLeftPixel[1]=lowerLeft[1]/zoomLevel;
	if (lowerLeftPixel[1] >= framebuffer.height) empty = true;	
	rasterPos[1]=lowerLeft[1];
	}
    else
	{	
	lowerLeftPixel[1]=0;
	rasterPos[1]=0;
	}

    if (!empty)
	{
	glRasterPos2iv(rasterPos);
	glPixelZoom(zoomLevel,zoomLevel);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS,lowerLeftPixel[0]);
	glPixelStorei(GL_UNPACK_SKIP_ROWS,  lowerLeftPixel[1]);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, framebuffer.width);
	glPixelStorei(GL_UNPACK_ALIGNMENT,ALIGNMENT);
	glDrawPixels(framebuffer.width-lowerLeftPixel[0],framebuffer.height-lowerLeftPixel[1],GL_RGB,GL_UNSIGNED_BYTE,framebuffer.image);
	glPixelZoom(1.0,1.0);
	}
#endif
   #if 0
    // DrawImmediately
    glColor3ub(255,0,0);
    glRecti(100,100,200,200);
   #endif

    
   #ifdef USE_DOUBLE_BUFFER
    glutSwapBuffers();
   #else
    /* always flush at end of frame since were are not double buffering */
    glFlush();
   #endif

    /* trigger another redisplay */
    if (capture)
	glutPostRedisplay();
    }

/**
\brief 'motion' responds to GLUT motion callbacks
*/
void ZoomWindow::motion (int x, int y)
    {
    int delta[2];
    if (last[0] > 0)
	{
	delta[0] =   last[0] - x;
	delta[1] = -(last[1] - y);

       #if ZW_DRAW_MODE == ZW_DRAW_TEXTURE_QUAD 
	float 
	    windowToWorld [2] = {(float)(viewWindow.upperRight[0]-viewWindow.lowerLeft[0]) / (float)width,
			         (float)(viewWindow.upperRight[1]-viewWindow.lowerLeft[1]) / (float)height};
	if (drag)
	    {
	    viewWindow.lowerLeft[0]  += delta[0]*windowToWorld[0];
	    viewWindow.lowerLeft[1]  += delta[1]*windowToWorld[1];
	    viewWindow.upperRight[0] += delta[0]*windowToWorld[0];
	    viewWindow.upperRight[1] += delta[1]*windowToWorld[1];
	    }
	else if (zoom)
	    {
	    zoomView((float)delta[1]/100, windowToWorld);
	    }
       #elif ZW_DRAW_MODE == ZW_DRAW_DRAWPIXELS
	if (drag)
	    {
	    lowerLeft[0] += delta[0];
	    lowerLeft[1] += delta[1];
	    }
	else if (zoom)
	    {
	    zoomLevel += (float)delta[1] / 100;
	    if (zoomLevel < 1.0)
		zoomLevel = 1.0;
	    }
       #endif
	}

    last[0] = x; last[1] = y;
    glutSetWindow(this->windowID_);
    glutPostRedisplay();
    }

void ZoomWindow::zoomView(float scale, float windowToWorld [2])
    {
    float zoomCenterW [2]; // zoomCenter in world coordinates
    ViewWindow vw; // 'viewWindow'

    /* scale viewWindow about the center zoomWindow by scale factor determined by mouse motion */
    zoomCenterW [0] = zoomCenter [0] * windowToWorld[0] + viewWindow.lowerLeft[0];
    zoomCenterW [1] = zoomCenter [1] * windowToWorld[1] + viewWindow.lowerLeft[1];

    vw.upperRight[0] = (viewWindow.upperRight[0] - zoomCenterW[0]) * scale + viewWindow.upperRight[0];
    vw.upperRight[1] = (viewWindow.upperRight[1] - zoomCenterW[1]) * scale + viewWindow.upperRight[1];	    	    
    vw.lowerLeft[0] = (viewWindow.lowerLeft[0] - zoomCenterW[0]) * scale + viewWindow.lowerLeft[0];
    vw.lowerLeft[1] = (viewWindow.lowerLeft[1] - zoomCenterW[1]) * scale + viewWindow.lowerLeft[1];

    /* clamp degree of zoom level to >= 1 */
    if (vw.upperRight[0] - vw.lowerLeft[0] <= width/MIN_ZOOM &&  vw.upperRight[0] > vw.lowerLeft[0] &&
    vw.upperRight[1] - vw.lowerLeft[1] <= height/MIN_ZOOM && vw.upperRight[1] > vw.lowerLeft[1])
    viewWindow = vw;
    }

/**
\brief 'reshape' responds to GLUT reshape callbacks
*/
void ZoomWindow::reshape(int w, int h)
    {
   #ifdef ZW_USE_TEXTURE
    /* return if window is minimized */
    if (w == 0 || h == 0)
	return;

    /* use the entire Window area for OpenGL drawing */
    glViewport( 0, 0,width, height);

    /* place the coordinate origin at the lower left hand corner of the window, 
    but keep the view window size equal to the size of the Window area as
    measured in pixels.  The purpose is to have one unit in OpenGL
    coordinates correspond to one pixel unit in the window.  This makes
    sense because this program is designed for doing drawing operations on
    individual pixels.
    */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();    
    viewWindow.upperRight[0] += (float)(w-width) * ((viewWindow.upperRight[0]-viewWindow.lowerLeft[0])/width);
    viewWindow.upperRight[1] += (float)(h-height) * ((viewWindow.upperRight[1]-viewWindow.lowerLeft[1])/height);
    gluOrtho2D(viewWindow.lowerLeft[0],viewWindow.upperRight[0],viewWindow.lowerLeft[1],viewWindow.upperRight[1]);
    width = w;
    height = h;
   #else   
    width = w;
    height = h;
    matchWorldCoordinatesToPixelCoordinates(w,h);
   #endif
    glutPostRedisplay();
    }


/**
\brief 'passiveMotion' responds to GLUT passiveMotion callbacks
*/
void ZoomWindow::passiveMotion(int x, int y)
    {
    last[0] = x; last[1] = y;   
    }

/**
\brief 'mouse' responds to GLUT mouse callbacks
*/
void ZoomWindow::keyboard(unsigned char key, int x, int y)
    {   
    switch(key)
	{
	case '\x1B': // ASCII 'ESC'
	    exit(0);
	    break;
	}
    }

/**
\brief 'mouse' responds to GLUT mouse callbacks
*/
void ZoomWindow::mouse(int button, int state, int x, int y)
    {    
    float 
	windowToWorld [2] = {(float)(viewWindow.upperRight[0]-viewWindow.lowerLeft[0]) / (float)width,
			     (float)(viewWindow.upperRight[1]-viewWindow.lowerLeft[1]) / (float)height};

    switch (button)
	{
	case GLUT_LEFT_BUTTON:
	    drag = state == GLUT_DOWN;
	    break;
	case GLUT_WHEEL_UP:
	    if (state == GLUT_UP)
		{
		zoomCenter [0] = x, zoomCenter [1] = -y + height;
		zoomView( 0.1,windowToWorld);
		}
	    break;
	case GLUT_WHEEL_DOWN:
	    if (state == GLUT_UP)
		{
		zoomCenter [0] = x, zoomCenter [1] = -y + height;
		zoomView(-0.1,windowToWorld);
		}
	    break;
	case GLUT_RIGHT_BUTTON:
	    zoom = state == GLUT_DOWN;
	    if (state == GLUT_DOWN)
		zoomCenter [0] = x, zoomCenter [1] = -y + height;
	    break;
	}	
#if 0
    /** GRRR... no mouse wheel support in standard GLUT 3.7.6 */
    if (state == GLUT_UP )
	{
	if ( button == GLUT_WHEEL_UP )
	    {
	    printf("Wheel Up\n");
	    }
	else if( button == GLUT_WHEEL_DOWN )
	    {
	    printf("Wheel Down\n");
	    }
	}
#endif
    glutSetWindow(this->windowID_);
    glutPostRedisplay();
    }


ZoomWindow::CaptureDesktop::CaptureDesktop()
    {
    }

void ZoomWindow::CaptureDesktop::display()
    {
    /* reshape */
    reshape(width,height);

#if 1
    /* keep this fullscreen window at bottom of Window system's hierarchy */
    /** \todo find a more robust solution to this */
    for (int i=20;i>0;i--)
	glutPushWindow();
#endif

    /* capture framebuffer */
    ITCS4120::OpenGLTrainer::ZoomWindow::captureFramebuffer();    
    }

void ZoomWindow::CaptureDesktop::reshape (int w, int h)
    {
    width = w; height = h;
    ITCS4120::OpenGLTrainer::matchWorldCoordinatesToPixelCoordinates(w,h);
    }

void ZoomWindow::CaptureDesktop::glutCreateWindow (const char* name)
    {
    glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
    Window::glutCreateWindow(name);   
    glutFullScreen();
    for (int i=20;i>0;i--)
	glutPushWindow();
    }
