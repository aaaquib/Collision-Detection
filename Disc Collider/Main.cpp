/**
Copyright 2007, All Rights Reserved: Zachary Wartell, University of North Carolina at Charlotte
\file Main.cpp
\author Zachary Wartell
\brief Main.cpp illustrates how to use the PanZoomWindow class.  The pan and zoom user interface is
documented in \ref PanZoomWindow_USER_INTERFACE and quoted below:

    "Left-click + CTRL : mouse movements pans the contents of the pan-zoom window.
     Left-click + CTRL + SHIFT: vertical mouse movement zooms in and out.
     mouse wheel : zooms in and out
     (mouse wheel support available only with compatible GLUT libraries)"

Additionally, Left-click and drag moves a dot around on the screen.

TO DO LIST:
\todo

BUG LIST:
\bug

FOOTNOTES:
- [F1] This is an example footnote.

REFERENCES:
- [R1] Donald Hearn and M. Pauline Baker.  Computer Graphics with OpenGL: Third Edition.
*/

/*******************************************************************************
    Includes	
*******************************************************************************/
#ifdef _WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <iostream>
#include <math.h>
#include <time.h>

#if 0
#include <GL/gl.h>
#else
// replacement for GL/gl.h incase an advanced student wants to use OGL extensions, OGL 2.1, etc.
#include <GL/glew.h>   
#endif

#include <GL/glut.h>

#include <OpenGLTrainer/OpenGLTrainer.h>
#include <OpenGLTrainer/PanZoomWindow.h>
#include <OpenGLTrainer/Framebuffer.h>

using namespace std;

/*******************************************************************************
    File Scope (static) Globals
*******************************************************************************/
/** lowerLeft and upperRight corner of axis aligned rectangle in world coordinates
    where we will draw all our stuff */
static const float PLAY_FIELD[2][2] = {{0,0},{1e6,1e6}};

/*******************************************************************************
    File Scope (static) Functions
*******************************************************************************/
using namespace ITCS4120::OpenGLTrainer;

class MyPanZoomWindow : public PanZoomWindow
    {
    public:
    MyPanZoomWindow (const float viewLowerLeft[2], const float viewUpperRight[2]);
    
    /** Overridden callback member functions.

	Each of these is automatically called whenever the similarly name GLUT callback function
	is called for the GLUT window created by this PanZoomWindow.
     */
    virtual void display(void);
    virtual void motion (int x, int y);
    virtual void passiveMotion (int x, int y);
    virtual void keyboard (unsigned char key, int x, int y);
    virtual void mouse (int button, int state, int x, int y);

    /** non-virtual functions */
    void resetOpenGL(void);
    bool firstDisplay;
    bool firstSelect;
    bool secondSelect;
    bool firstClick;
    
    /** All the following functions and data members must be replaced by the student!

    They implement simple drawing and interaction above and beyond the standard
    functionality of PanZoomWindow.  They are provided just to illustrate proper usage of 
    a PanZoomWindow derived class.
    */
    struct Disc
	{
	int x;
	int y;
	float rvalue;
	float gvalue;
	float bvalue;
	}discs[100000];
    struct Grid
	{
	int x;
	int y;
	int noDiscs;
	bool selected;
	Disc * containedDiscs[10];
	}playGrid[1000][1000];

    void InsertDiscs();
    void DrawDiscs(float cx,float cy);
    void GenerateDiscs();
    void Initialize();
    GLuint createDL();
    //void changeSize(int w, int h) ;
    inline void ZJW_mouse (int button, int state, int x, int y);
    inline void ZJW_passiveMotion (int x, int y);    
    inline void ZJW_motion(int gx, int gy);
    void ZJW_draw_frame();
    void SelectIntersectedCells(int x1,int y1,int x2,int y2, bool bline);
    void setPixel(int px, int py, int x1, int y1, int x2, int y2, bool bline);
    void highlightDiscsFunc();
    void DeleteIntersectedDiscs(int x1,int y1,int x2,int y2,Disc * containedDiscs, bool bline);

    bool ZJW_drag;
    int cell_width;
    int cell_height;
    int disc_radius;
    GLuint discID;
    int selectedRect1x,selectedRect1y,selectedRect2x,selectedRect2y;
    int spaceCounter;
    bool rectSelect;
    bool highlightDiscs;
    bool deleteDiscs;

    struct Point
	{
	int x;
	int y;
	} ZJW_point;    
    } panZoomWindow (PLAY_FIELD[0],PLAY_FIELD[1]);

