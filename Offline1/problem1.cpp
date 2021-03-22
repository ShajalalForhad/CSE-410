#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#define pi (2*acos(0.0))
#define EPS 1e-6

int drawgrid;
int drawaxes;
double angle;
double rotationConst = 2;
double moveConst = 2;

struct point {
	double x,y,z;

	point() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	point(double _x, double _y, double _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	point operator +(point another) {
		return point(x + another.x, y + another.y, z + another.z);
	}

	point operator +=(point another) {
		x += another.x, y += another.y, z += another.z;
		return point(x ,y ,z);
	}

	point operator -(point another) {
		return point(x - another.x, y - another.y, z - another.z);
	}

	point operator -=(point another) {
		x -= another.x, y -= another.y, z -= another.z;
		return point(x, y, z);
	}

};

point scale(point vect, double mul) {
	return point(vect.x * mul, vect.y * mul, vect.z * mul);
}

point cross(point vect, point another) {
	double xx = vect.y * another.z - vect.z * another.y;
	double yy = vect.z * another.x - vect.x * another.z;
	double zz = vect.x * another.y - vect.y * another.x;
	return point(xx, yy, zz);
}

point rotate(point vect, point axis, double degree) {
	return scale(vect, cos(degree * (pi/180))) + scale( cross(axis, vect), sin(degree* (pi/180))); // rotate r around u by rotationConst degree
}

point unitVector(point vect) {
	double r = sqrt(vect.x * vect.x + vect.y * vect.y + vect.z * vect.z);
	return point(vect.x/r, vect.y/r, vect.z/r);
}

// point unitVector(point p1, point p2) {
// 	double x = fabs(p1.x - p2.x) < EPS ? 0.0 : (p1.x - p2.x)/(p1.x - p2.x);
// 	double y = fabs(p1.y - p2.y) < EPS ? 0.0 : (p1.y - p2.y)/(p1.y - p2.y);
// 	double z = fabs(p1.z - p2.z) < EPS ? 0.0 : (p1.z - p2.z)/(p1.z - p2.z);
// 	return point(x, y, z);
// }

point cameraPos( 100, 100, 0);
point u(0, 0, 1), r(-1/sqrt(2), 1/sqrt(2), 0), l(-1/sqrt(2), -1/sqrt(2), 0);


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}



void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			r = rotate(r, u, rotationConst); // rotate r around u by rotationConst degree
			// r = unitVector(r);
			l = rotate(l, u, rotationConst); // rotate l around u by rotationConst degree
			// l = unitVector(l);
			break;
		case '2':
			r = rotate(r, u, -rotationConst); // rotate r around u by -rotationConst degree
			// r = unitVector(r);
			l = rotate(l, u, -rotationConst); // rotate l around u by -rotationConst degree
			// l = unitVector(l);
			break;
		case '3':
			l = rotate(l, r, rotationConst); // rotate l around r by -rotationConst degree
			// r = unitVector(r);
			u = rotate(u, r, rotationConst); // rotate u around r by -rotationConst degree
			// l = unitVector(l);
			break;
		case '4':
			l = rotate(l, r, -rotationConst); // rotate l around r by -rotationConst degree
			// r = unitVector(r);
			u = rotate(u, r, -rotationConst); // rotate u around r by -rotationConst degree
			// l = unitVector(l);
			break;
		case '5':
			r = rotate(r, l, rotationConst); // rotate l around r by -rotationConst degree
			// r = unitVector(r);
			u = rotate(u, l, rotationConst); // rotate u around r by -rotationConst degree
			// l = unitVector(l);
			break;
		case '6':
			r = rotate(r, l, -rotationConst); // rotate l around r by -rotationConst degree
			// r = unitVector(r);
			u = rotate(u, l, -rotationConst); // rotate u around r by -rotationConst degree
			// l = unitVector(l);
			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraPos -= scale(l, moveConst);
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraPos += scale(l, moveConst);
			break;

		case GLUT_KEY_RIGHT:
			cameraPos += scale(r, moveConst);
			break;
		case GLUT_KEY_LEFT:
			cameraPos -= scale(r, moveConst);
			break;

		case GLUT_KEY_PAGE_UP:
			cameraPos += scale(u, moveConst);
			break;
		case GLUT_KEY_PAGE_DOWN:
			cameraPos -= scale(u, moveConst);
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			}
			break;

		case GLUT_RIGHT_BUTTON:
		    if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
                drawgrid = 1 - drawgrid;
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes = 1 - drawaxes;
			}
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,	cameraPos.x + l.x, cameraPos.y + l.y, cameraPos.z + l.z,	u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    glColor3f(1,0,0);
    drawSquare(10);


    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
