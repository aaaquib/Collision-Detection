/**
\author Zachary Wartell - Copyright 2007.  All rights reserved.  This code may not be
reused for commericial or non-commericial purposes or projects outside Prof. Wartell's 
courses without expression permission from the author.

\brief Image.h defines Image template classes that easy creation of C++ objects
that are compatible with OpenGL client size images.

TO DO LIST:
\todo add Image2D class whose GL image format and GL image type is defined at run-time
\todo add functions for converting between these Image2D classes
\todo [PRIORITY=MED] add support for other GL image formats and image GL types
Ideally do this by parsing gl.h or glew.h, etc.
\todo Make some form of adaptor template class for using FreeImage library to load
Image2D classes (this should be a separately compiled unit that USES OpenGLTrainer
and FreeImage).
\todo [PRIORITY=LOW] make classes for 1D and 3D GL images

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] 

\internal
*/
#ifndef OGLTRAINER_IMAGE_H
#define OGLTRAINER_IMAGE_H

/*******************************************************************************
    INCLUDES
*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <OpenGLTrainer/OpenGLTrainer.h>

namespace ITCS4120
{
namespace OpenGLTrainer
{
namespace Experimental
{

template <GLenum FORMAT>
class Format
    {
    };


/******************************************************************************
    PUBLIC USE TEMPLATES
******************************************************************************/

class UnknownType
    {
    unsigned char dummy;
    };

/**
\brief Instantiations of this template are statically allocated 2D image arrays.
They are NOT resizable and their size must be specified at compile time.

As with C style arrays no bounds checking is performed.  

Example Usage:

Image2D<GL_RGB,GLunsigned,50,50> image;

image [10][10][0] = red;
image [10][10][1] = green;
image [10][10][2] = blue;

Limitations:

Only the following GLenum's (FORMAT) and GL types (TYPE) are currently supported:

	GLubyte
	GLbyte
	GLshort
	GLushort
	GLint
	GLuint
	GLfloat
	GLdouble

	GL_RGB
	GL_RGBA
	GL_RED
	GL_BLUE
	GL_GREEN
	GL_ALPHA
	GL_LUMINANCE

Performance:

An optimizing compiler should make access via the [] operators equivalent to direct
C array access for statically allocated arrays.
*/
template <GLenum FORMAT=GL_NO_ERROR, typename TYPE=UnknownType, int WIDTH=-1, int HEIGHT=-1>
class Image2D
    {
    public:
    /**
    \brief Return width of this Image2D 
    */
    int width()const { return WIDTH;}
    /**
    \brief Return height of this Image2D 
    */
    int height()const { return HEIGHT;}
    /**
    \brief Return GL format of this Image2D 
    */
    GLenum format()const { return FORMAT;}
    /**
    \brief Return GL type of this Image2D 
    */
    GLenum type()const { return Type<TYPE>;}

    /**
    \brief Return point to this Image2D image data
    */
    GLvoid* data() { return image_;}

    /**
    GL type of the pixels in this Image2D
    */
    typedef TYPE Type;

    /**
    \brief 'Row' class helps provides the 'Image2D [y][x][c]' interface to this
    Image2D's pixel data.
    */
    class Row
	{
	private:
	inline Row(TYPE (*row) [WIDTH][Format<FORMAT>::COMPONENTS]){row_=row;}
	public:
	inline TYPE* operator[] (int x) { return &(*row_)[x][0]; }
	TYPE (*row_) [WIDTH][Format<FORMAT>::COMPONENTS];
	friend class Image2D<FORMAT,TYPE,WIDTH,HEIGHT>;
	};

    /**
    \brief Return row 'y' of ('Image2D [y]') of to this Image2D's pixel data.
    */
    inline Row operator[] (int y) { return Row(&image_[y]); }
    
    /**
    \brief Call glDrawPixels as follows:

	glDrawPixels(this->width(),this->height(),this->format(),this->type(),this->data());
    */
    static void glDrawPixels();
    private:
    TYPE image_[HEIGHT][WIDTH][Format<FORMAT>::COMPONENTS];
    };

template <GLenum FORMAT, typename TYPE, int WIDTH, int HEIGHT>
void Image2D<FORMAT,TYPE,WIDTH,HEIGHT>::glDrawPixels()
    {
    glDrawPixels(width(),height(),format(),type(),data());
    }

/**
\brief Instantiations of this template are dynamically allocated and resizable
2D image arrays but their GL Format and GL type are fixed at compile time.

As with C style arrays no bounds checking is performed.  

Usage:

Image2D<GL_RGB,GLunsigned> image(10,10);
image [y][x][0] = red;
image [y][x][1] = green;
image [y][x][2] = blue;

Performance:

An optimizing compiler should make access via the [] operators equivalent to direct
C array access for dynamically allocated arrays.
*/
template <GLenum FORMAT,typename TYPE>
class Image2D <FORMAT,TYPE,-1,-1>
    {
    public:
    Image2D(int width, int height);
    Image2D();
    ~Image2D();

    typedef TYPE Type;
    GLenum format()const { return FORMAT;}
    GLvoid* data() { return image_;}

    void resize(int width, int height);


    /**
    \brief Return pixel 'x' in this Row in an Image2D.

    Usage:
	Image2D<GL_RGB,GL_UNSIGNED> image(10,10);
	GLunsigned* components = image [y][x];
	components [0] = red;
	components [1] = green;
	components [2] = blue;
	
    */
    class Row
	{
	public:
	inline Type* operator [] (int x) {return row_ + x*Format<FORMAT>::COMPONENTS;}
	private:
	inline Row (Type* row){row_= row;}
	TYPE* row_;
	friend class Image2D <FORMAT,TYPE,-1,-1>;
	};

    /**
    \brief Return 'y' row in this image.

    Usage:
	image [y][x];
    */
    inline Row operator[] (int y) { return image_ + (y * width_*Format<FORMAT>::COMPONENTS);}

    private:
    int width_;
    int height_;
    TYPE* image_;
    };