/*******************************************************************************
    File Scope (static) Functions
*******************************************************************************/
using namespace ITCS4120::OpenGLTrainer;

/**
\brief Construct a PanZoomWindow whose view window is initially bounded by 'viewLowerLeft' and
'viewUpperRight'.
*/
MyPanZoomWindow::MyPanZoomWindow (const float viewLowerLeft[2], const float viewUpperRight[2]) : 
	PanZoomWindow (viewLowerLeft,viewUpperRight)
    {
    firstDisplay = true;
    firstClick=true;
    highlightDiscs = false;
    deleteDiscs=false;
    cell_width=1000;
    cell_height=1000;
    disc_radius=250;
    spaceCounter=1;
    for(int i=0;i<100000;i++)
	{
	discs[i].rvalue=0.2;
	discs[i].gvalue=0.8;
	discs[i].bvalue=0.2;
	}
	Initialize();
	GenerateDiscs();
	InsertDiscs();
    }

GLuint MyPanZoomWindow::createDL() {
	GLuint listID;

	listID = glGenLists(1);
	
	glNewList(listID,GL_COMPILE);
	
	for(int i=0;i<100000;i++)
	    {
	    glPushMatrix();
	    glColor3f(discs[i].rvalue,discs[i].gvalue,discs[i].bvalue);
	    MyPanZoomWindow::DrawDiscs(discs[i].x,discs[i].y);
	    glPopMatrix();
	    }
	glEndList();

	return(listID);
}

void MyPanZoomWindow::Initialize()
    {	    
	    for(int y=0;y<1000;y++)
		{
		for(int x=0;x<1000;x++)
		    {
		    playGrid[x][y].x=x*1000;
		    playGrid[x][y].y=y*1000;
		    playGrid[x][y].noDiscs=0;
		    }
		}
         }

void MyPanZoomWindow::GenerateDiscs()
    {
    srand(time(0));
    for(int i=0;i<100000;i++)
	{
	discs[i].x= (rand()*32 % 1000000) + rand()%32;
	discs[i].y= (rand()*32 % 1000000) + rand()%32;
	
	}
    }

void MyPanZoomWindow::DrawDiscs (float cx,float cy)
    {
    // draw a circle centered at (xc,yc) with radius disc_radius
    
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx,cy);
	for (int angle=0; angle<=360; angle=angle+20)
	    {
	    float angle_radians = angle * (float)3.14159 / (float)180;
	    float x = cx + disc_radius * (float)cos(angle_radians);
	    float y = cy + disc_radius * (float)sin(angle_radians);
	    glVertex2f(x,y);
	    }
	glEnd();
    }

