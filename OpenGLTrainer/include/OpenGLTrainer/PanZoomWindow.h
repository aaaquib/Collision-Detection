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
#ifndef PANZOOM_WINDOW_H
#define PANZOOM_WINDOW_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <OpenGLTrainer/OpenGLTrainer.h>

namespace ITCS4120
{
namespace OpenGLTrainer
{

/**
\brief PanZoomWindow is a GLUT window for 2D rendering that can be panned and zoomed.

\section PanZoomWindow_USAGE Usage

See "Tools and Examples\PanZoomWindow\main.cpp" for example usage.

\section PanZoomWindow_USER_INTERFACE User Interface

- Left-click + CTRL : mouse movements pans the contents of the pan-zoom window.  
- Left-click + CTRL + SHIFT: vertical mouse movement zooms in and out.  
- mouse wheel : zooms in and out \n
 (mouse wheel support available only with compatible GLUT libraries)
*/
class OPENGLTRAINER_CLASS PanZoomWindow : public Window
    {
    public:
    PanZoomWindow (const float viewLowerLeft[2], const float viewUpperRight[2]);
    //PanZoomWindow ();

    private:
    enum {
	/* default window width */
	DEFAULT_WIDTH=500,
	/* default window height */
	DEFAULT_HEIGHT=500,
	/* byte alignment for OGL pixel transfer */
	ALIGNMENT=1};
    

    public:
    /**
    \brief ViewWindow represents a 2D rectangular view window within world coordinates 
    that is mapped to this PanZoomWindow's GLUT window.
    The view window is axis-aligned in world coordinates.
    */
    struct ViewWindow
	{
	/** lower left corner of view window in world coordinates */
	float lowerLeft [2];
	/** upper right corner of view window in world coordinates */
	float upperRight [2];
	};    
    private:
    ViewWindow viewWindow_;
    public:
    /**
    \brief Read accessor for 'viewWindow_'
    */
    const ViewWindow& viewWindow()const{return viewWindow_;}


    void mouseCoordinatesToWorldCoordinatesPoint(const int mouse[2], float world[2]);
    void mouseCoordinatesToWorldCoordinatesVector(const int mouse[2], float world[2]);
    void setOpenGLView();


    private:
    /* center of mouse zoom (in glut mouse coordinates) */
    int zoomCenter [2];

    /** 'last' last mouse position recorded by latest call to motion callback */
    int last[2];

    /** max view window width allowed */
    float maxViewWindowWidth;

    /** 'drag' indicates if UI is in drag mode */
    bool drag;
    /** 'zoom' indiciates if UI is in zoom mode */
    bool zoom;

    void zoomView(float scale);

    protected:
    /** 
     **  glut display callback member functions (details in .cpp file)
     **/
    //virtual void display(void);
    virtual void motion (int x, int y);
    virtual void passiveMotion (int x, int y);
    virtual void mouse (int button, int state, int x, int y);
    virtual void reshape (int w, int h);
    virtual void keyboard(unsigned char key, int x, int y);
    };



};
};
#endif