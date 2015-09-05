/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief Framebuffer.h defines the Framebuffer class.

TO DO LIST:
\todo 

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] Technical Brief: Fast Texture Downloads and Readbacks using Pixel Buffer
       Objects in OpenGL.  August 2005.

\internal
*/
#ifndef FRAMEBUFFER4_H
#define FRAMEBUFFER4_H


/*******************************************************************************
    INCLUDES
*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <OpenGLTrainer/OpenGLTrainer.h>


/*******************************************************************************
    MACROS
*******************************************************************************/
/*
\brief 'OGT_FB_USE_PIXEL_BUFFER_OBJECTS' - #define'ing OGT_FB_USE_PIXEL_BUFFER_OBJECTS
enables faster implementation of the Framebuffer class.  This implementation
requires OpenGL 2.1 support on the graphics card.  If OGT_FB_USE_PIXEL_BUFFER_OBJECTS
is not #define'd, then a less efficient implementaiton of the Framebuffer class
is compiled that is compatible with earlier versions of OpenGL.

Framebuffer::init will abort if code was compiled with incompatible settings.
Then you would comment out this #define and recompile.
*/
//#define OGT_FB_USE_PIXEL_BUFFER_OBJECTS

/*
\brief If 'FBT4_DO_BOUNDS_CHECKING' is define'd all Framebuffer member functions 
that access the framebuffer's pixels will check that the specified target pixel 
coordinates are inside the bounds of the framebuffer.  

Note, 'FBT4_DO_BOUNDS_CHECKING' should generally be left undefine'd when during optimized 
compiling; hence under non-debug compilation it is disabled by default.
*/
#ifndef NDEBUG
#define FBT4_DO_BOUNDS_CHECKING
#endif

/*******************************************************************************
    DATA TYPES
*******************************************************************************/
namespace ITCS4120
{
namespace OpenGLTrainer
{

/**
\brief [INTERNAL] 'USE_TAKE4_BY_DEFAULT' -- when define'd this causes this version of the 
Framebuffer class to be compiled as ITCS4120::OpenGLTrainer::Framebuffer instead
of ITCS4120::OpenGLTrainer::Take4::Framebuffer.

For this this should be define as the other versions of this class are 
experimental or deprecated.
*/
#define USE_TAKE4_BY_DEFAULT
#ifndef USE_TAKE4_BY_DEFAULT
namespace Take4
{
#endif

/**
\brief Framebuffer provides an interface for reading and writing pixels to 
an OpenGL window's framebuffer.  The Framebuffer class is for pedogogical purposes and for
playing with rasterization algorithms that directly read and write pixels to framebuffer memory
while assuming the hardware framebuffer is organized as a simple 2D array.

To aid debugging rasterization algorithms, Framebuffer.mode can be set to DrawImmediately which
forces every single change to a pixel to immediately appear on the screen.  This
mode executes very slowly, but allows students to use the debugger to step through their
rasterization algorithm and see the algorithm's effect one pixel change at a time.

When not debugging, one should use DrawFastest Mode instead.  (This is enable by default).   
Here individual pixel updates may not appear on the screen until the OpenGL pipeline's buffers are flushed, 
etc. but rasterization algorithms will execute must faster than in DrawImmediately Mode.

Unless otherwised documented, Framebuffer methods implementing low level pixel operations expect 
the programmer to only access valid pixel coordinates.  In this respect, Framebuffer access is just
like accessing a 2D array. Only methods whose documentation states the method performs clipping 
will accept arbitrary pixel coordinates without generating errors.  

For the low level methods, the Debug compilation will perform some bounds checking and attempts to access 
invalid framebuffer pixels will cause an immediate fatal error. In the Release compilation, 
no bounds checking is done; hence accessing invalid pixels will create undefined results including 
memory errors.



API USAGE:

From within a GLUT display callback
\code
void display (void)
    {
    Framebuffer* framebuffer;

    ... do opengl screen setup stuff ...
    
    //
    //    Lock Framebuffer
    // 
    // !! regular OpenGL calls cannot be called between .lock and .unlock !!
    if(!Framebuffer::lock(framebuffer))
	goto BadFrameBuffer;
    
    ... do your rasterization algorithms .. here I just draw one pixel ...

    framebuffer->setPixel(x,y,red,green,blue);

    //
    //    Unlock Framebuffer
    //
    Framebuffer::unlock(framebuffer);

    BadFrameBuffer:

    ... do other OGL stuff ...
    }

\endcode

A complete example is found in the "OpenGLTrainer\Tools and Utilities\Framebuffer".

\b Advanced Options

Use of GL Pixel Buffer Objects can also be optionally compiled (see OGT_FB_USE_PIXEL_BUFFER_OBJECTS
in Framebuffer.h) for faster performance.

*/
class OPENGLTRAINER_CLASS Framebuffer
    {
    public:
    /**************************************************************************
         PUBLIC STUFF
     *************************************************************************/