void MyPanZoomWindow::InsertDiscs()
    {
    int gx,gy,tempx,tempy,j;
    for(int i=0;i<100000;i++)	    //For each disc
	{
	    //calculate the location (lower left corner) of the cell in which the disc lies 
	tempx=discs[i].x - discs[i].x % cell_width;
	tempy=discs[i].y - discs[i].y % cell_height;
			

	gx=tempx/1000;		    //calculate the cell array index ,gx and gy, in which to insert the disc
	gy=tempy/1000;
	
	j=playGrid[gx][gy].noDiscs;
			//store the pointer to the disc in the grid datastructure
	playGrid[gx][gy].containedDiscs[j]=&discs[i];
	playGrid[gx][gy].noDiscs++;
			//Check if disc also lies in adjacent cells
	if(discs[i].x + disc_radius >= tempx + cell_width && gx<999)	    //right cell
	    {
	    j=playGrid[gx+1][gy].noDiscs;
	    playGrid[gx+1][gy].containedDiscs[j]=&discs[i];
	    playGrid[gx+1][gy].noDiscs++;
	    }
	if(discs[i].x - disc_radius < tempx && gx>0)		    //left cell
	    {
	    j=playGrid[gx-1][gy].noDiscs;
	    playGrid[gx-1][gy].containedDiscs[j]=&discs[i];
	    playGrid[gx-1][gy].noDiscs++;
	    }
	if(discs[i].y + disc_radius >= tempy + cell_height && gy<999)		    //top cell
	    {
	    j=playGrid[gx][gy+1].noDiscs;			    
	    playGrid[gx][gy+1].containedDiscs[j]=&discs[i];
	    playGrid[gx][gy+1].noDiscs++;
	    }
	if(discs[i].y - disc_radius < tempy && gy>0)		    //bottom cell
	    {
	    j=playGrid[gx][gy-1].noDiscs;			    
	    playGrid[gx][gy-1].containedDiscs[j]=&discs[i];
	    playGrid[gx][gy-1].noDiscs++;
	    }
	if(discs[i].x + disc_radius/(sqrtf(2)) >= tempx + cell_width && discs[i].y + disc_radius/(sqrtf(2)) >= tempy + cell_height  && gy<999 && gx<999)		    //top right cell
	    {
	    j=playGrid[gx+1][gy+1].noDiscs;			    
	    playGrid[gx+1][gy+1].containedDiscs[j]=&discs[i];
	    playGrid[gx+1][gy+1].noDiscs++;
	    }
	if(discs[i].x - disc_radius/(sqrtf(2)) < tempx && discs[i].y + disc_radius/(sqrtf(2)) >= tempy + cell_height  && gy<999 && gx>0)		    //top left cell
	    {
	    j=playGrid[gx-1][gy+1].noDiscs;			    
	    playGrid[gx-1][gy+1].containedDiscs[j]=&discs[i];
	    playGrid[gx-1][gy+1].noDiscs++;
	    }
	if(discs[i].x - disc_radius/(sqrtf(2)) < tempx && discs[i].y - disc_radius/(sqrtf(2)) < tempy && gy>0 && gx>0)		    //bottom left cell
	    {
	    j=playGrid[gx-1][gy-1].noDiscs;			    
	    playGrid[gx-1][gy-1].containedDiscs[j]=&discs[i];
	    playGrid[gx-1][gy-1].noDiscs++;
	    }
	if(discs[i].x + disc_radius/(sqrtf(2)) >= tempx + cell_width && discs[i].y - disc_radius/(sqrtf(2)) < tempy && gy>0 && gx<999)		    //bottom right cell
	    {
	    j=playGrid[gx+1][gy-1].noDiscs;			    
	    playGrid[gx+1][gy-1].containedDiscs[j]=&discs[i];
	    playGrid[gx+1][gy-1].noDiscs++;
	    }

	}
    }

/**
\brief initialize some default OGL settings
*/
void MyPanZoomWindow::resetOpenGL (void)
    {
    //set clear color to black
    glClearColor(0.0f,0.0f,0.0f,1.0f);    
    }

/**
\brief 'motion' handle mouse motions.
*/
void MyPanZoomWindow::motion
	(
	int gx,  // 'glutX' - mouse X in GLUT coordinates
	int gy   // 'glutY' - mouse Y in GLUT coordinates
	)
    {
    /* perform standard PanZoomWindow motion event processing */
    PanZoomWindow::motion(gx,gy);

    /* do something with mouse motion */
    ZJW_motion(gx,gy);
    }

