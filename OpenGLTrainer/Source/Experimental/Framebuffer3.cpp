#include <OpenGLTrainer/Framebuffer3.h>
#include <iostream>

using namespace ITCS4120::OpenGLTrainer::Take3;

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
    image = NULL; 
    }
Framebuffer::Framebuffer()
    {
    pixelbuffer = NULL;
    }

/**
\brief 'lock' allows access to the framebuffer for the current GLUT window.  Between calls to
'lock' and 'unlock' no OpenGL calls should be made.
*/
void Framebuffer::lock()
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
 
    pixelbuffer->resize(width,height);
    glPointSize(1.0);
    glPixelStorei(GL_PACK_ALIGNMENT,Framebuffer::ALIGNMENT);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixelbuffer->image);
    }

/**
\brief 'unlock' unlocks access to the framebuffer for this window.  Between calls to
'lock' and 'unlock' no OpenGL calls can be made.
*/
void Framebuffer::unlock()
    {
    glPixelStorei(GL_UNPACK_ALIGNMENT,Framebuffer::ALIGNMENT);
    glRasterPos2i(0,0);
    glDrawPixels(pixelbuffer->width,pixelbuffer->height,GL_RGB,GL_UNSIGNED_BYTE,pixelbuffer->image);
    glFlush();

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
void Framebuffer::flush()
    {   
    glPixelStorei(GL_UNPACK_ALIGNMENT,Framebuffer::ALIGNMENT);
    glRasterPos2i(0,0);
    glDrawBuffer(GL_FRONT_AND_BACK);
    glDrawPixels(pixelbuffer->width,pixelbuffer->height,GL_RGB,GL_UNSIGNED_BYTE,pixelbuffer->image);
    glDrawBuffer(GL_BACK);
    }

void Framebuffer::Pixelbuffer::resize(int w, int h)
    {
    if (w != width || h != height)
	{
	delete[] image;
	image = new Pixel[w*h];
	width = w; height = h;
	}
    }

