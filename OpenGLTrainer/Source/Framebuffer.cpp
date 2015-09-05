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


IMPLEMENTATION DETAILS:

Recall, OpenGL was designed for advanced 3D graphics hardware and to allow the hardware designers
to implement all sorts of advanced hardware-based parallel rasterization algorithms
(tiled architectures, interleaved-scanlines, etc.).  In such advanced hardware systems the
framebuffer need not be organized as a simple 2D array in memory.  Hence, to allow such
advanced hardware design, OpenGL does not allow direct framebuffer access in which the graphics 
cards framebuffer memory is mapped into to a processes virtual address space.  
When teaching basic rasterization algorithms in introductory graphics courses this is a bit
limiting.  This class, ITCS4120::OpenGLTrainer::Framebuffer, presents an interface that appears
to directly access the framebuffer as a simple 2D array.

Pixel Buffer Objects:

If OGT_FB_USE_PIXEL_BUFFER_OBJECTS is #define'd, then Pixel Buffer Objects are used.

When in DrawFastest mode Framebuffer uses a OpenGL 2.1 Pixel Buffer Object.  The PBO is
initialized with a copy of the GL window framebuffer (FB).  Then PBO is mapped into system memory
space (Framebuffer.pixels).  Then all pixel read and write Framebuffer methods access 
this mapped memory.  When a Framebuffer is unlocked (or switched to DrawImmediately Mode), 
the PBO is glDrawPixel'ed back to windows framebuffer.   

If OGT_FB_USE_PIXEL_BUFFER_OBJECTS is not #define'd, then client memory is used instead.

ARRRGGGG!!!!  So for all my effort, even on a Nvidia 8800 the PBO implementation is not
running any faster than the client buffer approach.  Possible reasons:
    -my GL drivers are defaulting to using client memory instead of GPU memory for PBOs
    -the above is happening because I'm not using the most optimal settings for GL PBOs
    -the above is happening because I'm not using the most optimal settings for GL PBOs
     on my specific graphics card

I tested the case where all I did was call Framebuffer lock/unlock and I disable
vertical sync through the Control Panel.   Hence any overhead in actual calling 
the set pixel routines was removed and still I didn't see significant
performance differences.
*/

#include <OpenGLTrainer/Framebuffer.h>

#include <assert.h>
#include <iostream>
#include <algorithm>


#ifdef DOXYGEN
using namespace ITCS4120::OpenGLTrainer;
#endif

#ifdef USE_TAKE4_BY_DEFAULT
using namespace ITCS4120::OpenGLTrainer;
#else
using namespace ITCS4120::OpenGLTrainer::Take4;
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
Framebuffer::WindowIDToFramebuffer_t Framebuffer::windowIDToFramebuffer_;

/*******************************************************************************
    File Scope Macros 
*******************************************************************************/
/*
\brief Empirically glFlush isn't enough to force each new GL_POINT to be rendered
to the display so use glFinish instead.
*/
#define GL_FORCE_PIXEL_OUT() glFinish()
//#define GL_FORCE_PIXEL_OUT() glFlush()

/*
\brief These macros generate code for checking pixel coordinates bounds relative
to a given Framebuffer.  They are conditionally compiled based on macro
FBT4_DO_BOUNDS_CHECKING
*/
#ifdef FBT4_DO_BOUNDS_CHECKING
#define BDCK_PIXEL(fb,x,y)\
    if (x < 0 || x >= fb->width() || y < 0 || y >= fb->height())\
	{\
	std::cerr << "Framebuffer: Pixel Bound Error: File=" << __FILE__ << "Line=" << __LINE__ << std::endl;\
	abort();\
	}
	
#define BDCK_ROW(fb,x,y,length)\
    if (x < 0 || x+length > fb->width() || y < 0 || y >= fb->height())\
	{\
	std::cerr << "Framebuffer: Pixel Bound Error: File=" << __FILE__ << "Line=" << __LINE__ << std::endl;\
	abort();\
	}
#else
#define BDCK_PIXEL(fb,x,y)
#define BDCK_ROW(fb,x,y,length)
#endif