/**
\brief 'mouse' handles mouse button presses.
*/
void MyPanZoomWindow::mouse(int button, int state, int x, int y)
    {
    /* perform standard PanZoomWindow mouse event processing */
    PanZoomWindow::mouse(button,state,x,y);

    /* perform additional mouse event processing */    
    ZJW_mouse(button, state, x, y);
    }

/**
\brief 'passiveMotion' handles passiveMotion.
*/
void MyPanZoomWindow::passiveMotion(int x, int y)
    {
    /* perform standard PanZoomWindow passiveMotion event processing */
    PanZoomWindow::passiveMotion(x,y);

    /* perform additional passiveMotion event processing */
    // \todo do stuff...
    }

/**
\brief 'keyboard' handles keypresses.
*/
void MyPanZoomWindow::keyboard (unsigned char key, int x, int y)
    {
    /* perform standard PanZoomWindow keyboard event processing */
    PanZoomWindow::keyboard(key,x,y);

    /* perform additional keyboard event processing */
    // \todo do stuff...
    
    if(key == ' ' && spaceCounter==1)
	{
	highlightDiscs=true;
	spaceCounter++;
	}
    else if(key == ' ' && spaceCounter==2)
	{
	spaceCounter++;
	deleteDiscs=true;
	}
    else if(key == ' ' && spaceCounter==3)
	{
	highlightDiscs=false;
	spaceCounter=1;
	deleteDiscs=false;
	firstSelect=false;
	secondSelect=false;
	firstClick=true;
	}
    glutPostRedisplay();
    }


/**
\brief 'display' is called whenever the GLUT display callback function for this object's
GLUT window is called.
*/
void MyPanZoomWindow::display(void)
    {      
    /**
	perform any 'one time' setup OGL calls.

	Note, OGL calls are only legal during execution of a GLUT display callback.  Below 
	is a standard method to make any OpenGL calls that we wish to make only the first
	time the GLUT display callback is executed.
     **/    
    if (firstDisplay)
	{
	resetOpenGL();
	discID=createDL();
	firstDisplay = false;
	}

    /**
	setup frame
     **/   
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setOpenGLView();
    
    /**
	draw frame
     **/
    ZJW_draw_frame();
 
    /**
	end frame
     **/    
    ITCS4120::OpenGLTrainer::showFrameRate();
    assert(glGetError()==GL_NO_ERROR);
    glutSwapBuffers();
    }

