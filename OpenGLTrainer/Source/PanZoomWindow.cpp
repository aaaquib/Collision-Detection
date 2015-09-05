/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief PanZoomWindow.cpp implements the PanZoomWindow class.

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

#include <OpenGLTrainer/PanZoomWindow.h>

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
// NONE
/*******************************************************************************
    File Scope (static/private) globals
*******************************************************************************/
// NONE

/*******************************************************************************
    File Scope Macros 
*******************************************************************************/
// NONE

/*******************************************************************************
    Exported (extern) Globals
*******************************************************************************/
//ZoomWindow ZoomWindow::zoomWindow;

/*******************************************************************************
    Exported (extern) Functions
*******************************************************************************/



/**
\brief Construct this PanZoomWindow so that the view window in world coordinates
bounded by axis-aligned rectangle with corners viewLowerLeft[2] and viewUpperRight[2]
is mapped to the PanZoomWindow's GLUT window.
*/
PanZoomWindow::PanZoomWindow (const float viewLowerLeft[2], const float viewUpperRight[2])

    {
    /* GLUT window initial size */
    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;

    /* view window size */
    viewWindow_.lowerLeft[0]=viewLowerLeft[0];
    viewWindow_.lowerLeft[1]=viewLowerLeft[1];
    viewWindow_.upperRight[0]=viewUpperRight[0];
    viewWindow_.upperRight[1]=viewUpperRight[1];

    maxViewWindowWidth = viewWindow_.upperRight[0] - viewWindow_.lowerLeft[0];

    /* misc. */
    last [0] = last[1] = -1;
    drag = zoom = false;
    }

#if 0
/**
\brief Construct this ZoomWindow
*/
PanZoomWindow::PanZoomWindow() 
    {
    /* GLUT window initial size */
    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;

    /* view window size */
    viewWindow_.lowerLeft[0]=0;
    viewWindow_.lowerLeft[1]=0;
    viewWindow_.upperRight[0]=DEFAULT_WIDTH;
    viewWindow_.upperRight[1]=DEFAULT_HEIGHT;
    maxViewWindowWidth = viewWindow_.upperRight[0] - viewWindow_.lowerLeft[0];

    /* misc. */
    last [0] = last[1] = -1;
    drag = zoom = false;
    initialized = false;
    }
#endif

/*******************************************************************************
    PRIVATE FUNCTIONS (static functions/private member functions)
*******************************************************************************/

/**
\brief 'motion' responds to GLUT motion callbacks.  This handles the pan and zoom.
Sub-classes that override this member function must call this member function as well.  See \ref PanZoomWindow_USAGE.
*/
void PanZoomWindow::motion (int x, int y)
    {
    GLint delta[2];
    if (last[0] > 0)
	{
	delta[0] =   last[0] - x;
	delta[1] =   last[1] - y;

	float 
	    deltaW[2];
	mouseCoordinatesToWorldCoordinatesVector(delta,deltaW);
	using namespace std;
       #if 0
	cout << "deltaW: " << deltaW[0] << " " << deltaW[1] << endl;
       #endif
	if (drag)
	    {
	    viewWindow_.lowerLeft[0]  += deltaW[0];
	    viewWindow_.lowerLeft[1]  += deltaW[1];
	    viewWindow_.upperRight[0] += deltaW[0];
	    viewWindow_.upperRight[1] += deltaW[1];
	    }
	else if (zoom)
	    {
	    zoomView((delta[1]/100.0+1.0));
	    }
	}

    last[0] = x; last[1] = y;
    glutSetWindow(this->windowID_);
    glutPostRedisplay();
    }

/**
\brief 'reshape' responds to GLUT reshape callbacks in order to manage panning/zooming.  
Sub-classes that override this member function must call this member function as well.  See \ref PanZoomWindow_USAGE.
*/
void PanZoomWindow::reshape(int w, int h)
    {
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
    gluOrtho2D(viewWindow_.lowerLeft[0],viewWindow_.upperRight[0],viewWindow_.lowerLeft[1],viewWindow_.upperRight[1]);
    width = w;
    height = h;

    glutPostRedisplay();
    }

/**
\brief Setup OGL view matrices.  This should be called at the beginning of a sub-class's member function ::display 
before any OGL drawing is done.
*/
void PanZoomWindow::setOpenGLView()
    {
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
    gluOrtho2D(viewWindow_.lowerLeft[0],viewWindow_.upperRight[0],viewWindow_.lowerLeft[1],viewWindow_.upperRight[1]);
    }
