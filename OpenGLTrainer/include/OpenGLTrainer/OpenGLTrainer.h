/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief OpenGLTrainer.h defines a variety of functions for early ITCS4120 projects.
*/
#ifndef OPENGL_TRAINER_H
#define OPENGL_TRAINER_H

/*******************************************************************************
    INCLUDES
*******************************************************************************/

#include <map>
#include <GL/glut.h>
#include <OpenGLTrainer/OpenGLTrainer_exports.h>

/*******************************************************************************
    MACROS
*******************************************************************************/

/**
\brief Similar to the standard C assert macro, this macro evaluates expression 
'expression' and exit program if it fails to be true.
(Unlike standard C assert this macro evaluates and tests the expression even with
_NDEBUG defined).
*/
#define assert_always(expression)\
    do {\
    if (!(expression)) {\
	std::cout << "FAILED: " << #expression << std::endl <<\
	     "   File: " << __FILE__ << "  Line: " << __LINE__ << std::endl;\
	exit(1);\
	}\
    } while (0)

/**
\brief Similar to the standard C assert macro, this macro evaluates expression 'expression' and exit program if it fails to be true.
(Unlike standard C assert this macro evaluates and tests the expression even with
_NDEBUG defined).  The message 'char_string' is also printed in the error message
*/
#define assert_always2(expression,char_string)\
    do {\
    if (!(expression)) {\
	std::cout << "FAILED: " << #expression << std::endl <<\
	     "   File: " << __FILE__ << "  Line: " << __LINE__ << std::endl;\
	std::cout << "   " << char_string << std::endl;\
	exit(1);\
	}\
    } while (0)

/**
\brief Similar to the standard C assert macro, this macro evaluates expression 'expression' and exit program if it fails to be true.
Also display any OpenGL error message.  (Unlike standard C assert this macro evaluates 
and tests the expression even with _NDEBUG defined).
*/
#define assert_always_OGL(expression)\
    do {\
    if (!(expression)) {\
	std::cout << "FAILED: " << #expression << std::endl <<\
	     "   File: " << __FILE__ << "  Line: " << __LINE__ << std::endl;\
	std::cout << "glGetError: " << gluErrorString(glGetError()) << std::endl;\
	abort();\
	}\
    } while (0)

/*******************************************************************************
    DATA TYPES
*******************************************************************************/
/**
\brief ITCS4120 includes all declarations in OpenGLTrainer
*/
namespace ITCS4120
{

/**
\brief OpenGLTrainer includes all declarations in OpenGLTrainer
*/
namespace OpenGLTrainer
{

/*
\brief 'ExperimentControl' enables/disables calling of various GL functions
for purposes of experimenting and learning their effects.

This is used for certain student exercises.
*/
struct OPENGLTRAINER_CLASS ExperimentControl
    {
    /** enable/disable message when calling display callback */
    bool traceDisplay;
    /** enable/disable message when calling idle callback */
    bool traceIdle;

    /** enable/disable call to post redisplay in idle callback */
    bool idlePostRedisplay;

    /** enable/disable skiping all drawing in display callback */
    bool enableDisplay;

    /** enable/disable glFlush at end of display callback */
    bool flush;

    /** enable/disable glutSwapBuffer at end of display callback */
    bool swapBuffer;

    /** enable/disable clearing framebuffer at start of display callback */
    bool clear;

    /** enable/disable call to post redisplay in motion callback */
    bool motionPostRedisplay;


    static void initMenus(unsigned int mode);

    private:
    enum {TRACE_DISPLAY,TRACE_IDLE,SKIP_DISPLAY,IDLE_POST_REDISPLAY,MOTION_POST_REDISPLAY,FLUSH,SWAP_BUFFER,CLEAR};    
    static void menuCallback(int value);
    };

/**
\brief 'Window' partially wraps the GLUT C-style API into a C++ OOP API.  Mainly, it uses virtual member functions of a Window class
rather than the standard C-style GLUT callback functions.  

Window is a minimalistic C++ wrapper around GLUT's C window and event handling API.  For each of the standard GLUT callback functions 
there is a corresponding Window virtual member function which you would override in a sub-class of Window.  See \ref Window_Usage.

If we/you need anything fancier you should just grab someoneelse's complete C++ GLUT wrapper.  
*/
class OPENGLTRAINER_CLASS Window
    {
    public:
    /**
    CallbackMethod determines whether this Window's GLUT callback virtual member functions
    are automatically registered as GLUT callbacks or whether the virtual member functions
    must be manually executed by the programmer from within his own GLUT callback functions.

    By default, a Window will automatically register itself and you should just use rely on 
    this default behavior.
    */
    enum CallbackMethod{
	AUTO_CALLBACKS=0,	//!< automatically call the virtual member functions 
	MANUAL_CALLBACKS=1,     //!< require programmer to manually handle getting GLUT callbacks to call the virtual member functions
	};