// \brief draw some random stuff
void MyPanZoomWindow::ZJW_draw_frame()
    {
    const int WIDTH = PLAY_FIELD[1][0]-PLAY_FIELD[0][0];
    const int HEIGHT = PLAY_FIELD[1][1]-PLAY_FIELD[0][1];
    const int CENTER_X = WIDTH/2;
    const int CENTER_Y = HEIGHT/2;

    /* draw play field */
    glColor3ub(120,120,200);
    glBegin(GL_QUADS);
	glVertex2fv(PLAY_FIELD[0]);
	glVertex2f (PLAY_FIELD[1][0],PLAY_FIELD[0][0]);
	glVertex2fv(PLAY_FIELD[1]);
	glVertex2f (PLAY_FIELD[0][0],PLAY_FIELD[1][0]);
    glEnd();
   
//Draw Selected Cells..
    glColor4f(0.8,0.0,0.5, 0.4);
    glLineWidth(1);
    if(firstSelect==true)
	{
	glBegin(GL_POLYGON);
	glVertex2i(playGrid[selectedRect1x][selectedRect1y].x,playGrid[selectedRect1x][selectedRect1y].y);
	glVertex2i(playGrid[selectedRect1x][selectedRect1y].x + cell_width,playGrid[selectedRect1x][selectedRect1y].y);

	glVertex2i(playGrid[selectedRect1x][selectedRect1y].x + cell_width,playGrid[selectedRect1x][selectedRect1y].y + cell_height);
	glVertex2i(playGrid[selectedRect1x][selectedRect1y].x,playGrid[selectedRect1x][selectedRect1y].y + cell_height);
	glEnd();
	}
    if(secondSelect==true)
	{
	glBegin(GL_POLYGON);
	glVertex2i(playGrid[selectedRect2x][selectedRect2y].x,playGrid[selectedRect2x][selectedRect2y].y);
	glVertex2i(playGrid[selectedRect2x][selectedRect2y].x + cell_width,playGrid[selectedRect2x][selectedRect2y].y);

	glVertex2i(playGrid[selectedRect2x][selectedRect2y].x + cell_width,playGrid[selectedRect2x][selectedRect2y].y + cell_height);
	glVertex2i(playGrid[selectedRect2x][selectedRect2y].x,playGrid[selectedRect2x][selectedRect2y].y + cell_height);
	glEnd();
	}


    //Highlight all cells intersected by the line segment
    if(firstSelect==true && secondSelect==true)
	{
	int dx = (playGrid[selectedRect2x][selectedRect2y].x - playGrid[selectedRect1x][selectedRect1y].x);
	int dy = (playGrid[selectedRect2x][selectedRect2y].y - playGrid[selectedRect1x][selectedRect1y].y);
	if((dy>=0 && dx >=0) || (dy<0 && dx<0))
	    {
	    glColor4f(0.6,0.1,0.1,0.2);
	SelectIntersectedCells(playGrid[selectedRect1x][selectedRect1y].x + cell_width,playGrid[selectedRect1x][selectedRect1y].y, playGrid[selectedRect2x][selectedRect2y].x + cell_width,playGrid[selectedRect2x][selectedRect2y].y,true);
	glColor4f(0.3,0.3,0.7,0.2);
	SelectIntersectedCells(playGrid[selectedRect2x][selectedRect2y].x, playGrid[selectedRect2x][selectedRect2y].y + cell_height, playGrid[selectedRect1x][selectedRect1y].x, playGrid[selectedRect1x][selectedRect1y].y + cell_height,false);
	
	glColor3ub(20,10,50);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2i(playGrid[selectedRect1x][selectedRect1y].x + cell_width,playGrid[selectedRect1x][selectedRect1y].y );
	glVertex2i(playGrid[selectedRect2x][selectedRect2y].x + cell_width,playGrid[selectedRect2x][selectedRect2y].y);
	glVertex2i(playGrid[selectedRect2x][selectedRect2y].x, playGrid[selectedRect2x][selectedRect2y].y + cell_height);
	glVertex2i(playGrid[selectedRect1x][selectedRect1y].x, playGrid[selectedRect1x][selectedRect1y].y + cell_height);
	glEnd();
	    }
	else
	    {
	    glColor4f(0.6,0.1,0.1,0.2);
	    SelectIntersectedCells(playGrid[selectedRect1x][selectedRect1y].x, playGrid[selectedRect1x][selectedRect1y].y, playGrid[selectedRect2x][selectedRect2y].x,playGrid[selectedRect2x][selectedRect2y].y, true);
	    glColor4f(0.3,0.3,0.7,0.2);
	    SelectIntersectedCells(playGrid[selectedRect2x][selectedRect2y].x + cell_width, playGrid[selectedRect2x][selectedRect2y].y + cell_height, playGrid[selectedRect1x][selectedRect1y].x + cell_width, playGrid[selectedRect1x][selectedRect1y].y + cell_height, false);
	    
	    glColor3ub(20,10,50);
	    glLineWidth(2);
	    glBegin(GL_LINE_LOOP);
	    glVertex2i(playGrid[selectedRect1x][selectedRect1y].x, playGrid[selectedRect1x][selectedRect1y].y);
	    glVertex2i(playGrid[selectedRect2x][selectedRect2y].x, playGrid[selectedRect2x][selectedRect2y].y);
	    glVertex2i(playGrid[selectedRect2x][selectedRect2y].x + cell_width, playGrid[selectedRect2x][selectedRect2y].y + cell_height);
	    glVertex2i(playGrid[selectedRect1x][selectedRect1y].x + cell_width, playGrid[selectedRect1x][selectedRect1y].y + cell_height);
	    glEnd();
	    }
	}

       //draw Grid
    glColor3ub(20,20,100);
    glLineWidth(2);
    glBegin(GL_LINES);
    for(int i=0;i<=999000;i+=1000)
	{
	glVertex2f(i,0);
	glVertex2f(i,1e6);
	}
    for(int j=0;j<=999000;j+=1000)
	{
	glVertex2f(0,j);
	glVertex2f(1e6,j);
	}
    glEnd();
    glLineWidth(1);

    if(highlightDiscs==true || deleteDiscs==true)
	{
	discID=createDL();
	deleteDiscs=false;
	highlightDiscs=false;
	}
    //Draw discs
    glCallList(discID);

    /* draw X at center of field */
    /*glLineWidth(1);
    const int SIZE=WIDTH/25;
    glColor3ub(255,0,0);
    glBegin(GL_LINE_LOOP);
	glVertex2i(CENTER_X-SIZE,CENTER_Y+SIZE);
	glVertex2i(CENTER_X+SIZE,CENTER_Y+SIZE);

	glVertex2i(CENTER_X-SIZE,CENTER_Y-SIZE);
	glVertex2i(CENTER_X+SIZE,CENTER_Y-SIZE);
    glEnd();*/

    /* draw dot */
    glPointSize(2.0f);
    glBegin(GL_POINTS);
	glColor3ub(255,0,255);
	glVertex2i(ZJW_point.x,ZJW_point.y);
    glEnd();
    //***************My Code*******************
   
    }
