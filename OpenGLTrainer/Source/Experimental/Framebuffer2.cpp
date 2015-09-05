#include <OpenGLTrainer/Framebuffer2.h>
#include <iostream>

using namespace ITCS4120::OpenGLTrainer::Take2;

/*******************************************************************************
    File Scope Data Types
*******************************************************************************/
// NONE
/*******************************************************************************
    File Scope Functions
*******************************************************************************/
//
/*******************************************************************************
    File Scope (static/private) globals
*******************************************************************************/
Framebuffer::WindowIDToPixelbuffer Framebuffer::windowIDToPixelbuffer;

/*******************************************************************************
    File Scope Macros 
*******************************************************************************/
#define FB_SWAPBUFFER_APPROACH
/*******************************************************************************
    Exported (extern) Globals
*******************************************************************************/
// NONE

/*******************************************************************************
    Exported (extern) Functions
*******************************************************************************/
Pixel::Pixel(GLubyte r, GLubyte g, GLubyte b)
    {
    rgb[0]=r;rgb[1]=g;rgb[2]=b;
    }

Framebuffer::Pixelbuffer::Pixelbuffer()
    {
    width = height = 0;
    image = NULL; rows = NULL;
    }
Framebuffer::Framebuffer()
    {
    pixelbuffer = new Pixelbuffer;
    }

/**
\brief 'lock' allows access to the framebuffer for the current GLUT window.  Between calls to
'lock' and 'unlock' no OpenGL calls should be made.
*/
void Framebuffer::lock(Mode m)
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
    
    WindowIDToPixelbuffer::iterator i;
    i = windowIDToPixelbuffer.find(windowID);
    if (i == windowIDToPixelbuffer.end())
	{
	pixelbuffer = new Pixelbuffer;
	windowIDToPixelbuffer[windowID] = pixelbuffer;
	}
    else
	pixelbuffer = (*i).second;

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
 
    pixelbuffer->mode = m;
    pixelbuffer->resize(width,height);
    glPointSize(1.0);
    if (pixelbuffer->mode == DrawFastest)
	{
	glDrawBuffer(GL_BACK);	
	glBegin(GL_POINTS);	
	}
    else
	{
       #ifdef FB_SWAPBUFFER_APPROACH
	glutSwapBuffers();
	glDrawBuffer(GL_FRONT);
       #else
	glDrawBuffer(GL_FRONT_AND_BACK);
       #endif
	}

    }

/**
\brief 'unlock' unlocks access to the framebuffer for this window.  Between calls to
'lock' and 'unlock' no OpenGL calls can be made.
*/
void Framebuffer::unlock()
    {
    if (pixelbuffer->mode == DrawFastest)
	{
	glEnd();
	}
    else
	{
       #ifdef FB_SWAPBUFFER_APPROACH
	glutSwapBuffers();
       #endif	
	glDrawBuffer(GL_BACK);
	}

    /* \todo return to previous glViewport settings */

    /* return to previous view matrices */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    }

/**
\brief 'mode' sets the mode for framebuffer writes
*/
void Framebuffer::mode(Mode m)
    {   
    if (m != pixelbuffer->mode)
	{
	if (m == DrawFastest)
	    {// switching into DrawFastest, so just switch to drawing to BACK buffer
	    pixelbuffer->mode = DrawFastest;
           #ifdef FB_SWAPBUFFER_APPROACH
	    glutSwapBuffers();
           #endif
	    glDrawBuffer(GL_BACK);
	    glBegin(GL_POINTS);	    
	    }
	else 
	    {// switching into DrawImmediately, so update framebuffer with cached framebuffer
	     // and switch to drawing to both BACK and FRONT buffers.
	    pixelbuffer->mode = DrawImmediately;
	    glEnd();
           #ifdef FB_SWAPBUFFER_APPROACH
	    glutSwapBuffers();
	    glDrawBuffer(GL_FRONT);
           #else
	    glDrawBuffer(GL_FRONT_AND_BACK);
           #endif	    
	    }
	}
    }
void Framebuffer::Pixelbuffer::resize(int w, int h)
    {
    if (w != width || h != height)
	{
	delete[] image;
	image = new Component[w*h*3];
	width = w; height = h;
	for (int y=h-1;y>=0;y--)
	    for (int x=w-1;x>=0;x--)
		for (int c = 2; c>=0; c--)
		    {
		    image [y*w*3 + x*3 + c].pixelbuffer = this;
		    image [y*w*3 + x*3 + c].loc[0] = x;
		    image [y*w*3 + x*3 + c].loc[1] = y;
		    image [y*w*3 + x*3 + c].color = (Component::Color)c;
		    }		
	}
    }