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
#ifndef Framebuffer3_H
#define Framebuffer3_H
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <OpenGLTrainer/OpenGLTrainer.h>

namespace ITCS4120
{
namespace OpenGLTrainer
{

namespace Take3
{
class Pixel
    {
    public:
    Pixel(GLubyte r,GLubyte g, GLubyte b);
    GLubyte* operator[](int i) {return rgb+i;}
    private:
    GLubyte rgb[3];
    };
/**
\brief Empirically glFlush isn't enough to force each new GL_POINT to be rendered
to the display so use glFinish instead.
*/
#define GL_FORCE_PIXEL_OUT() glFinish()
//#define GL_FORCE_PIXEL_OUT() glFlush()

/**

This class allows syntax such as:

frameBuffer [y][x] = Pixel(255,x%256,(x+10)%256);
frameBuffer [y][x].set(255,x%256,(x+10)%256);
frameBuffer [y][x][0] = red;
frameBuffer [y][x][1] = green;
frameBuffer [y][x][2] = blue;

But this class executes significantly slower in Debug compilation in the 'OpenGL Framebuffer' test application
than Take1::Framebuffer in Framebuffer.h.

This is true even for the comparatively faster:

frameBuffer [y][x].set(255,x%256,(x+10)%256);

The Release compilation does much better but I'm loath to suggest students compile in Release configuration
since many students are new C++ coders and Release compilation disables many useful debugging features.

So while I'd like to use this Take2::Framebuffer because it keeps the syntax of addressing 
array in memory in practice it is just too much of a performance issue even for a pedagodical
tool like this class.
*/
class Framebuffer
    {
    public:
    Framebuffer();
    
    typedef GLubyte Component;    
    class Pixel
	{
	public:
	inline Component& operator[] (int c) {return rgb[3];}
	private:
	Component rgb[3];
	};
    class PixelRow
	{
	public:	
	PixelRow(Pixel*p){pixels=p;}
	inline Pixel& operator [] (int x) { return pixels [x];}
	Pixel* pixels;
	};    
    class Pixelbuffer
	{
	public:
	Pixelbuffer();
	Pixel* image;	
	int width;
	int height;	
	void resize(int w, int h);
	};
    Pixelbuffer* pixelbuffer;
    enum {ALIGNMENT=1};
    public:
    inline PixelRow operator [](int y) { return PixelRow(pixelbuffer->image+pixelbuffer->width*y);}    
    void lock();
    void unlock();
    void flush();
    int width(){return pixelbuffer->width;}
    int height(){return pixelbuffer->height;}

    /**
     **  static member functions
     **/
    public:    
    typedef std::map<int,Pixelbuffer*> WindowIDToPixelbuffer;
    static WindowIDToPixelbuffer windowIDToPixelbuffer;
    };
}}};