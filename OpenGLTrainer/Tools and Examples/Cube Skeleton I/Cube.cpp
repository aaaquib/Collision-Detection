/**
\file Cube.cpp
\author Zachary Wartell \n \n
   Copyright 2006.  Zachary Wartell, University of North Carolina at \n
   Charlotte. All rights reserved.

\brief A brief comment describing the purpose of this file goes here.

More detailed comments go here.  

TO DO LIST:
- \\ todo Replace the '\\ ' with '\' to generate a real todo comment

BUG LIST:
- \\ bug Replace the '\\ ' with '\' to generate a real bug comment. 

FOOTNOTES:
- [_F1_] <blank example. replace _F1_ with F1,F2, etc. for actual footnote>

REFERENCES:
- \anchor DK90 Donald Knuth, Algorithms and Data Structures, 1990.

\internal
*/

/*******************************************************************************
    Includes	
*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif
#include <math.h>
#include <Cube.h>
 

using namespace ZJW;

/*******************************************************************************
    Private Macros 
*******************************************************************************/
/* NONE */
/*******************************************************************************
    Private Function Prototypes 
*******************************************************************************/
/* NONE */
/*******************************************************************************
    Private Data Types 
*******************************************************************************/
/* NONE */

/*******************************************************************************
    Private Global Declarations 
*******************************************************************************/
const GLint Cube::faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };

const GLfloat Cube::n[6][3] = {  /* Normals for the 6 faces of a cube. */
	  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
	  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLfloat Cube::v[8][3];  
bool Cube::init=false;

/*******************************************************************************
    Internal Global Declarations 
*******************************************************************************/
/* NONE */
/*******************************************************************************
    Exported Global Declarations 
*******************************************************************************/
/* NONE */


/*******************************************************************************
    Exported Functions & Exported Class Member Functions 
*******************************************************************************/
/**
\brief Construct this cube
**/
Cube::Cube()
{
	if (!init)
		initVertices();
	color[0] = 1.0;
	color[1] = color[2] = 0.0;
	spin = 0;
	velocity = 45;
}

/**
\brief update cube pose
*/
void Cube::animate(float deltaT)
	{
	spin += velocity * deltaT;
	spin = fmod(spin,361);
	}

/**
\brief draw cube
*/
void Cube::draw()
{	
	/* Adjust cube position to be asthetic angle. */ 
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(spin, 0.0, 0.0, 1.0);

	/* draw vertices */
	glColor3fv(color);
	glBegin(GL_QUADS);
	int i;
	for (i = 0; i < 6; i++) 
		{		
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);	
		}
	glEnd();
}

/*******************************************************************************
    Internal Functions & Internal Class Member Functions 
*******************************************************************************/
/* NONE */
/*******************************************************************************
    Private Functions & Private Class Member Functions 
*******************************************************************************/
/*
\brief Init static member vertex data
*/
void Cube::initVertices()
{
	/* Setup cube vertex data. */
	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;
	init = true;
}
/* NONE */