#ifdef OGT_FB_USE_PIXEL_BUFFER_OBJECTS
#define USE_PBOS
#endif

/******************************************************************************
    Exported (extern) Globals
*******************************************************************************/
// NONE

/*******************************************************************************
    Exported Functions
*******************************************************************************/

/**
\brief Construct a Framebuffer
*/
Framebuffer::Framebuffer()
    {
    width_ = height_ = 0;
    mode_ = DrawFastest;
    pixelBuffer = 0;
    pixels = NULL;
    locked_ = false;
    lockCount = 0;
    }

/**
\brief Set mode to DrawImmediately -- equivalent to Framebuffer.mode(DrawImmediately) 
*/
void Framebuffer::drawImmediately()
    {
    mode(DrawImmediately);
    }

/**
\brief Set mode to DrawFastest -- equivalent to Framebuffer.mode(DrawFastest) 
*/
void Framebuffer::drawFastest()
    {
    mode(DrawFastest);
    }

/**
\brief 'lock' allows access to the framebuffer for the current GLUT window.  lock' returns
a pointer 'framebuffer' that points to a Framebuffer object which can access the framebuffer 
of the current GLUT window.

Between calls to 'lock' and 'unlock' no direct OpenGL calls should be made.  It is permissible
to lock a buffer multiple times; however, every call to lock must have a corresponding 
call to unlock.

If lock returns false framebuffer is inaccessible (likely due to window being minmized).

\return true on success
\return false on failure (usually occurs if window is minimized) 
*/
bool Framebuffer::lock(Framebuffer* & framebuffer)
	{    
	using namespace std;    
	/*#
	get Pixelbuffer associated with the current GLUT window
	**/
	int windowID = glutGetWindow();
	assert_always2(windowID,"Framebuffer::lock() called outside of display callback");

	WindowIDToFramebuffer_t::iterator i;
	i = windowIDToFramebuffer_.find(windowID);
	if (i == windowIDToFramebuffer_.end())
		{
		framebuffer = new Framebuffer;
		windowIDToFramebuffer_[windowID] = framebuffer;
		}
	else
		framebuffer  = (*i).second;

	if (!framebuffer->locked())
		{
		framebuffer->lockCount++;
		return framebuffer->lock();
		}
	framebuffer->lockCount++;
	return true;
	}

/**
\brief 'unlock' unlocks the framebuffer 'framebuffer' associated with the current 
GLUT window.  
*/
void Framebuffer::unlock(Framebuffer* framebuffer)
    {
    framebuffer->lockCount--;
    if (framebuffer->lockCount == 0)
	framebuffer->unlock();
    }

/**
\brief Initialize Framebuffer dependent libraries, etc.
*/
void Framebuffer::init()
	{
	GLenum err = glewInit();
	if (GLEW_OK != err)
		{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
		}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));    

#ifdef USE_PBOS
	if (!GLEW_ARB_pixel_buffer_object)
		{
		fprintf(stdout, 
			"Framebuffer::init():\n"
			"Error: Pixel Buffer Object not available on this graphics card\n"
			"       Please recompile without: \n"
			"          #define OGT_FB_USE_PIXEL_BUFFER_OBJECTS\n"
			"       in Framebuffer.h\n"
			);
		}
	else
		fprintf(stdout, 
		"Framebuffer::init():\n"
		"Status: Using Pixel Buffer Objects\n");
#else
	if (!GLEW_ARB_pixel_buffer_object)
		fprintf(stdout, 
		"Framebuffer::init():\n"
		"Status: Not using Pixel Buffer Objects\n");
	else
		{
		fprintf(stdout, 
			"Framebuffer::init():\n"
			"Notice: Pixel Buffer Object support is available on this graphics card\n"
			"       You may see performance improvements by recompiling with\n"
			"\n"
			"          #define OGT_FB_USE_PIXEL_BUFFER_OBJECTS\n"
			"\n"
			"       in Framebuffer.h\n"
			);	
		}
#endif
	}

