/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief Framebuffer.cpp implements the Framebuffer class.

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

#include <OpenGLTrainer/Framebuffer1.h>

#include <assert.h>
#include <iostream>

#ifdef USE_TAKE1_BY_DEFAULT
using namespace ITCS4120::OpenGLTrainer;
#else
using namespace ITCS4120::OpenGLTrainer::Take1;
#endif

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
Framebuffer::WindowIDToFramebuffer Framebuffer::windowIDToFramebuffer;

/*******************************************************************************
    File Scope Macros 
*******************************************************************************/
/**
\brief Empirically glFlush isn't enough to force each new GL_POINT to be rendered
to the display so use glFinish instead.
*/
#define GL_FORCE_PIXEL_OUT() glFinish()
//#define GL_FORCE_PIXEL_OUT() glFlush()

/*******************************************************************************
    Exported (extern) Globals
*******************************************************************************/
// NONE

/*******************************************************************************
    Exported Functions
*******************************************************************************/

/*
\brief Construct a Framebuffer
*/
Framebuffer::Framebuffer()
    {
    width_ = height_ = 0;
    mode_ = DrawFastest;
    }

/*
\brief Set mode to DrawImmediately
*/
void Framebuffer::drawImmediately()
    {
    mode(DrawImmediately);
    }

/*
\brief Set mode to DrawFastest
*/
void Framebuffer::drawFastest()
    {
    mode(DrawFastest);
    }

/**
\brief 'lock' allows access to the framebuffer for the current GLUT window.  lock' returns
a pointer 'framebuffer' that points to a Framebuffer object which can access the framebuffer 
of the current GLUT window.

Between calls to 'lock' and 'unlock' no direct OpenGL calls should be made.
*/
void Framebuffer::lock(Framebuffer* & framebuffer)
    {
    using namespace std;    
    /**
	get Pixelbuffer associated with the current GLUT window
     **/
    int windowID = glutGetWindow();
    if (!windowID)
	{
	cout << "Framebuffer::lock() called outside of display callback" << endl;
	exit(1);
	}
    
    WindowIDToFramebuffer::iterator i;
    i = windowIDToFramebuffer.find(windowID);
    if (i == windowIDToFramebuffer.end())
	{
	framebuffer = new Framebuffer;
	windowIDToFramebuffer[windowID] = framebuffer;
	}
    else
	framebuffer  = (*i).second;
    framebuffer->lock();
    }

/**
\brief 'unlock' unlocks the framebuffer 'framebuffer' associated with the current 
GLUT window.  
*/
void Framebuffer::unlock(Framebuffer* framebuffer)
    {
    framebuffer->unlock();
    }

/**
\brief Set pixel at location 'x','y', to RGB color value (red,green,blue)
*/
void Framebuffer::setPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue)
    {
    if (mode_ == DrawImmediately)
	{
	glBegin(GL_POINTS);
	glColor3ub(red,green,blue);
	glVertex2i(x,y);
	glEnd();
	GL_FORCE_PIXEL_OUT();
	}
    else
	{
	glColor3ub(red,green,blue);
	glVertex2i(x,y);
	}
    }

/**
\brief Set the row of pixels starting location 'x','y' to the 
pixels in array 'rgb[][3]' which has 'nPixels' stored in it.

\warning results are undefined if rgb[][3] extends past the end of the 
framebuffer
*/
void Framebuffer::setRow(int x, int y, int nPixels, GLubyte rgb[][3])
    {
    if (mode_ == DrawFastest)
	{
	glEnd();
	glRasterPos2i(x,y);
	glDrawPixels(nPixels,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);	
	glBegin(GL_POINTS);
	}
    else
	{
	glRasterPos2i(x,y);
	glDrawPixels(nPixels,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);
	GL_FORCE_PIXEL_OUT();
	}
    }

/**
\brief Set pixel at location 'locaiton' to RGB color value 'rgb'
*/
void Framebuffer::setPixel(const int location[2], const GLubyte rgb[3])
    {
    if (mode_ == DrawImmediately)
	{
	glBegin(GL_POINTS);
	glColor3ubv(rgb);
	glVertex2iv(location);
	glEnd();
	GL_FORCE_PIXEL_OUT();
	}
    else
	{
	glColor3ubv(rgb);
	glVertex2iv(location);
	}
    }

/**
\brief Read RGB value of pixel 'x','y' into 'rgb'
*/
void Framebuffer::getPixel(int x, int y, GLubyte rgb[3])
    {
    if (mode_ == DrawImmediately)
	{
	glReadPixels(x,y,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);	    
	}
    else
	{
	glEnd();
	glReadPixels(x,y,1,1,GL_RGB,GL_UNSIGNED_BYTE,rgb);
	glBegin(GL_POINTS);	    
	}
    }


/**
\brief 'mode' sets the mode for framebuffer writes
*/
void Framebuffer::mode(Mode m)
    {
    if (m != mode_)
	{
	if (m == DrawFastest)
	    {// switching into DrawFastest, so just switch to drawing to BACK buffer
	    mode_ = DrawFastest;

	    /** in DrawImmediately Mode were temporarily drawing to front buffer to see
	        immediate results.  Now we're returning to DrawFastest Mode.  So
		copy our Front buffer to the Back buffer then swap buffers and return to
		drawing to back buffer */
	    glReadBuffer(GL_FRONT);
	    glDrawBuffer(GL_BACK);
	    glRasterPos2i(0,0);
	    glCopyPixels(0,0,width_,height_,GL_COLOR);
	    glutSwapBuffers();

	    glBegin(GL_POINTS);	    
	    }
	else 
	    {// switching into DrawImmediately, so update framebuffer with cached framebuffer
	     // and switch to drawing to both BACK and FRONT buffers.
	    mode_ = DrawImmediately;
	    glEnd();

	    glutSwapBuffers();
	    glDrawBuffer(GL_FRONT);
	    }
	}
    }

/*******************************************************************************
    PRIVATE FUNCTIONS (static func's,private member func's, etc.)
*******************************************************************************/

/**
\brief 'lock' allows access to the framebuffer for the current GLUT window.  Between calls to
'lock' and 'unlock' no OpenGL calls should be made.
*/
void Framebuffer::lock()
    {
    int 
	width  = glutGet(GLUT_WINDOW_WIDTH),
	height = glutGet(GLUT_WINDOW_HEIGHT);

    /**
	set view matrices so world coordinates directly window coordinates 
     **/
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0,width,0,height);    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPixelStorei(GL_UNPACK_ALIGNMENT,Framebuffer::ALIGNMENT);
    glPixelStorei(GL_PACK_ALIGNMENT,Framebuffer::ALIGNMENT);

    width_ = width;
    height_ = height;
    glPointSize(1.0);
    if (mode_ == DrawFastest)
	{
	glDrawBuffer(GL_BACK);	
	glBegin(GL_POINTS);	
	}
    else
	{       
	glutSwapBuffers();
	glDrawBuffer(GL_FRONT);
	}
    }


/**
\brief 'unlock' unlocks access to the framebuffer for this window.  Between calls to
'lock' and 'unlock' no OpenGL calls can be made.
*/
void Framebuffer::unlock()
    {
    if (mode_ == DrawFastest)
	{
	glEnd();
	}
    else
	{
	glutSwapBuffers();
	glDrawBuffer(GL_BACK);
	}

    /* \todo return to previous glViewport settings */

    /* return to previous view matrices */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    }