void MyPanZoomWindow::SelectIntersectedCells(int p1x, int p1y, int p2x, int p2y, bool bline)
{
    int F, x, y;

    if (p1x > p2x)  // Swap points if p1 is on the right of p2
    {
        swap(p1x, p2x);
        swap(p1y, p2y);
    }

    // Handle trivial cases separately
    //case 1: Vertical line
    if (p1x == p2x)
    {
        if (p1y > p2y)  // Swap y-coordinates if p1 is above p2
        {
            swap(p1y, p2y);
        }

        x = p1x;
        y = p1y;
        while (y <= p2y)
        {
            setPixel(x, y, p1x,p1y,p2x,p2y, bline);
            y++;
        }
        return;
    }
    // Horizontal line
    else if (p1y == p2y)
    {
        x = p1x;
        y = p1y;

        while (x <= p2x)
        {
            setPixel(x, y, p1x, p1y, p2x,p2y, bline);
            x++;
        }
        return;
    }

    int dy            = p2y - p1y;  // y-increment from p1 to p2
    int dx            = p2x - p1x;  // x-increment from p1 to p2
    int dy2           = 2*dy;
    int dx2           = 2*dx;

    if (dy >= 0)    // m >= 0
    {
        // Case 1: 0 <= m <= 1 (Original case)
        if (dy <= dx)   
        {
            F = dy - dx;    // initial F

            x = p1x;
            y = p1y;
            while (x < p2x)
            {
		//x++;
		//y++;
                 setPixel(x, y, p1x, p1y, p2x,p2y, bline);


                if (F > 0)
                {   
		    y+=1000;
                    F = F - dx;
                }
                else if(F < 0)
                {
                    x+=1000;
                    F = F + dy;
                }
		else
                {
                    y+=1000;
		    x+=1000;
                    F = F + dy - dx;
                }
		
            }
        }
        // Case 2: 1 < m < INF (Mirror about y=x line
        // replace all dy by dx and dx by dy)
        else
        {
            F = dx - dy;    // initial F

            y = p1y;
            x = p1x;
            while (y < p2y)
            {
                 setPixel(x, y, p1x, p1y, p2x,p2y, bline);
                if (F > 0)
                {
		    x+=1000;
                    F -= dy;
                }
               else if(F<0)
                {
                    y+=1000;
                    F += dx;
                }
                else
		    {
		    x+=1000;
		    y+=1000;
		    F = F + dx - dy;
		    }
            }
        }
    }
    else    // m < 0
    {
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy)
        {
            F = -dy - dx;    // initial F

            x = p1x;
            y = p1y;
            while (x < p2x)
            {
		//setPixel(x, y);
                if (F < 0)
                {
		    x+=1;
		     setPixel(x, y, p1x, p1y, p2x,p2y, bline);
                    F = F - dy;
                }
                else if(F>0)
                {
                    y-=1;
		     setPixel(x, y, p1x, p1y, p2x,p2y, bline);
                    F = F - dx;
                }
		else
		{
		    
		    x+=1;
		    y-=1;
		    if(x%1000!=0 && y%1000!=0)
			 setPixel(x, y, p1x, p1y, p2x,p2y, bline);
		    F = F - dy - dx;
		}
		
            }
        }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror 
        // about y=x line, replace all dx by -dy and dy by dx)
        else    
        {
            F = dx + dy;    // initial F

            y = p1y;
            x = p1x;
            while (y > p2y)
            {
	        //setPixel(x,y);
                if (F < 0)
                {
		    y-=1;
		      setPixel(x, y, p1x, p1y, p2x,p2y, bline);
                    F += dx;
                }
                else if(F>0)
                {
                    x+=1;
		      setPixel(x, y, p1x, p1y, p2x,p2y, bline);
                    F += dy;
                }
		else
		{
		 setPixel(x, y, p1x, p1y, p2x,p2y, bline);
                y-=1;
		x+=1;
		
		F = F + dy + dx;
		}
		
            }

        }
    }
}
void MyPanZoomWindow::setPixel(int px, int py, int x1, int y1, int x2, int y2, bool bline)
{
    int x=px-px%1000;
    int y=py-py%1000;
    glBegin(GL_POLYGON);
    glVertex3i(x,y,1);
    glVertex3i(x + cell_width, y,1);
    glVertex3i(x + cell_width, y + cell_height,1);
    glVertex3i(x, y + cell_height,1);
    glEnd();
    playGrid[x/1000][y/1000].selected=true;
    int gx=x/1000;
    int gy=y/1000;
    int i=0;
    if(playGrid[gx][gy].noDiscs!=0)
	{
      if(highlightDiscs==true)
	   { 	
	    for(i=0; i<playGrid[gx][gy].noDiscs; i++)
		{
		playGrid[gx][gy].containedDiscs[i]->rvalue=0.858824;
		playGrid[gx][gy].containedDiscs[i]->gvalue=0.439216;
		playGrid[gx][gy].containedDiscs[i]->bvalue=0.858824;
		}
	    }
      else
	  {	for(i=0; i<playGrid[gx][gy].noDiscs; i++)
		{
		playGrid[gx][gy].containedDiscs[i]->rvalue=0.2;
		playGrid[gx][gy].containedDiscs[i]->gvalue=0.8;
		playGrid[gx][gy].containedDiscs[i]->bvalue=0.2;
		}
	  }
	if(deleteDiscs==true)
	    {
	    int n = playGrid[gx][gy].noDiscs;
	    for(int i=0; i < n; i++)
		{
		DeleteIntersectedDiscs(x1,y1,x2,y2,playGrid[gx][gy].containedDiscs[i], bline);
		playGrid[gx][gy].noDiscs -= 1;
		}

	    }
	}
}