/**
\brief Set pixel at location 'x','y', to RGB color value (red,green,blue)

\pre location but be valid framebuffer coordinate - otherwise results are undefined
*/
void Framebuffer::setPixel(int x, int y, GLubyte red, GLubyte green, GLubyte blue)
	{
	BDCK_PIXEL(this,x,y)
	if (mode_ == DrawFastest)
		{
		pixels [(y*width_ + x)*COMPONENTS  ] = red;
		pixels [(y*width_ + x)*COMPONENTS+1] = green;
		pixels [(y*width_ + x)*COMPONENTS+2] = blue;
		}
	else
		{
		glBegin(GL_POINTS);
		glColor3ub(red,green,blue);
		glVertex2i(x,y);
		glEnd();
		GL_FORCE_PIXEL_OUT();
		}
	}

/**
\brief Copy image in 2D array 'rgb' into this Framebuffer starting at
Framebuffer location ('x','y') where ('x','y') is the lower-left corner 
of the destination rectangle in the Framebuffer. The rectangle is clipped before copying
into the Framebuffer. The width and height of 'rgb' are given by 'rgbWidth' and 'rgbHeight'.

*/  
void Framebuffer::clipAndSetRectangle(int x, int y, int rgbWidth, int rgbHeight, const GLubyte (*rgb)[3])
    {
	if (x+rgbWidth <= 0 || y+rgbHeight <= 0 || y >= height_ || x >= width_)
		// destination rectangle is completely outside framebuffer, so draw nothing
		return;

	int destX0 = max(x, 0);
	int srcX0 = x < 0 ? -x : 0;
	int destY0 = max(y, 0);
	int srcY0 = y < 0 ? -y : 0;
	int srcSizeY  = min(rgbHeight,height_- y - 1);
	int srcSizeX = min(rgbWidth, width_- x - 1);

	for (int destY=destY0, srcY=srcY0; srcY < srcSizeY; destY++,srcY++)
		setRow(destX0,destY,srcX0,srcSizeX-srcX0,&rgb[srcY*rgbWidth]);
    }

/**
\brief 	Set the row of pixels starting a location 'x','y' in this Framebuffer
by copying pixels from the array 'rgb[][3]' starting from 'rgb[offset]'
up to and including 'rgb[offset+length-1]'.

\pre rgb[][3] must not extend past the end of the framebuffer - otherwise 
     results are undefined
\pre rgb[][3] must have at least 'length' pixels - otherwise 
     results are undefined
*/
void Framebuffer::setRow(int x, int y, int offset, int length, const GLubyte rgb[][3])
    {
    BDCK_ROW(this,x,y,length)
	if (mode_ == DrawFastest)
		memcpy(pixels + (y*width_ + x)*COMPONENTS,rgb+offset,sizeof(GLubyte)*COMPONENTS*length);
	else
		{
		glRasterPos2i(x,y);
		glDrawPixels(length,1,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,rgb+offset);
		GL_FORCE_PIXEL_OUT();
		}
	}

/**
\brief Set the row of pixels starting location 'x','y' to the 
pixels in array 'rgb[][3]' which has 'nPixels' stored in it.

\pre rgb[][3] must not extend past the end of the framebuffer - otherwise 
     results are undefined
*/
void Framebuffer::setRow(int x, int y, int nPixels, const GLubyte rgb[][3])
    {
	BDCK_ROW(this,x,y,nPixels)
	if (mode_ == DrawFastest)
		memcpy(pixels + (y*width_ + x)*COMPONENTS,rgb,sizeof(GLubyte)*COMPONENTS*nPixels);
	else
		{
		glRasterPos2i(x,y);
		glDrawPixels(nPixels,1,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,rgb);
		GL_FORCE_PIXEL_OUT();
		}
    }

