/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief OpenGLTrainer.cpp implements various functions to help ITCS4120 students get up 
and running quickly in the first few programming assignments.

This functions in this file can generally just be used as given in the per project 
skeleton code without students being concerned with how these functions work.  We will
cover all this stuff eventually in later sections of the course.

TO DO LIST:
\todo

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] Donald Hearn and M. Pauline Baker.  Computer Graphics with OpenGL: Third Edition.

\internal
*/
#ifdef WIN32
#include <windows.h>
#endif
//#include <GL/gl.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <OpenGLTrainer\OpenGLTrainer.h>
#include <OpenGLTrainer\Framebuffer.h>

#include <iostream>

using namespace ITCS4120;
using namespace ITCS4120::OpenGLTrainer;

/*******************************************************************************
    File Scope Data Types
*******************************************************************************/

/*******************************************************************************
    File Scope Functions
*******************************************************************************/
inline void glutBitmapString(void* font,char string[])
    {
    const size_t size = strlen(string)-1;
    for(size_t i=0;i<=size;i++)
	glutBitmapCharacter(font,string[i]);
    }

/*******************************************************************************
    File Scope (static/private) globals
*******************************************************************************/
OpenGLTrainer::Window::IDtoWindow_t OpenGLTrainer::Window::IDtoWindow_;

/*******************************************************************************
    Exported (extern) Globals
*******************************************************************************/
/*
*/
OpenGLTrainer::ExperimentControl OpenGLTrainer::experimentControl =
    {
    /* enable/disable message when calling display callback */
    true,//bool traceDisplay;
    /* enable/disable message when calling idle callback */
    true,//bool traceIdle;

    /* enable/disable call to post redisplay in idle callback */
    true,//bool idlePostRedisplay;

    /* enable/disable skiping all drawing in display callback */
    true, //bool enableDisplay;

    /* enable/disable glFlush at end of display callback */
    true, //bool flush;

    /* enable/disable glutSwapBuffer at end of display callback */
    true //bool swapBuffer;

    /* enable/disable clearing framebuffer at start of display callback */
    ,true//bool clear;

    /* enable/disable call to post redisplay in motion callback */
    ,true // bool motionidlePostRedisplay;

    };

/*******************************************************************************
    Exported (extern) Functions
*******************************************************************************/

/**
\brief 'matchWorldCoordinatesToPixelCoordinates' is a GLUT reshape callback that
updates OpenGL view transformations so that OpenGL world coordinates directly map to
pixel coordinates in the Window's framebuffer with (0,0) at the lower-left and
(width-1,height-1) at the upper- right where width and height are the current
size of Window sub-window.

Usage:

\code
int main (int argc, char** argv)
    {
    glutInit();
	...
    glutReshapeFunc(OpenGLTrainer::matchWorldCoordinatesToPixelCoordinates);
	...
    glutMainLoop();
    }
\endcode

\note ITCS4120: For early assignments, students can just use this function without
worrying about how it works.

*/
void OpenGLTrainer::matchWorldCoordinatesToPixelCoordinates(int width, int height)
   {
   /* avoid making bad GL calls for degenerate window */
   if (width == 0 || height== 0)
	return;

   /* use the entire Window area for OpenGL drawing */
   glViewport( 0, 0,(GLsizei)width,(GLsizei)height);

   /* place the coordinate origin at the lower left hand corner of the window, 
   but keep the view window size equal to the size of the Window area as
   measured in pixels.  The purpose is to have one unit in OpenGL
   coordinates correspond to one pixel unit in the window.  This makes
   sense because this program is designed for doing drawing operations on
   individual pixels.
   */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   // debating which of these two is correct!
#if 1
   gluOrtho2D( 0,width,0,height);
#else
   gluOrtho2D( 0,width-1,0,height-1); 
#endif
   
   }

