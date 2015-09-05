/**
This is my sandbox for hacking around with coding ideas
**/
#ifdef _WIN32
#include <windows.h>
#endif

//#include <GL/GL.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
using namespace std;
void test(void);
namespace Test
{
/**
This approach allows syntax like:

    Framebuffer [y][x][c] = 255;

for setting row 'y' pixel 'x' and component 'c' (0 through 2 for RGB).

This code is NOT thread safe, however, since since it uses a global variable to 
store the start address of the current Framebuffer object.

It keeps a client copy of the GL framebuffer and uses address arithmatic to
work the magic of the 'Framebuffer [y][x][c]' notation without requiring
the Component, Pixel or PixelRow classes to have to have additional pointers 
back to the client copy.

I haven't thought about how the address arithmetic tricks would work with
pixel buffer objects but it seems quite possible.
*/
class Framebuffer
    {
    public:
    enum Mode { DrawImmediate, DrawFastest};
    int width_;
    int height_;
    int mode_;
    class Component
	{
	public:
	void operator = (GLubyte c)
	    {
	    value = c;
	    if (Framebuffer::framebuffer.mode_ == DrawImmediate)
		{
		size_t diff;
		size_t y,cx,x,c;
		diff = (GLubyte*)this - (GLubyte*)framebuffer.pixels;
		y = diff / (framebuffer.width_*sizeof(Pixel));
		cx = diff % (framebuffer.width_*sizeof(Pixel));
		x = cx / sizeof(Pixel);
		c = cx % sizeof(Pixel);
		cout << "Write Pixel: [" << y << "][" << x << "][" << c << "]\n";
		//glBegin(GL_PIXELS); glColor3ubv(???); glVertex2i(x,y); glEnd();
		}
	    }
	private:
	GLubyte value;	
	};
    class Pixel
	{
	public:

	inline Component& operator[] (int c) 
	    { 
	    return rgb[c];
	    }
	Component rgb[3];
	};
    class PixelRow
	{
	public:
	PixelRow(Pixel*p){pixels=p;}
	Pixel& operator [] (int x) 
	    { 
	    return pixels[x]; 
	    }
	private:
	Pixel* pixels;
	friend Framebuffer;
	};
    inline PixelRow operator [] (int y) 
	{ 
	//PixelRow* pr = reinterpret_cast<PixelRow*>(pixels+y);
	//return *reinterpret_cast<PixelRow*>(pixels+y);
	return PixelRow(pixels+y*framebuffer.width_);
	}
    private:
    Pixel* pixels;
    GLubyte (*image)[3];
        
    static Framebuffer framebuffer;

    friend Component;
    friend Pixel;
    public:
    
    friend extern void ::test(void);
    };
};
using namespace Test;
Framebuffer Framebuffer::framebuffer;

void test(void)
    {    
    Framebuffer framebuffer;
    GLubyte 
	(*pixel)[3];
    GLubyte red;
    
    framebuffer.width_ = 500; 
    framebuffer.height_ = 500;
    framebuffer.image = reinterpret_cast<GLubyte(*)[3]>(
	new GLubyte [framebuffer.width_*framebuffer.height_*3]);
    red = framebuffer.image[100][0];
    pixel = framebuffer.image+100*framebuffer.width_ + 200;
    int diff,x,y;
    diff = pixel - framebuffer.image;
    y = diff / framebuffer.width_;
    x = diff % framebuffer.width_;

    framebuffer.pixels = new Framebuffer::Pixel [framebuffer.width_*framebuffer.height_];

    framebuffer [10][5][2] = 10;
    exit(1);
    }

#include <OpenGLTrainer/Image.h>

void test1(void)
    {
    using namespace ITCS4120::OpenGLTrainer::Experimental;
    Image2D<GL_RGB,GLubyte,50,50> image1;
    Image2D<GL_RGB,GLubyte> image2 (10,10);
    
    image1 [49][20][2] = 10;
    image2 [9][9][0] = 10;
    }