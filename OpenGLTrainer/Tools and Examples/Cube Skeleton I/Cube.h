#ifndef CUBE_H
#define CUBE_H
/**
\file file.h
\author John Doe \n
   Copyright 2006.  Zachary Wartell, University of North Carolina at \n
   Charlotte. All rights reserved.  

\brief A Brief comment describing the purpose of this file goes here.

More details can comments go here.  But often you might be happier putting
them in file.c so that you can edit them without having to recompile all
files that #include this one.

*/

/*******************************************************************************
    Includes	
*******************************************************************************/
//#include <MyLibrary/MyLibrary_exports.h>
#include <GL/glew.h>  
/*******************************************************************************
    Macros
*******************************************************************************/
/* NONE */
/*******************************************************************************
    Data Types 
*******************************************************************************/
/**
\brief All code in ZJW namespace should be completely replaced by the student to implement
their assignment.
*/
namespace ZJW
{

/**
\brief A dumbed-down Cube class
**/
class Cube
{
public:
	Cube();
	void draw();
	void animate(float deltaT);

	// OGL color
	GLfloat color[3];
	// orientaiton angle
	GLfloat spin;
	// angular velocity
	GLfloat velocity;
private:

	/**
	static data members (shared by all instances of Cube)
	**/

	/* Normals for the 6 faces of a cube. */
	static const GLfloat n[6][3]; 

	/* Vertex indices for the 6 faces of a cube. */
	static const GLint faces[6][4]; 

	/* Vertex coordinates */
	static GLfloat v[8][3];  

	static bool init;

	static void initVertices();
};
}
/*******************************************************************************
    Function Prototypes
*******************************************************************************/
/* NONE */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

/*******************************************************************************
    Global Variables
*******************************************************************************/
/* NONE */
#endif
