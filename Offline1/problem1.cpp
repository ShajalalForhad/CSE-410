
#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define pi (2*acos(0.0))
#define EPS 1e-6

int drawgrid;
int drawaxes;

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
	return scale(vect, cos(degree * (pi/180))) + scale( cross(axis, vect), sin(degree* (pi/180)));
}

point unitVector(point vect) {
	double r = sqrt(vect.x * vect.x + vect.y * vect.y + vect.z * vect.z);
	return point(vect.x/r, vect.y/r, vect.z/r);
}


// global variables
point pos;
point u, r, l;
int totalBullet;
point bullet[100];
double dof1, dof2, dof3, dof4;

void drawAxes() {
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


void drawGrid() {
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

void drawSquare(double a) {
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}

void drawBarrelHead(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	double alternateColor = 1;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		r = 2 * radius - r;
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
			// alternateColor colors
			alternateColor = 1- alternateColor;
			glColor3f(alternateColor, alternateColor, alternateColor);
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawCylinder(double radius,double height,int segments) {
    int i;
    double shade;
	int alternateColor = 1;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        // alternateColor colors
		if (alternateColor)
 	       glColor3f(0, 0, 0);
		else 
 	       glColor3f(1, 1, 1);
		alternateColor = 1 - alternateColor;

		glBegin(GL_QUADS);{
			glVertex3f(points[i].x, points[i].y, 0);
			glVertex3f(points[i+1].x, points[i+1].y, 0);
			glVertex3f(points[i].x, points[i].y, height);
			glVertex3f(points[i+1].x, points[i+1].y, height);
		}glEnd();
    }
}

void drawHemiSphere(double radius, int slices, int stacks) {
	struct point points[100][100];
	int i,j;
	double h,r;
	int alternateColor = 1;
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
	for(i=0;i<stacks;i++) {

		for(j=0;j<slices;j++) {
        // alternateColor colors
		alternateColor = 1 - alternateColor;
        glColor3f(alternateColor, alternateColor, alternateColor);
		
		glBegin(GL_QUADS);{
			glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
			glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
			glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
			glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);

		}glEnd();
		}
	}
}



void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			r = rotate(r, u, 2.0); // rotate r around u 
			// r = unitVector(r);
			l = rotate(l, u, 2.0); // rotate l around u
			// l = unitVector(l);
			break;
		case '2':
			r = rotate(r, u, -2.0); // rotate r around u by
			// r = unitVector(r);
			l = rotate(l, u, -2.0); // rotate l around u by
			// l = unitVector(l);
			break;
		case '3':
			l = rotate(l, r, 2.0); // rotate l around r by
			// r = unitVector(r);
			u = rotate(u, r, 2.0); // rotate u around r by
			// l = unitVector(l);
			break;
		case '4':
			l = rotate(l, r, -2.0); // rotate l around r by
			// r = unitVector(r);
			u = rotate(u, r, -2.0); // rotate u around r by
			// l = unitVector(l);
			break;
		case '5':
			r = rotate(r, l, 2.0); // rotate l around r by
			// r = unitVector(r);
			u = rotate(u, l, 2.0); // rotate u around r by
			// l = unitVector(l);
			break;
		case '6':
			r = rotate(r, l, -2.0); // rotate l around r by
			// r = unitVector(r);
			u = rotate(u, l, -2.0); // rotate u around r by
			// l = unitVector(l);
			break;
		case 'q':
			if (dof1 + 2.0 < 45)
				dof1 += 2.0;
			break;
		case 'w':
			if (dof1 - 2.0 > -45)
				dof1 -= 2.0;
			break;
		case 'e':
			if (dof2 + 2.0 < 45)
				dof2 += 2.0;
			break;
		case 'r':
			if (dof2 - 2.0 > -45)
				dof2 -= 2.0;
			break;
		case 'a':
			if (dof3 + 2.0 < 45)
				dof3 += 2.0;
			break;
		case 's':
			if (dof3 - 2.0 > -45)
				dof3 -= 2.0;
			break;
		case 'd':
			if (dof4 + 2.0 < 45)
				dof4 += 2.0;
			break;
		case 'f':
			if (dof4 - 2.0 > -45)
				dof4 -= 2.0;
			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos -= scale(l, 2.0);
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos += scale(l, 2.0);
			break;

		case GLUT_KEY_RIGHT:
			pos += scale(r, 2.0);
			break;
		case GLUT_KEY_LEFT:
			pos -= scale(r, 2.0);
			break;

		case GLUT_KEY_PAGE_UP:
			pos += scale(u, 2.0);
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos -= scale(u, 2.0);
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
				double  bound = atan2 (300, 600) * 180 / pi;
				if ( fabs(dof1) < bound && fabs(dof2 + dof3) < bound ) {
					bullet[totalBullet] = point(dof1, dof2, dof3);
					totalBullet++;
				}
			}
			break;

		case GLUT_RIGHT_BUTTON:
		    if(state == GLUT_DOWN){	
                drawgrid = 1 - drawgrid;
			}
			break;

		case GLUT_MIDDLE_BUTTON:
			
			break;

		default:
			break;
	}
}


void drawGun() {
	glPushMatrix();
    {
		glRotatef(90,1,0,0);
		{
			glColor3f(1,0,0);
			glRotatef(dof1,0,1,0);
			drawHemiSphere(25, 100, 20);
		}
		{
			glColor3f(1,0,0);
			glRotatef(-180 + dof2,1,0,0);
			drawHemiSphere(25, 100, 20);
		}
		{
			glColor3f(1,0,0);
			glTranslatef(0, 0, 25);
			glRotatef(-180 ,0,1,0);
			glRotatef(-dof3, 1, 0, 0);
			glRotatef(dof4, 0, 0, 1);
			glTranslatef(0, 0, -10);
			drawHemiSphere(10, 100, 40);
		}
		{
			glRotatef(-180 ,0,1,0);
			drawCylinder(10, 100, 80);
		}

		glPushMatrix();
			glTranslatef(0, 0, 100);
			glRotatef(-180 ,0,1,0);
			drawBarrelHead(10, 100, 40);
    	glPopMatrix();

	}
    glPopMatrix();
	glPushMatrix();
    {
        glRotatef(90,1,0,0);
        glTranslatef(0, 0, -600);
        glColor3f(0.6, 0.6, 0.6);
        drawSquare(300);
    }
    glPopMatrix();
    glRotatef(90,1,0,0);
	for (int idx = 0; idx < totalBullet; idx ++) {
		glPushMatrix();
		{
			glColor3f(1,0,0);
			glRotatef(bullet[idx].x,0,1,0);
			glRotatef(-180 + bullet[idx].y,1,0,0);
			glRotatef(-180 ,0,1,0);
			glRotatef(-bullet[idx].z, 1, 0, 0);
			// glRotatef(bullets[idx].dof4, 0, 0, 1);
			glTranslatef(0, 0, -598);
			drawSquare(4);
		}
		glPopMatrix();
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

	gluLookAt(pos.x, pos.y, pos.z,	pos.x + l.x, pos.y + l.y, pos.z + l.z,	u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    drawGun();


    //drawCircle(25,24);

    //drawCone(20,50,24);

	//drawSphere(25,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	pos = point(100, 100, 0);
	u = point(0, 0, 1);
	r = point(-1/sqrt(2), 1/sqrt(2), 0);
	l =  point(-1/sqrt(2), -1/sqrt(2), 0);
	totalBullet = 0;
	dof1 = 0;
	dof2 = 0;
	dof3 = 0;
	dof4 = 0;
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