template <GLenum FORMAT,typename TYPE>
Image2D<FORMAT,TYPE>::Image2D (int width, int height)
    {
    width_ = width_;
    height_ = height_;
    image_ = new TYPE [width_*height_*Format<FORMAT>::COMPONENTS];
    }

/**
\brief 'resize' resizes this Image2D.  Any previous allocated memory is
deallocated.
*/
template <GLenum FORMAT,typename TYPE>
void Image2D<FORMAT,TYPE>::resize (int width, int height)
    {
    delete[] image_;
    width_ = width; height_ = height;
    image_ = new TYPE [width_ * height_ * Format<FORMAT>::COMPONENTS];
    }

/**
\brief 'resize' resizes this Image2D.  Any previous allocated memory is
deallocated.
*/
template <GLenum FORMAT,typename TYPE>
Image2D<FORMAT,TYPE>::~Image2D ()
    {
    delete[] image_;
    }

#if 0
/**
\brief Instantiations of this template are dynamically allocated and resizable
2D image arrays and their GL Format and GL type are set a run-time.

As with C style arrays no bounds checking is performed.  

Usage:

Image2D<> image(GL_RGB,GL_UNSIGNED,10,10);
image [y][x][0] = red;
image [y][x][1] = green;
image [y][x][2] = blue;

Performance:

An optimizing compiler should make access via the [] operators equivalent to direct
C array access for dynamically allocated arrays.
*/
template <>
class Image2D <GL_NO_ERROR,UnknownType,-1,-1>
    {
    public:
    Image2D(GLenum format, GLenum type,int width, int height);
    Image2D();
    ~Image2D();

    typedef UnknownType Type;
    GLenum format()const { return format_;}
    GLenum type()const { return type_;}
    GLvoid* data() { return image_;}

    void resize(int width, int height);


    /**
    \brief Return pixel 'x' in this Row in an Image2D.

    Usage:
	Image2D<> image(GL_RGB,GL_UNSIGNED,10,10);
	GLubyte* components = (GLubyte*) image [y][x];
	components [0] = red;
	components [1] = green;
	components [2] = blue;
	
    */
    class Row
	{
	public:
	inline GLvoid* operator [] (int x) {return row_ + x*;}
	private:
	inline Row (GLvoid* row,Image2D <GL_NO_ERROR,UnknownType,-1,-1>* image2D){image2D_=image2D;row_= row;}
	GLvoid* row_;
	Image2D <GL_NO_ERROR,UnknownType,-1,-1> *image2D_;
	friend class Image2D <GL_NO_ERROR,UnknownType,-1,-1>;
	};

    /**
    \brief Return 'y' row in this image.

    Usage:
	image [y][x];
    */
    inline Row operator[] (int y) 
	{ 
	return Row((image_ + y * width_)*components_*componentsSize_,this);
	}

    private:
    int width_;
    int height_;
    GLenum format_;
    GLenum type_;

    size_t componentSize_;
    int components_;
    GLvoid* image_;
    };

template <>
Image2D<GL_NO_ERROR,UnknownType,-1,-1>::Image2D (GLenum format, GLenum type,int width, int height)
    {
    width_ = width_;
    height_ = height_;
    type_ = type;
    format_ = format;
    image_ = new TYPE [width_*height_*TYPE_TO_SIZE[type_]*FORMAT_TO_COMPONENTS[format_]];
    }
#endif
/******************************************************************************
    INTERNAL USE TEMPLATES
******************************************************************************/

#define SPECIALIZE_Format(GLENUM,COMPONENTS_COUNT)\
template <>\
class Format<GLENUM>\
    {\
    public:\
    enum {COMPONENTS=COMPONENTS_COUNT};\
    };

SPECIALIZE_Format(GL_RGB,3);
SPECIALIZE_Format(GL_RGBA,4);
SPECIALIZE_Format(GL_RED,1);
SPECIALIZE_Format(GL_BLUE,1);
SPECIALIZE_Format(GL_GREEN,1);
SPECIALIZE_Format(GL_ALPHA,1);
SPECIALIZE_Format(GL_LUMINANCE,1);

template <typename TYPE>
class Type
    {
    };

#define SPECIALIZE_Type(TYPE,GL_TYPE_)\
template <>\
class Type<TYPE>\
    {\
    public:\
    enum {GL_TYPE=GL_TYPE_};\
    };

SPECIALIZE_Type(GLubyte,GL_UNSIGNED_BYTE);
SPECIALIZE_Type(GLbyte,GL_BYTE);
SPECIALIZE_Type(GLshort,GL_SHORT);
SPECIALIZE_Type(GLushort,GL_UNSIGNED_SHORT);
SPECIALIZE_Type(GLint,GL_INT);
SPECIALIZE_Type(GLuint,GL_UNSIGNED_INT);
SPECIALIZE_Type(GLfloat,GL_FLOAT);
SPECIALIZE_Type(GLdouble,GL_DOUBLE);

}}};
#endif
