
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

struct point {
	double x,y;

	point() {
		x = 0.0;
		y = 0.0;
	}

	point(double _x, double _y) {
		x = _x;
		y = _y;
	}

	point operator +(point another) {
		return point(x + another.x, y + another.y);
	}

	point operator +=(point another) {
		x += another.x, y += another.y;
		return point(x ,y);
	}

	point operator -(point another) {
		return point(x - another.x, y - another.y);
	}

	point operator -(){
		return point(-x, -y);
	}

	point operator -=(point another) {
		x -= another.x, y -= another.y;
		return point(x, y);
	}

};

double value(point vect)  {
	return sqrt(vect.x * vect.x + vect.y * vect.y);
}

point scale(point vect, double mul) {
	return point(vect.x * mul, vect.y * mul);
}

double dot(point vect, point other) {
	return vect.x * other.x + vect.y * other.y;
}

point rotate(point vect, double degree) {
	point ans;
	ans.x = cos(degree * (pi/180)) * vect.x - sin(degree * (pi/180))* vect.y;
	ans.y = sin(degree * (pi/180)) * vect.x + cos(degree * (pi/180))* vect.y;
	return ans;
}

point unitVector(point vect) {
	double r = sqrt(vect.x * vect.x + vect.y * vect.y);
	return point(vect.x/r, vect.y/r);
}

point projection(point vect, point on) {
	return scale(unitVector(on), dot(vect, on)/value(on));
	// self * cos(0) = (self dot on)/|on| * unit(on)
}

point perpendicular(point vect) {
	return point(vect.y, -vect.x);
}

// global variables
#define bubbleCount 6
bool inside[bubbleCount];
point bubbles[bubbleCount], v[bubbleCount];
bool isPaused;
double bubbleSpeed, increaseSpeed;


void drawSquare(double a) {
	glBegin(GL_LINES);
        glVertex2f(a,a);
        glVertex2f(a,-a);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(a,-a);
        glVertex2f(-a,-a);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(-a,-a);
        glVertex2f(-a,a);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(-a,a);
        glVertex2f(a,a);
    glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
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


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'p':
			isPaused = !isPaused;
			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_UP:		// up arrow key
			if (bubbleSpeed + .1 < 3)
				bubbleSpeed += .1;
			break;
		case GLUT_KEY_DOWN:		//down arrow key
			if (bubbleSpeed - .1 > .1)
				bubbleSpeed -= .1;
			break;

		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
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
			break;

		case GLUT_RIGHT_BUTTON:
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
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

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	glColor3f(0, 1, 0);
    drawSquare(120);
	glColor3f(1, 0, 0);
	// red circle draw
    drawCircle(60, 50);
	// bubbles draw
	for(int i = 0; i < bubbleCount; i++) {
       	glPushMatrix();
        {	
			glColor3f(0.5, 0.5, 0.5);
           	glTranslatef(bubbles[i].x, bubbles[i].y, 0);
           	drawCircle(10, 30);
        }
       	glPopMatrix();
    }
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void bubbleCollision(int i) {
	for(int other = 0; other < bubbleCount; other++) {
		if ( other == i ) continue;
		if (!inside[other]) continue;
		point dist = bubbles[i] - bubbles[other];
		point after1 = bubbles[i] + scale(v[i], 10);
		point after2 = bubbles[other] + scale(v[other], 10);
		point distAfter = after1 - after2;
		// deciding if two bubbles are going towards each other or getting out
		if( value(distAfter) > 2 * 10 ) continue;

        if( 2 * 10 - value(dist) < 0.1 ) {
			// printf("bichite bichite dhakka!");

			// calculating velocity using law of conservation of momentum
            point c1c2 = bubbles[i] - bubbles[other];
            point c1c2Perp = perpendicular(c1c2);
            point v1Hor = projection(v[i], c1c2);
            point v1Ver = projection(v[i], c1c2Perp);

			point c2c1 = bubbles[other] - bubbles[i];
            point c2c1Perp = perpendicular(c2c1);
            point v2Hor = projection(v[other], c2c1);
            point v2Ver = projection(v[other], c2c1Perp);

			// just taking the direction keeping the original value
			// as original velocity stays
			point v1 = v2Hor + v1Ver;
			v[i] = scale(unitVector(v1), value(v[i]));
			point v2 = v1Hor + v2Ver;
			v[other] = scale(unitVector(v2), value(v[other]));

        }
    }
}

void bubbleCircleCollision(int i) {
	if( fabs(60 - 10  - value(bubbles[i]) ) < 0.1 ) {
		if( dot(bubbles[i], v[i]) < 0 ) return;
		point posVector = bubbles[i];
		point projOnPos = projection(v[i],  posVector);
		point perpProjOnPos = projection(v[i], perpendicular(posVector));
		v[i] = -projOnPos + perpProjOnPos;
	}
}

void animate(){

	// every collision logic goes here in the loop
	for(int i = 0; i < bubbleCount; i++) {

        bubbleCircleCollision(i);
		// still not inside the circle
        if(!inside[i]) {
			// updating inside array
            if( value(bubbles[i]) < 60 - 10 )
        		inside[i] = true;

			// handling  square collision
            if( 120 - fabs(bubbles[i].y) < 10 ) {
				v[i].y = -v[i].y;
			}
			if( 120 - fabs(bubbles[i].x) < 10 ) {
				v[i].x = -v[i].x;
			}
        }

        else {
			// handling bubble bubble collision
            bubbleCollision(i);

			// handling bubble circle collision
			bubbleCircleCollision(i);
        }

        if(!isPaused) {
            bubbles[i] += scale(v[i], bubbleSpeed);
        }

    }

	glutPostRedisplay();
}


void init(){
	//codes for initialization
	isPaused = false;
	bubbleSpeed = 1; 
	increaseSpeed = 0.1;

	for (int i = 0; i < bubbleCount; i ++) {
		inside[i] = false;
	}
	// set initial bubble coordinates and velocities
	point velocity(0.02, 0.02);
	for (int i = 0; i < bubbleCount; i ++) {
		bubbles[i].x = bubbles[i].y = -(120 - 10);
		v[i] = rotate(velocity, 20*i);
	}

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