    /*
     **  Data Types
     */

    /** 
    \brief Mode - When using Framebuffer write pixel methods, Mode determines how immediately and quickly 
    changes to the framebuffer's pixels will appear on screen 
    */
    enum Mode
	{
	/** In 'DrawImmediately' Mode causes every single pixel change to the framebuffer to be 
	    immediately visible.   This mode is useful for debugging rasterization algorithms, but likely
	    will run significantly slower that 'DrawFastest' mode */
	DrawImmediately,
	/** In 'DrawFastest' Mode per-pixel changes will typically not appear until the
	    Framebuffer is unlocked.  'DrawFastest' Mode should be used by default when your 
	    program and while 'DrawImmediately' Mode is useful when debugging your rasterization 
	    algorithms */
	DrawFastest
	};

    /*
     **  Member Functions (non-inline function comments are found in .cpp file)
     */
    public:
    Framebuffer ();
    void setPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue);
    void setRow(int x, int y, int nPixels, const GLubyte rgb[][3]);
    void setRow(int x, int y, int offset, int length, const GLubyte rgb[][3]);
    void setPixel(const int location[2], const GLubyte rgb[3]);    
    void clipAndSetRectangle(int x, int y, int width, int height, const GLubyte (*rgb)[3]);
    void getPixel(int x, int y, GLubyte rgb[3]);
    void drawImmediately();
    void drawFastest();

    /** 
    \brief get framebuffer width 
    */
    inline int width ()  const { return width_;}
    /** 
    \brief get framebuffer height 
    */
    inline int height () const { return height_;}
 
    void mode(Mode m);

    /** \brief get pixel writing mode */
    inline Mode mode() const {return mode_;}

    /** \brief get framebuffer lock status. */
    inline bool locked() const {return locked_;}

    /* 
     **  Static Member Functions
     */

    static bool lock(Framebuffer* & framebuffer);
    static void unlock(Framebuffer* framebuffer);
    static void init();

    /**************************************************************************
         PRIVATE STUFF
     *************************************************************************/
    private:

    /* 'width_' is the width of this Framebuffer */
    int width_;
    /* 'height_' height of this Framebuffer */
    int height_;
    /* 'mode_' is the current pixel writing Mode for this Framebuffer (see Mode) */
    Mode mode_;	

    /* 'locked_' is this Framebuffer locked */
    bool locked_;

    /* 'lockCount' is used for nested calls to lock */
    int lockCount;

    /* 'pixelBuffer' is the OGL handle to the created PBO */
    GLuint pixelBuffer;

    /* 'pixels' is glMapBuffer'ed to the Pixel Buffer Object */
    GLubyte* pixels;

    /** int constants */
   #ifdef OGT_FB_USE_PIXEL_BUFFER_OBJECTS    
    enum {ALIGNMENT=1,COMPONENTS=3,FORMAT=GL_RGB};   
    /* these settings below were me testing for optimization for Nvidia hardware (see [R1]) 
       since I'm not seeing significant performance improvement with PBOs :(
     */
    //enum {ALIGNMENT=1,COMPONENTS=3,FORMAT=GL_RGB};
    //enum {ALIGNMENT=1,COMPONENTS=3,FORMAT=GL_BGR};
   #else
    enum {ALIGNMENT=1,COMPONENTS=3,FORMAT=GL_RGB};    
   #endif


    bool lock();
    void unlock();       

    /*
     **  static member functions
     */
    private:    
    typedef std::map<int,Framebuffer*> WindowIDToFramebuffer_t;
#pragma warning( push )
#pragma warning( disable : 4251 )	
	static WindowIDToFramebuffer_t windowIDToFramebuffer_;
#pragma warning( pop )    
	public:
	static Framebuffer* windowIDToFramebuffer (int i) { return windowIDToFramebuffer_[i];}
    };

#ifndef USE_TAKE4_BY_DEFAULT
}
#endif
}};
#endif