void MyPanZoomWindow::DeleteIntersectedDiscs(int x1,int y1,int x2,int y2,Disc * containedDiscs, bool bline)
    {
    int dy=y2-y1;
    int dx=x2-x1;
    float m= (float)dy/(float)dx;
    float dist;
	//Calculate the distance between the center of the disc and the line
	dist = abs(dy*(containedDiscs->x - x1) - dx*(containedDiscs->y - y1))/sqrt((float)(dx*dx) + (dy*dy));	

	if((dist-disc_radius)<=0 )	   //Disc Intersects the line
	    {
	    containedDiscs->x=0;
	    containedDiscs->y=0;
	    }
	else
	    {
	    float f = m*(containedDiscs->x - x1) - (containedDiscs->y - y1);
	    if(m>0)				    //if slope is positive
		{
		if(!bline)
		    f=f*-1;
		if( f < 0 )			    //Discs lies within the path rectangle
		    {
		    containedDiscs->x=0;
		    containedDiscs->y=0;
		    }
		}
	    if(m<0)				    //if slope is negative
		{
		if(bline)
		    f=f*-1;
		if( f > 0 )			    //Discs lies within the path rectangle
		    {
		    containedDiscs->x=0;
		    containedDiscs->y=0;
		    }
		}
	    }
    }

