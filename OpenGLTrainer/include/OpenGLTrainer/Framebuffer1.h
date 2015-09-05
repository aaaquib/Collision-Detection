/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.
\brief Framebuffer.h defines the Framebuffer class.

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
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <OpenGLTrainer/OpenGLTrainer.h>

namespace ITCS4120
{
namespace OpenGLTrainer
{

//#define USE_TAKE1_BY_DEFAULT
#ifndef USE_TAKE1_BY_DEFAULT
namespace Take1
{
#endif


/**
\brief Framebuffer provides an simplified API for directly reading and writing pixels to 
an OpenGL windows framebuffer.  Pixels are accessed as RGB GLubyte triples using a syntax
like 'framebuffer [y][x].r(10)' to set the red component of pixel x,y and 'framebuffer [y][x].r()'
to read the pixel.  The Framebuffer class is for pedogogical purposes to simplify early
computer graphics assignments when implementing rasterization algorithms.

To aid debugging rasterization algorithms, Framebuffer.mode can be set to DrawImmediately which
forces every single pixel update to immediately appear on the screen.  This
mode executes very slowly, but allows you to use the debugger to step through a rasterization
algorithm and see the output change one pixel at a time.

When not debugging rasterization algorithms use the DrawFastest Mode instead.  
Here individual pixel updates may not appear on the screen until the OpenGL pipeline's 
buffers are flushed but your rasterization algorithms will execute must faster 
than in DrawImmediately Mode.

API USAGE:

From within a GLUT display callback

void display (void)
    {
    Framebuffer* framebuffer;
    ... do opengl screen setup stuff ...
    
    //
    //    Lock Framebuffer
    // 
    // !! regular OpenGL calls cannot be called between .lock and .unlock !!
    Framebuffer::lock(framebuffer);  
    
    ... do your rasterization algorithms .. here I just draw one pixel ...

    framebuffer->setPixel(x,y,red,green,blue);

    //
    //    Unlock Framebuffer
    //
    Framebuffer::unlock(framebuffer);

    ... do other OGL stuff ...
    }

*/
class Framebuffer
    {
    /**************************************************************************
         PUBLIC STUFF
     *************************************************************************/

    /**
     **  Data Types
     **/

    /** When using Framebuffer write pixel methods, Mode determines how immediately and quickly 
    changes to the framebuffer's pixels will appear on screen */
    enum Mode
	{
	/** In 'DrawImmediately' Mode will cause every single pixel change to the framebuffer to be 
	    immediately visible.   This mode is useful for debugging rasterization algorithms, but likely
	    will run significantly slower that 'DrawFastest' mode */
	DrawImmediately,
	/** In 'DrawFastest' Mode per-pixel changes will typically not appear until the
	    Framebuffer is unlocked.  'DrawFastest' Mode should be used by default when your 
	    program and while 'DrawImmediately' Mode is useful when debugging your rasterization 
	    algorithms */
	DrawFastest
	};

    /**
     **  Member Functions
     **/
    public:
    Framebuffer ();

    /**
    \brief Set pixel at location 'x','y', to RGB color value (red,green,blue)
    */
    void setPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue);

    /**
    \brief Set the row of pixels starting location 'x','y' to the 
    pixels in array 'rgb[][3]' which has 'nPixels' stored in it.

    \warning results are undefined if rgb[][3] extends past the end of the 
    framebuffer
    */
    void setRow(int x, int y, int nPixels, GLubyte rgb[][3]);

    /**
    \brief Set pixel at location 'locaiton' to RGB color value 'rgb'
    */
    void setPixel(const int location[2], const GLubyte rgb[3]);

    /**
    \brief Read RGB value of pixel 'x','y' into 'rgb'
    */
    void getPixel(int x, int y, GLubyte rgb[3]);

    /** get framebuffer width */
    inline int width ()  const { return width_;}
    /** get framebuffer height */
    inline int height () const { return height_;}


    /** get pixel writing mode */
    Mode mode() const {return mode_;}
    /** set pixel writing mode */
    void mode(Mode m);

    /*
    \brief Set mode to DrawImmediately -- equivalent to Framebuffer.mode(DrawImmediately) 
    */
    void drawImmediately();
    /*
    \brief Set mode to DrawFastest -- equivalent to Framebuffer.mode(DrawFastest) 
    */
    void drawFastest();

    /**
     **  Static Member Functions
     **/

    /**
    \brief 'lock' allows access to the framebuffer for the current GLUT window.  lock' returns
    a pointer 'framebuffer' that points to a Framebuffer object which can access the framebuffer 
    of the current GLUT window.

    Between calls to 'lock' and 'unlock' no direct OpenGL calls should be made.
    */
    static void lock(Framebuffer* & framebuffer);

    /**
    \brief 'unlock' unlocks the framebuffer 'framebuffer' associated with the current 
    GLUT window.  
    */
    static void unlock(Framebuffer* framebuffer);


    /**************************************************************************
         PRIVATE STUFF
     *************************************************************************/
    private:

    int width_;
    int height_;
    Mode mode_;	

    /** int constants */
    enum {ALIGNMENT=1};

    /**
    \brief 'lock' allows access to the framebuffer for the current GLUT window.  Between calls to
    'lock' and 'unlock' no OpenGL calls should be made.
    */
    void lock();

    /**
    \brief 'unlock' unlocks access to the framebuffer for this window.  Between calls to
    'lock' and 'unlock' no OpenGL calls can be made.
    */
    void unlock();       

    /**
     **  static member functions
     **/
    private:    
    typedef std::map<int,Framebuffer*> WindowIDToFramebuffer;
    static WindowIDToFramebuffer windowIDToFramebuffer;
#if 0
    class PixelStream
	{
	public:
	PixelStream();	    
	inline void begin() { glBegin(GL_POINTS);open_=true;}
	inline void end() { if (open_) {glEnd(); open_ = false; }
	inline void draw(int x, int y, GLubyte r, GLubyte g, GLubyte b)
	    {
	    glColor3ub(r,g,b);
	    glVertex2i(x,y);
	    }
	private:
	bool open_;
	};
    PixelStream pixelStream;
#endif
    };

#ifndef USE_TAKE1_BY_DEFAULT
}
#endif
}};