/**
\brief Convert a point's x,y coordinate in GLUT mouse coordinates to the point's world coordinates.
*/
void PanZoomWindow::mouseCoordinatesToWorldCoordinatesPoint(const int mouse[2], float world[2])
    {
    float 
	windowToWorld [2] = {(float)(viewWindow_.upperRight[0]-viewWindow_.lowerLeft[0]) / (float)width,
			     (float)(viewWindow_.upperRight[1]-viewWindow_.lowerLeft[1]) / (float)height};
    world [0] = mouse[0]*windowToWorld[0] + viewWindow_.lowerLeft[0];
    world [1] = (height-mouse[1])*windowToWorld[1] + viewWindow_.lowerLeft[1];
    }

/**
\brief Convert a vector's x,y coordinate in GLUT mouse coordinates to the vector's world coordinates.
*/
void PanZoomWindow::mouseCoordinatesToWorldCoordinatesVector(const int mouse[2], float world[2])
    {
    float 
	windowToWorld [2] = {(float)(viewWindow_.upperRight[0]-viewWindow_.lowerLeft[0]) / (float)width,
			     (float)(viewWindow_.upperRight[1]-viewWindow_.lowerLeft[1]) / (float)height};
    world [0] = mouse[0]*windowToWorld[0];
    world [1] = -mouse[1]*windowToWorld[1];
    }


/**
\brief 'mouse' responds to GLUT mouse callbacks by panning and zooming this PanZoomWindow::viewWindow_ in world coordinates.
Sub-classes that override this member function must call this member function as well.  See \ref PanZoomWindow_USAGE.
*/
void PanZoomWindow::mouse(int button, int state, int x, int y)
    {    
    float 
	windowToWorld [2] = {(float)(viewWindow_.upperRight[0]-viewWindow_.lowerLeft[0]) / (float)width,
			     (float)(viewWindow_.upperRight[1]-viewWindow_.lowerLeft[1]) / (float)height};

    switch (button)
	{
	case GLUT_LEFT_BUTTON:
	    {
	    int mods = glutGetModifiers();
	    if (mods & GLUT_ACTIVE_CTRL && mods & GLUT_ACTIVE_SHIFT)
		{
		zoom = state == GLUT_DOWN;
		if (state == GLUT_DOWN)
		    zoomCenter [0] = x, zoomCenter [1] = y;
		}
	    else if (mods & GLUT_ACTIVE_CTRL)
		drag = state == GLUT_DOWN;
	    else
		{
		drag = false;
		zoom = false;
		}
	    break;
	    }
       #ifdef GLUT_WHEEL_UP    
	case GLUT_WHEEL_UP:
	    if (state == GLUT_UP)
		{
		zoomCenter [0] = x, zoomCenter [1] = y;
		zoomView( 0.9);
		}
	    break;
	case GLUT_WHEEL_DOWN:
	    if (state == GLUT_UP)
		{
		zoomCenter [0] = x, zoomCenter [1] = y;
		zoomView( 1.1);
		}
	    break;
       #endif 
	}	

    glutSetWindow(this->windowID_);
    glutPostRedisplay();
    }

/**
\brief 'passiveMotion' keeps track of the last mouse coordinate location.
Sub-classes that override this member function must call this member function as well.  See \ref PanZoomWindow_USAGE.
*/
void PanZoomWindow::passiveMotion(int x, int y)
    {
    last[0] = x; last[1] = y;   
    }

/**
\brief 'keyboard' exits the program on the ESCAPE keypress.
Sub-classes that override this member function must call this member function as well.  See \ref PanZoomWindow_USAGE.
*/
void PanZoomWindow::keyboard(unsigned char key, int x, int y)
    {   
    switch(key) 
	{
	case '\x1B': // ASCII 'ESC'
	    exit(0);
	    break;
	}
    }

/**
\brief 'zoomView' zooms this window's viewWindow_ about the point at location this->zoomCenter by
the scale factor 'scale'
*/
void PanZoomWindow::zoomView(float scale)
    {
    float zoomCenterW [2]; // zoomCenter in world coordinates
    ViewWindow vw; // 'viewWindow_'

    /* scale viewWindow_ about the point 'zoomCenter' by scale factor 'scale' */
    mouseCoordinatesToWorldCoordinatesPoint(zoomCenter,zoomCenterW);

    vw.upperRight[0] = (viewWindow_.upperRight[0] - zoomCenterW[0]) * scale + zoomCenterW[0];
    vw.upperRight[1] = (viewWindow_.upperRight[1] - zoomCenterW[1]) * scale + zoomCenterW[1];    
    vw.lowerLeft[0] = (viewWindow_.lowerLeft[0] - zoomCenterW[0]) * scale + zoomCenterW[0];
    vw.lowerLeft[1] = (viewWindow_.lowerLeft[1] - zoomCenterW[1]) * scale + zoomCenterW[1];

    if (vw.upperRight[0] > vw.lowerLeft[0] && (vw.upperRight[0] - vw.lowerLeft[0]) <= maxViewWindowWidth &&
	vw.upperRight[1] > vw.lowerLeft[1])
	viewWindow_ = vw;
    }