/**
\brief Drag the dot around.
*/
void MyPanZoomWindow::ZJW_mouse(int button, int state, int x, int y)
    {
	int mouse[2]={x,y};
	float tempx,tempy;
	float mouseWorld[2];
	mouseCoordinatesToWorldCoordinatesPoint(mouse,mouseWorld);
	if(firstClick==true)
	    {
	    
	    switch (button)
	    {
	
	    case GLUT_LEFT_BUTTON:
	    if (glutGetModifiers()==0)
		{
		ZJW_drag = state == GLUT_DOWN;
		}
	    else
		ZJW_drag = false;
	    break;
	    case GLUT_RIGHT_BUTTON: 
	    if(state==GLUT_DOWN)
	    {
	    firstClick=false;
	    if(firstSelect==false)
		{
	    tempx=mouseWorld[0] - (int)mouseWorld[0] % cell_width;
	    tempy=mouseWorld[1] - (int)mouseWorld[1] % cell_height;
	    firstSelect=true;
	    selectedRect1x=tempx/1000;
	    selectedRect1y=tempy/1000;    
		}
	    }
	    }
	}
	else
	    {
	    switch(button)
		{
	    case GLUT_LEFT_BUTTON:
	    if (glutGetModifiers()==0)
		{
		ZJW_drag = state == GLUT_DOWN;
		}
	    else
		ZJW_drag = false;
	    break;
	case GLUT_RIGHT_BUTTON: 
	    if(state==GLUT_DOWN)
	    {
	    if(secondSelect==false)
		{
	    tempx=mouseWorld[0] - (int)mouseWorld[0] % cell_width;
	    tempy=mouseWorld[1] - (int)mouseWorld[1] % cell_height;
	    secondSelect=true;
	    selectedRect2x=tempx/1000;
	    selectedRect2y=tempy/1000;    
		}
	    else
		secondSelect=false;
	    }
	    break;
	}
    }
    }
/**
\brief Drag the dot around.
*/
void MyPanZoomWindow::ZJW_motion(int gx, int gy)
    {
    int mouse[2]={gx,gy};
    float mouseW[2];
    mouseCoordinatesToWorldCoordinatesPoint(mouse,mouseW);
   #if 0
    cout << "mouseW: " << mouseW[0] << " " << mouseW[1] << endl;
    cout << "Mouse: " << gx << " " << gy << endl;
   #endif

    if (ZJW_drag)
	{
	ZJW_point.x = mouseW[0];
	ZJW_point.y = mouseW[1];
	}
    glutPostRedisplay();
    }


/**
\brief 'main' is the standard C/C++ main function where execution starts
*/
int main (int argc, char** argv)
    {   
    /* Initialize GLUT library */
    glutInit(&argc, argv);
    
    /* create window */    
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE); 
    glutInitWindowPosition(100,100);
    glutInitWindowSize(500, 500);
    ::panZoomWindow.glutCreateWindow("PanZoom Window Disc Collider");

    /** init ITCS4120::OpenGLTrainer classes */
    ITCS4120::OpenGLTrainer::Framebuffer::init();        
    
    /* start GLUT main loop */
    glutMainLoop();
    return 0;
    }