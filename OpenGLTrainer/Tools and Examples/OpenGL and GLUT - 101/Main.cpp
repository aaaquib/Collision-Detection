/**
Copyright 2007, All Rights Reserved: Zachary Wartell, University of North Carolina at Charlotte
\file Main.cpp
\author Zachary Wartell
\brief Main.cpp is a example of GLUT and OpenGL plus a few ITCS4120::OpenGLTrainer
calls to help setup for simply 2D drawing.

TO DO LIST:
\todo

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] Donald Hearn and M. Pauline Baker.  Computer Graphics with OpenGL: Third Edition.
*/

/*******************************************************************************
    Includes	
*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <iostream>

#if 0
#include <GL/gl.h>
#else
// replacement for GL/gl.h incase an advanced student wants to use OGL extensions, OGL 2.1, etc.
#include <GL/glew.h>   
#endif

#include <GL/glut.h>

#include <OpenGLTrainer/OpenGLTrainer.h>
#include <OpenGLTrainer/Framebuffer.h>

using namespace std;

/*******************************************************************************
    File Scope (static) Globals
*******************************************************************************/
//NONE

/*******************************************************************************
    File Scope (static) Functions - To be replaced by Student
*******************************************************************************/
/**
\brief All code in ZJW namespace should be completely replaced by the student to implement
their assignment.
*/
namespace ZJW
{
/** a point that we move with the mouse */
static struct Point
    {
    int x;
    int y;
    } _point={0,0};

// \brief draw some random stuff
static void draw_frame()
    {
    const int WIDTH = glutGet(GLUT_WINDOW_WIDTH);
    const int HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);
    const int CENTER_X = WIDTH/2;
    const int CENTER_Y = HEIGHT/2;

    const int SIZE=50;
    glColor3ub(255,0,0);
    glBegin(GL_LINES);
	glVertex2i(CENTER_X-SIZE,CENTER_Y+SIZE);
	glVertex2i(CENTER_X+SIZE,CENTER_Y+SIZE);

	glVertex2i(CENTER_X-SIZE,CENTER_Y-SIZE);
	glVertex2i(CENTER_X+SIZE,CENTER_Y-SIZE);
    glEnd();

    glPointSize(10.0f);
    glBegin(GL_POINTS);
	glColor3ub(255,0,255);
	glVertex2i(_point.x,_point.y);
    glEnd();
    }

/**
\brief Print message for debugging to show us when GLUT is calling the idle callback function
*/
static void sayHello(void)
    {
    cout << "In Idle Function" << endl;
    }



/**
\brief Print message for showing mouse position
*/
static void respondMouseMotion(int x, int y)
    {
    cout << "Mouse: " << x << " " << y << endl;

    _point.x = x;
    _point.y = y;
    glutPostRedisplay();
    }
}

/*******************************************************************************
    File Scope (static) Functions
*******************************************************************************/

/**
\brief initialize some default OGL settings
*/
static
void initOpenGL (void)
    {
    //set clear color to white
    glClearColor(1.0f,1.0f,1.0f,1.0f);    
    }

/**
\brief 'motionFunc' is the GLUT motionFunc callback function
*/
static
void motionFunc 
	(
	int gx,  // 'glutX' - mouse X in GLUT coordinates
	int gy   // 'glutY' - mouse Y in GLUT coordinates
	)
    {
    /* convert from GLUT coordinate system to the 2D drawing coordinate system
       established by ITCS4120::OpenGLTrainer::matchWorldCoordinatesToPixelCoordinates 
     */
    const int WIDTH = glutGet(GLUT_WINDOW_WIDTH);
    const int HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);

    int sx,sy; // 'screenX', 'screenY'  mouse coordinate in our 2D screen coordinates
    sx = gx;
    sy = HEIGHT-gy;

    /* do something with mouse motion */
    ZJW::respondMouseMotion(sx,sy);
    }

/**
\brief 'display' is the GLUT display callback function
*/
static
void display(void)
    {      
    /**
	perform any 'one time' setup OGL calls.

	Note, OGL calls are only legal during execution of a GLUT display callback.  Below 
	is a standard method to make any OpenGL calls that we wish to make only the first
	time the GLUT display callback is executed.
     **/
    static boolean doOnce=true;
	if (doOnce)
	{
		initOpenGL();
		doOnce = false;
	}

    /**
	setup frame
     **/   
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    /**
	draw frame
     **/
    ZJW::draw_frame();
    
 
    /**
	end frame
     **/
    ITCS4120::OpenGLTrainer::showFrameRate();
    assert(glGetError()==GL_NO_ERROR);
    glutSwapBuffers();
    }

/**
\brief 'idle' is the GLUT idle callback function
*/
static
void idle(void)
    {
    /** update any animated objects' state */

    // ... no real code here yet ...
    ZJW::sayHello();

    /** trigger display callback to redraw scene based on object's updated state */
    glutPostRedisplay();
    }

/**
\brief 'main' is the standard C/C++ main function where execution starts
*/
int main (int argc, char** argv)
    {   
    
    /* Initialize GLUT library */
    glutInit(&argc, argv);

    /* create window */    
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutCreateWindow("OpenGL and GLUT - 101");

    /** init ITCS4120::OpenGLTrainer classes */
    ITCS4120::OpenGLTrainer::Framebuffer::init();
    
    /* set glut callbacks */
    glutDisplayFunc (display);
    glutIdleFunc (idle);
    glutMotionFunc (motionFunc);     
    glutReshapeFunc (ITCS4120::OpenGLTrainer::matchWorldCoordinatesToPixelCoordinates);

    /* start GLUT main loop */
    glutMainLoop();
    return 0;
    }