/**
\brief Set pixel at location 'locaiton' to RGB color value 'rgb'

\pre location but be valid framebuffer coordinate - otherwise results are undefined
*/
void Framebuffer::setPixel(const int location[2], const GLubyte rgb[3])
    {
    BDCK_PIXEL(this,location[0],location[1])
	if (mode_ == DrawFastest)
		{
		memcpy(pixels + (location[1]*width_ + location[0])*COMPONENTS,rgb,sizeof(GLubyte)*COMPONENTS);
		}
	else
		{
		glBegin(GL_POINTS);
		glColor3ubv(rgb);
		glVertex2iv(location);
		glEnd();
		GL_FORCE_PIXEL_OUT();
		}    
    }

/**
\brief Read RGB value of pixel 'x','y' into 'rgb'

\pre location but be valid framebuffer coordinate - otherwise results are undefined
*/
void Framebuffer::getPixel(int x, int y, GLubyte rgb[3])
    {
    BDCK_PIXEL(this,x,y)
	if (mode_ == DrawFastest)
		memcpy(rgb,pixels + (y*width_ + x)*COMPONENTS,sizeof(GLubyte)*COMPONENTS);
	else
		{
		glReadPixels(x,y,1,1,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,rgb);	    
		}
    }

/**
\brief 'mode' sets the mode for framebuffer access.  

\pre Framebuffer must be locked - otherwise process aborts
*/
void Framebuffer::mode(Mode m)
    {
	assert_always2(locked_,"Framebuffer::mode cannot change mode when Framebuffer isn't locked!");

	if (m != mode_)
		{       
		if (m == DrawFastest)
			{// switching into DrawFastest, so ...

			mode_ = DrawFastest;

			/* copy GL front FB to the back GL FB then swap buffers.

			(in DrawImmediately Mode we were temporarily drawing to front buffer to see
			immediate results.  Now we're returning to DrawFastest Mode where we draw
			to GL back FB.)*/
			glReadBuffer(GL_FRONT);
			glDrawBuffer(GL_BACK);
			glRasterPos2i(0,0);
			glCopyPixels(0,0,width_,height_,GL_COLOR);	    

#ifdef USE_PBOS
			/* copy GL back FB to pixel buffer object */
			glBindBuffer(GL_PIXEL_PACK_BUFFER,pixelBuffer);
			assert_always_OGL(glGetError() == GL_NO_ERROR);
			glReadPixels(0,0,width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER,NULL); // unbind from PACK_BUFFER
			assert_always_OGL(glGetError() == GL_NO_ERROR);
#else
			/* copy GL back FB to client pixel buffer */
			glReadPixels(0,0,width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,pixels);
#endif

			/* reset to ReadBuffer to back */
			glReadBuffer(GL_BACK);

#ifdef USE_PBOS
			/* map pixel buffer object to 'pixels' for writing to framebuffer */
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixelBuffer);
			pixels = reinterpret_cast<GLubyte*> (glMapBuffer(GL_PIXEL_UNPACK_BUFFER,GL_READ_WRITE));	
			assert_always_OGL(pixels != NULL);
#endif
			}
		else 
			{/* switching into DrawImmediately, so update GL FB with pixel buffer object 
			 data and then henceforth draw each pixel immediately */

#ifdef USE_PBOS
			/* unmap pixel buffer */
			assert_always_OGL(glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER)==GL_TRUE);
			pixels = NULL;

			/* copy pixel buffer to OGL back FB */
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixelBuffer);
			glRasterPos2i(0,0);
			glDrawPixels(width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,NULL);

			/* unbind pixel buffer */
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,NULL);
			assert_always_OGL(glGetError()==GL_NO_ERROR);
#else
			glRasterPos2i(0,0);
			glDrawPixels(width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,pixels);
#endif

			/* swap back to front and start drawing in front buffer for DrawImmediately mode */
			glutSwapBuffers();
			glDrawBuffer(GL_FRONT);
			mode_ = DrawImmediately;
			}
		}
	}

/*******************************************************************************
    PRIVATE FUNCTIONS (static func's,private member func's, etc.)
*******************************************************************************/

