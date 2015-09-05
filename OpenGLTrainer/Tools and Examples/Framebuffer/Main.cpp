/**
Copyright 2007, All Rights Reserved: Zachary Wartell, University of North Carolina at Charlotte
\file Main.cpp
\author Zachary Wartell
\brief Main.cpp is a example of using the ITCS4120::OpenGLTrainer::framebuffer class for
implementing rasterization algorithms.

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

//#include <GL/gl.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <OpenGLTrainer/OpenGLTrainer.h>
#include <OpenGLTrainer/Framebuffer.h>
#include <FreeImagePlus.h>

using namespace std;

/*******************************************************************************
    File Scope (static) Globals
*******************************************************************************/
class Entity
    {
    public:
    Entity();

    /* dynamic array of GLubyte[3] arrays */
    GLubyte (*image)[3];
    int width;
    int height;
    };
Entity* entity;

/*******************************************************************************
    File Scope (static) Functions
*******************************************************************************/

Entity::Entity()
    {
    fipImage i;

    /* load image */
    i.load(".\\unidentified.png",0);
    width = i.getWidth();
    height = i.getHeight();

    /* convert fipImage storage format to GL_RGB format using in Entity.image */
    image = reinterpret_cast<GLubyte(*)[3]>(new GLubyte [i.getWidth()*i.getHeight()*3]);
    for (int x=i.getWidth()-1;x>=0;x--)
	for (int y=i.getWidth()-1;y>=0;y--)
	    {
	    RGBQUAD value;
	    i.getPixelColor(x,y,&value);
	    image [y*width + x][0]=value.rgbRed;
	    image [y*width + x][1]=value.rgbGreen;
	    image [y*width + x][2]=value.rgbBlue;
	    }
    }


static
void initApp(void)
    {
    entity = new Entity;
    }

/**
\brief initialize some default OGL settings
*/
static
void initOpenGL (void)
    {
    glClearColor(0.0f,0.0f,0.0f,1.0f);    
    }

/**
\brief 'motionFunc ' is the GLUT motionFunc callback function
*/
static
void motionFunc (int x, int y)
    {
    }

/**
\brief 'display' is the GLUT display callback function
*/
static
void display(void)
    {  
    using namespace ITCS4120::OpenGLTrainer;

    Framebuffer* framebuffer;

    /**
	setup frame
     **/   
    glClear(GL_COLOR_BUFFER_BIT);

 
    /**
	lock framebuffer to allow drawing individual pixels to framebuffer
       (While locked, no other OGL commands can be used!)
     **/
    if (!Framebuffer::lock(framebuffer))
	goto BadFrameBuffer;

    /**
	draw some stuff with setRow
     **/
    for (int y=entity->height-1;y>=0;y--)
	framebuffer->setRow(100,y+100,entity->width,entity->image + entity->width*y);

    int x0;
    for (x0=100;x0 < framebuffer->width()-entity->width && entity->height-1+x0 < framebuffer->height()-entity->height;x0+=50)
	for (int y=entity->height-1;y>=0;y--)
	    framebuffer->setRow(x0,y+x0,entity->width,entity->image + entity->width*y);
    /**
	draw some horizontal lines 
     **/
    //- BKPT 1
    for (int y=0;y<framebuffer->height();y+=20)
	for (int x=0;x<framebuffer->width();x++)
	    framebuffer->setPixel(x,y,255,x%256,(x+10)%256);

    /**
	draw some vertical lines 
     **/
    //- BKPT 2
    for (int x=0;x<framebuffer->width();x += 20)
	for (int y=0;y<framebuffer->height();y++)
	    framebuffer->setPixel(x,y,255,y%256,(y+10)%256);

    /**
     unlock framebuffer to allow regular OGL calls
     **/   
    Framebuffer::unlock(framebuffer);    

    BadFrameBuffer:

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
    // ...nothing here yet....

    /** request redisplay to reflect change to object state */
    glutPostRedisplay();
    }

/**
\brief 'main' is the standard C/C++ main function where execution starts
*/
int main (int argc, char** argv)
    {
    //Test::Framebuffer::test();
    
    /* Initialize GLUT library */
    glutInit(&argc, argv);

    /* create window */    
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
    glutCreateWindow("Rasterization I");

    ITCS4120::OpenGLTrainer::Framebuffer::init();
 
    /* initOpenGL some default OGL stuff */
    initApp();    
    
    /* set glut callbacks */
    glutDisplayFunc (display);
    glutIdleFunc (idle);
    glutMotionFunc (motionFunc);     
    glutReshapeFunc (ITCS4120::OpenGLTrainer::matchWorldCoordinatesToPixelCoordinates);

    /* start GLUT main loop */
    glutMainLoop();
    return 0;
    }