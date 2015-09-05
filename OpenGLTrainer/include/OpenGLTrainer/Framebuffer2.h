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
#ifndef Framebuffer2_H
#define Framebuffer2_H
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <OpenGLTrainer/OpenGLTrainer.h>

namespace ITCS4120
{
namespace OpenGLTrainer
{

namespace Take2
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
#if 0
    class PixelPtr
	{
	private:
	int row;
	int column;
	};
    class PixelRow
	{	
	PixelPtr& operator[] (int x);
	private:
	int row;
	};
    PixelRow& operator[](int y)
	{
	return pixelbuffer.rows[y];
	}
    class Pixelbuffer2
	{
	PixelRow* rows;
	int width;
	int height;
	};
#endif
    public:
    Framebuffer();
    class Pixelbuffer;
    typedef GLshort Coordinate;
    enum Mode {DrawImmediately, DrawFastest};
    class Component
	{
	public:
	void operator= (GLubyte v)
	    {
	    value = v;	   
	    Component* pixel = &pixelbuffer->image[loc[1]*pixelbuffer->width*3+loc[0]*3];
	    glColor3ub(pixel[0].value,pixel[1].value,pixel[2].value);
	    if (pixelbuffer->mode == DrawImmediately)
		{
		glBegin(GL_POINTS);
		glVertex2sv(loc);
		glEnd();
		GL_FORCE_PIXEL_OUT();
		}
	    else
		glVertex2sv(loc);
	    }
	enum Color{RED,GREEN,BLUE};
	
	Color color;
	Coordinate loc[2];
	GLubyte value;
	Pixelbuffer* pixelbuffer;
	};
    class Pixel
	{
	public:
	void operator= (Take2::Pixel& p)
	    {
	    glColor3ubv((GLubyte*)&p);
	    if (components->pixelbuffer->mode == DrawImmediately)
		{
		glBegin(GL_POINTS);
		glVertex2sv(components->loc);
		glEnd();
		GL_FORCE_PIXEL_OUT();
		}
	    else
		glVertex2sv(components->loc);
	    }
	void set(GLubyte r,GLubyte g, GLubyte b)
	    {
	    glColor3ub(r,g,b);
	    if (components->pixelbuffer->mode == DrawImmediately)
		{
		glBegin(GL_POINTS);
		glVertex2sv(components->loc);
		glEnd();
		GL_FORCE_PIXEL_OUT();
		}
	    else
		glVertex2sv(components->loc);
	    }
	Pixel (Component* c) {components = c;}
	inline Component& operator [] (int c) { return components[c];}
	Component* components;
	};

    class PixelRow
	{
	public:
	PixelRow (Component* p) {pixels = p;}
	inline Pixel operator [] (int x) { return Pixel(pixels + x*3);}
	Component* pixels;
	};    
    class Pixelbuffer
	{
	public:
	Pixelbuffer();
	Component* image;
	PixelRow* rows;
	int width;
	int height;	
	Mode mode;
	void resize(int w, int h);
	};
    Pixelbuffer* pixelbuffer;
    
    public:
    inline PixelRow operator [](int y) { return PixelRow(pixelbuffer->image+pixelbuffer->width*y*3);}
    Component& operator() (int x, int y) { return pixelbuffer->image[(y*pixelbuffer->width+x)*3];}
    void lock(Mode m);
    void unlock();
    void mode(Mode m);
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