    Window();
    Window(CallbackMethod cm);

    /** DisplayFunc is pointer to a function matching the GLUT display callback prototype */
    typedef void (*DisplayFunc)(void);

    public:  
    private:
    CallbackMethod callbackMethod;
    public:

    /** \brief width of window.
    This is automatically set to the GLUT windows height each time the window is reshaped.
    Note, this is done after calling Window::reshape so that derived classes version of
    reshape has access to the previous value of height.
    */
    int width;
    /** \brief height of window.
    This is automatically set to the GLUT windows height each time the window is reshaped.
    Note, this is done after calling Window::reshape so that derived classes version of
    reshape has access to the previous value of height.
    */
    int height; 

    
    /** GLUT window ID */
    protected: int windowID_;
    /** return GLUT window ID */
    public: int windowID(){return windowID_;}

    public:
    void glutCreateWindow(const char* name);

    protected:
    /** 
     **  glut display callback member functions.
     **
     **  Assuming the CallbackMethod is AUTO_CALLBACKS, this will be automatically called
     **  whenever the corresponding GLUT callback would be called for this Window's GLUT window.
     **/
    virtual void display(void);
    virtual void motion (int x, int y);
    virtual void passiveMotion (int x, int y);
    virtual void keyboard (unsigned char key, int x, int y);
    virtual void mouse (int button, int state, int x, int y);
    virtual void reshape (int w, int h);

    /** 
     ** (static) glut display callback functions. These are used as a bridge between 
     ** Window virtual member functions and GLUT callback functions.
     **/
    private:    
    static void static_display(void);    
    static void static_motion(int x, int y);
    static void static_reshape(int w, int h);
    static void static_passiveMotion(int x, int y);
    static void static_mouse(int button, int state, int x, int y);
    static void static_keyboard (unsigned char key,int x, int y);

	private:
	typedef std::map<int,Window*> IDtoWindow_t;
#pragma warning( push )
#pragma warning( disable : 4251 )
	/** this maps GLUT window ID's to Window instances */
	static IDtoWindow_t IDtoWindow_;		
#pragma warning( pop )

	public:
	static Window* IDtoWindow (int i) { return IDtoWindow_[i]; }
    };


/******************************************************************************
    EXPORTED FUNCTIONS
******************************************************************************/
/* */
OPENGLTRAINER_EXPORT void matchWorldCoordinatesToPixelCoordinates(int width, int height);
OPENGLTRAINER_EXPORT void showFrameRate();

/******************************************************************************
    EXPORTED GLOBALS
******************************************************************************/

/* exported (extern) instance of ExperimentControl */
extern ExperimentControl experimentControl;



/**
Unused code
**/
#if 0
/**
\brief Pixel is a single RGB pixel (GLUBYTE) in a Framebuffer instance

Footnotes:
- F1 - Must use array instead of individual data members to guarantee compiler generates
rgb bytes at consecutive addresses.  (data members can be padded)
*/
class Pixel
    {    
    public:
	/**
	**  read accessors for RGB values of this pixel
	**/
	GLubyte r() const {return rgb[0];}
	GLubyte g() const {return rgb[1];}
	GLubyte b() const {return rgb[2];}
	/**
	**  write accessors for RGB values of this pixel
	**/
	void r(GLubyte r) {rgb[0]=r;}
	void g(GLubyte g) {rgb[1]=g;}
	void b(GLubyte b) {rgb[2]=b;}
    private:
	/** see Footnote F1 */
	GLubyte rgb[3];
    };
#endif


};
};
#endif