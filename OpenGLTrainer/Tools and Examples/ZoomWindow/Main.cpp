/**
Copyright 2007, All Rights Reserved: Zachary Wartell, University of North Carolina at Charlotte
\file Main.cpp
\author Zachary Wartell
\brief OpenGL ZoomWindow creates a window that displays a pannable and zoomable image of the
framebuffer captured from the display screen.  See \ref ZoomWindow_USER_INTERFACE.

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
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <OpenGLTrainer/OpenGLTrainer.h>
#include <OpenGLTrainer/ZoomWindow.h>

#include <iostream>

using namespace std;

/*******************************************************************************
    File Scope (static) Globals
*******************************************************************************/
// NONE

/*******************************************************************************
    File Scope (static) Functions
*******************************************************************************/

/**
\brief 'main' is the standard C/C++ main function where execution starts
*/
int main (int argc, char** argv)
    {    
    /* Initialize GLUT library */
    glutInit(&argc, argv);
    
   #if 0
    ITCS4120::OpenGLTrainer::ZoomWindow::create();  
   #else
    // test partially implemented fullscreen ZoomWindow
    ITCS4120::OpenGLTrainer::ZoomWindow::create(1024,768);
   #endif   

    /* start GLUT main loop */
    glutMainLoop();
    return 0;
    }