/**
\brief When placed at end of your display callback (but before any
call to glFlush to glutSwapBuffer), 'showFrameRate' displays the framerate in 
the lower left corner of the GLUT window.

\code
void display(void)
    {
    ...

    ITCS4120::OpenGLTrainer::showFrameRate();
    glSwapBuffer(); // if using double buffering 
    }

\endcode

\warning Uses standard clock() timer whose limited resolution may not yield 
    very accurate results   
\warning Doesn't support multiple GLUT Windows (uses static local variables)
\warning Not Thread Safe - Uses static local variables
*/
void OpenGLTrainer::showFrameRate()
	{
	static float lastTime = 0.0f;
	clock_t now;

	/* compute current time and time since last frame */
	now = clock ();    
	if (lastTime != 0.0f)
		{
		char message[512];
		float seconds =(float)(now - lastTime)/CLK_TCK;

		/* avoid making bad GL calls for degenerate window */
		if (!glutGet(GLUT_WINDOW_WIDTH)==0 && !glutGet(GLUT_WINDOW_HEIGHT)==0)
			{
			glPushAttrib(GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D( 0,glutGet(GLUT_WINDOW_WIDTH),0,glutGet(GLUT_WINDOW_HEIGHT));

			sprintf(message,"Frame Rate: %5.2f",1.0/seconds);

			glColor3ub (255,255,255);	
			glRasterPos2i(0,0);	
			glutBitmapString(  GLUT_BITMAP_TIMES_ROMAN_24,message);	
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopAttrib();
			}
		}

	/* record current time */
	lastTime=now;
	}

/**
\brief init GLUT menus for toggle 'experimentControl' data members
*/
void OpenGLTrainer::ExperimentControl::initMenus(unsigned int mode)
    {
    glutCreateMenu(OpenGLTrainer::ExperimentControl::menuCallback);
    glutAddMenuEntry(experimentControl.traceDisplay?"traceDisplay On":"traceDisplay Off",TRACE_DISPLAY);
    glutAddMenuEntry(experimentControl.traceIdle?"traceIdle On":"traceIdle Off",TRACE_IDLE);
    glutAddMenuEntry(experimentControl.enableDisplay?"enableDisplay On":"enableDisplay Off",SKIP_DISPLAY);
    glutAddMenuEntry(experimentControl.idlePostRedisplay?"idlePostRedisplay On":"idlePostRedisplay Off",IDLE_POST_REDISPLAY);
    glutAddMenuEntry(experimentControl.motionPostRedisplay?"motionPostRedisplay On":"motionPostRedisplay  Off",MOTION_POST_REDISPLAY);    
    if (mode == GLUT_SINGLE)
	glutAddMenuEntry(experimentControl.flush?"flush On":"flush Off",FLUSH);
    if (mode == GLUT_DOUBLE)
	glutAddMenuEntry(experimentControl.swapBuffer?"swapBuffer On":"swapBuffer Off",SWAP_BUFFER);
    glutAddMenuEntry(experimentControl.clear?"clear On":"clear Off",CLEAR);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    }


/**
\brief handle selection of ExperimentControl menu selection
*/
void OpenGLTrainer::ExperimentControl::menuCallback(int value)
    {
    switch(value)
	{
	case TRACE_DISPLAY: 
	    experimentControl.traceDisplay = !experimentControl.traceDisplay;
	    glutChangeToMenuEntry(TRACE_DISPLAY+1,experimentControl.traceDisplay?"traceDisplay On":"traceDisplay Off",TRACE_DISPLAY);
	    break;
	case TRACE_IDLE: 
	    experimentControl.traceIdle = !experimentControl.traceIdle;
	    glutChangeToMenuEntry(TRACE_IDLE+1,experimentControl.traceIdle?"traceIdle On":"traceIdle Off",TRACE_IDLE);
	    break;
	case SKIP_DISPLAY:
	    experimentControl.enableDisplay = !experimentControl.enableDisplay;
	    glutChangeToMenuEntry(SKIP_DISPLAY+1,experimentControl.enableDisplay?"enableDisplay On":"enableDisplay Off",SKIP_DISPLAY);
	    break;
	case IDLE_POST_REDISPLAY:
	    experimentControl.idlePostRedisplay = !experimentControl.idlePostRedisplay;
	    glutChangeToMenuEntry(IDLE_POST_REDISPLAY+1,experimentControl.idlePostRedisplay ?"idlePostRedisplay On":"idlePostRedisplay Off",IDLE_POST_REDISPLAY);
	    break;
	case MOTION_POST_REDISPLAY:
	    experimentControl.motionPostRedisplay = !experimentControl.motionPostRedisplay;
	    glutChangeToMenuEntry(MOTION_POST_REDISPLAY+1,experimentControl.motionPostRedisplay ?"motionPostRedisplay On":"motionPostRedisplay Off",MOTION_POST_REDISPLAY);
	    break;
	case FLUSH:
	    experimentControl.flush = !experimentControl.flush;
	    glutChangeToMenuEntry(FLUSH+1,experimentControl.flush ?"flush On":"flush Off",FLUSH);
	    break;
	case SWAP_BUFFER:
	    experimentControl.swapBuffer = !experimentControl.swapBuffer;
	    glutChangeToMenuEntry(FLUSH+1,experimentControl.swapBuffer ?"swapBuffer On":"swapBuffer Off",SWAP_BUFFER);
	    break;
	case CLEAR:
	    experimentControl.clear = !experimentControl.clear;
	    glutChangeToMenuEntry(CLEAR+1,experimentControl.clear ?"clear On":"clear Off",CLEAR);
	    break;
	}
    }

/**
\brief 'glutCreateWindow' wraps and replaces ::glutCreateWindow(name). Hence it creates a GLUT window. Further, it
registers GLUT callback functions to call this Window object's event related virtual member functions.

See \ref Window_Usage.
*/
void OpenGLTrainer::Window::glutCreateWindow(const char* name)
    {
    windowID_ = ::glutCreateWindow(name);
    Window::IDtoWindow_[windowID_] = this;

    if (callbackMethod == AUTO_CALLBACKS)
	{
	glutDisplayFunc(static_display);
	glutMotionFunc(static_motion);
	glutMouseFunc(static_mouse);
	glutPassiveMotionFunc(static_passiveMotion);
	glutReshapeFunc(static_reshape);
	glutKeyboardFunc(static_keyboard);
	}
    }


/**
\page Window

\section Window_Usage Window Usage

The users create a derived class from Window and then override the virtual member
functions which are named after the standard GLUT window callbacks.

OpenGLTrainer::Window Example:

\code
class MyWindow : public ITCS4120::OpenGLTrainer::Window
    {
    // override Window's GLUT callback member functions
    virtual void display(void);
    };

void MyWindow::display(void)
    {
    ... do your display callback stuff here ...
    }

int main (int argc, char** argv)
    {
    MyWindow window;
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    window.glutCreateWindow("Window");

    glutMainLoop();
    }
\endcode

The above has a similar effect as the following pure GLUT code:

\code

void display(void)
    {
    ... do your display callback stuff here ...
    }

int main (int argc, char** argv)
    {    
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutCreateWindow("Window");
    glutDisplayFunc(display);

    glutMainLoop();
    }
\endcode

*/

/**
\brief construct a Window using default settings.  

This automatically registers the Window event related virtual member functions to be called by the GLUT libraries event handling system.
*/
OpenGLTrainer::Window::Window()
    {
    callbackMethod = AUTO_CALLBACKS;
    width=0;
    height=0;
    windowID_ = -1;
    }

/**
\brief [ADVANCED] construct a Window and set the CallbackMethod to \a cm
*/
OpenGLTrainer::Window::Window(CallbackMethod cm)
    {
    callbackMethod = cm;
    width=0;
    height=0;
    windowID_ = -1;
    }

/**
\brief 'display' is the GLUT display callback function.  This function looks up the
Window object corresponding to the GLUT window that triggered this callback and
executes Window::display
*/
void OpenGLTrainer::Window::static_display(void)
    {
    IDtoWindow_t::iterator i = IDtoWindow_.find(glutGetWindow());
    if (i != IDtoWindow_.end())
	(*i).second->display();
    }

/**
\brief GLUT keyboard callback.  This function looks up the
Window object corresponding to the GLUT window that triggered this callback and
executes Window::keyboard

\warning only singleton Window is supported
*/
void OpenGLTrainer::Window::static_keyboard (unsigned char key,int x, int y)
    {
    IDtoWindow_t::iterator i = IDtoWindow_.find(glutGetWindow());
    if (i != IDtoWindow_.end())
	(*i).second->keyboard(key,x,y);
    }

/**
\brief GLUT motion callback. This function looks up the
Window object corresponding to the GLUT window that triggered this callback and
executes Window::motion.

\warning only singleton Window is supported
*/
void OpenGLTrainer::Window::static_motion (int x, int y)
    {
    IDtoWindow_t::iterator i = IDtoWindow_.find(glutGetWindow());
    if (i != IDtoWindow_.end())
	(*i).second->motion(x,y);
    }

/**
\brief GLUT passiveMotion callback.  This function looks up the
Window object corresponding to the GLUT window that triggered this callback and
executes Window::passiveMotion.

\warning only singleton Window is supported
*/
void OpenGLTrainer::Window::static_passiveMotion (int x, int y)
    {
    IDtoWindow_t::iterator i = IDtoWindow_.find(glutGetWindow());
    if (i != IDtoWindow_.end())
	(*i).second->passiveMotion(x,y);
    }

/**
\brief GLUT mouse callback. This function looks up the
Window object corresponding to the GLUT window that triggered this callback and
executes Window::mouse.

\warning only singleton Window is supported
*/
void OpenGLTrainer::Window::static_mouse(int button, int state, int x, int y)
    {    
    IDtoWindow_t::iterator i = IDtoWindow_.find(glutGetWindow());
    if (i != IDtoWindow_.end())
	(*i).second->mouse(button,state,x,y);
    }


/**
\brief GLUT reshape callback.  This function looks up the
Window object corresponding to the GLUT window that triggered this callback and
executes Window::reshape.

\warning only singleton Window is supported

FOOTNOTES:
- [F2] this test was needed to avoid crash when destructing a fullscreen Window 
while processing an exit()
*/
void OpenGLTrainer::Window::static_reshape(int w,int h)
    {    
    if(IDtoWindow_.empty())  // see [F2]
	return;
    IDtoWindow_t::iterator i = IDtoWindow_.find(glutGetWindow());
    if (i != IDtoWindow_.end())
	{
	(*i).second->reshape(w,h);
	(*i).second->width = w; (*i).second->height = h;
	}
    }


/**
\brief 'display' is called whenever the GLUT display callback is triggered for this Window.


 \sa CallbackMethod
*/
void OpenGLTrainer::Window::display(void)
    {
    }
/**
\brief 'motion' is called whenever GLUT motion callback is triggered for this Window.


 \sa CallbackMethod
*/
void OpenGLTrainer::Window::motion (int x, int y)
    {
    }

/**
\brief 'reshape' is called whenever GLUT reshape callback is triggered for this Window.


 \sa CallbackMethod
*/
void OpenGLTrainer::Window::reshape (int w, int h)
    {
    }


/**
\brief 'passiveMotion' is called whenever GLUT passiveMotion callback is triggered for this Window.


 \sa CallbackMethod
*/
void OpenGLTrainer::Window::passiveMotion (int x, int y)
    {
    }

/**
\brief 'mouse' is called whenever GLUT mouse callback is triggered for this Window.


 \sa CallbackMethod
*/
void OpenGLTrainer::Window::mouse (int button, int state, int x, int y)
    {
    }

/**
\brief 'keyboard' is called whenever GLUT keyboard callback is triggered for this Window.


 \sa CallbackMethod
*/
void OpenGLTrainer::Window::keyboard (unsigned char key, int x, int y)
    {
    }