/**
\brief 'lock' allows access to the framebuffer for the current GLUT window.  Between calls to
'lock' and 'unlock' no OpenGL calls should be made.

\ret true on success
\ret false on failure (usually occurs if window is minimized) 
*/
bool Framebuffer::lock()
	{
	int 
		width  = glutGet(GLUT_WINDOW_WIDTH),
		height = glutGet(GLUT_WINDOW_HEIGHT);

	/** fail if window is degenerate */
	if (width == 0 || height == 0)
		return false;

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

	/**
	enable selected Framebuffer Mode 
	**/
	glPointSize(1.0);

	if (mode_ == DrawFastest)
		{       
		if (!pixelBuffer || width != width_ || height != height_)
			{// pixelBuffer needs allocation or resize, so do so

			/* allocate and (re)size pixel buffer object 'pixelBuffer' and initialize with
			frame buffer image (from 'pixelsCopy') */
			width_ = width;
			height_ = height;

#ifdef USE_PBOS
			if (!pixelBuffer)
				glGenBuffers(1,&pixelBuffer);
			assert_always_OGL(glGetError() == GL_NO_ERROR);

			glBindBuffer(GL_PIXEL_PACK_BUFFER,pixelBuffer);
			assert_always_OGL(glGetError() == GL_NO_ERROR);

			glBufferData(GL_PIXEL_PACK_BUFFER,sizeof(GLubyte)*COMPONENTS*width*height,NULL,GL_DYNAMIC_COPY);
			assert_always_OGL(glGetError() == GL_NO_ERROR);	    

			/* copy GL back framebuffer to pixel buffer */
			glReadPixels(0,0,width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,0);
			glBindBuffer(GL_PIXEL_PACK_BUFFER,NULL);
			assert_always_OGL(glGetError() == GL_NO_ERROR);

			/* rebind pixel buffer to unpack buffer for later glDrawPixels */
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixelBuffer);
			assert_always_OGL(glGetError() == GL_NO_ERROR);	    
#else
			/* copy GL back framebuffer to pixel buffer */
			pixelBuffer = 1;
			delete [] pixels;
			pixels = new GLubyte [width*height*COMPONENTS];
			glReadPixels(0,0,width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,pixels);
#endif
			}
		else
			{
#ifdef USE_PBOS
			/* copy GL back framebuffer to pixel buffer */
			glBindBuffer(GL_PIXEL_PACK_BUFFER,pixelBuffer);
			glReadPixels(0,0,width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,0);
			assert_always_OGL(glGetError() == GL_NO_ERROR);

			/* rebind pixel buffer to unpack buffer for later glDrawPixels */
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixelBuffer);
			assert_always_OGL(glGetError() == GL_NO_ERROR);	    
#else
			/* copy GL back framebuffer to pixel buffer */	    
			glReadPixels(0,0,width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,pixels);
#endif
			}
#ifdef USE_PBOS	
		glDrawBuffer(GL_BACK);	
		pixels = reinterpret_cast<GLubyte*> (glMapBuffer(GL_PIXEL_UNPACK_BUFFER,GL_READ_WRITE));	
		assert_always_OGL(pixels != NULL);
#endif
		}
	else
		{       
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawBuffer(GL_FRONT);
		}
	width_ = width;
	height_ = height;
	locked_ = true;
	return true;
	}



/**
\brief 'unlock' unlocks access to the framebuffer for this window.  Between calls to
'lock' and 'unlock' no OpenGL calls can be made.
*/
void Framebuffer::unlock()
	{
	if (mode_ == DrawFastest)
		{
#ifdef USE_PBOS
		assert_always_OGL(glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER)==GL_TRUE);
		pixels = NULL;
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pixelBuffer);
		glRasterPos2i(0,0);
		glDrawPixels(width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,NULL);
		assert_always_OGL(glGetError()==GL_NO_ERROR);
#else
		glRasterPos2i(0,0);
		glDrawPixels(width_,height_,Framebuffer::FORMAT,GL_UNSIGNED_BYTE,pixels);
#endif
		}
	else
		{
		glutSwapBuffers();
		glDrawBuffer(GL_BACK);
		}
	locked_ = false;

	/* \todo return to previous glViewport settings */

	/* return to previous view matrices */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	}