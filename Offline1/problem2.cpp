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

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double rotationConst = 2;
double moveConst = 2;
double squareLength = 120;
double bigCircleRadius = 65;
double smallCircleRadius = 10;
bool pause;
double speed = 1, increaseSpeed = 0.1;

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


#define bubbleCount 8
bool inside[bubbleCount];
point bubbles[bubbleCount], v[bubbleCount];

void setSpeedOfCircles(double speed) {
	for (int idx = 0; idx < bubbleCount; idx++) {
		v[idx].x *= speed;
		v[idx].y *= speed;
	}
}

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

void drawSmallCircles(int num) {
	for(int idx = 0; idx < num; idx++) {
       	glPushMatrix();
        {	
			glColor3f(0.7, 0.7, 0.7);
           	glTranslatef(bubbles[idx].x, bubbles[idx].y, 0);
           	drawCircle(smallCircleRadius, 30);
        }
       	glPopMatrix();
    }
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case 'p':
			pause = !pause;
			break;
		default:
			break;
	}
}

void incrementSpeed(double &speed, double speedLimit) {
	if (speed + increaseSpeed < speedLimit)
		speed += increaseSpeed;
}

void decrementSpeed(double &speed, double speedLimit) {
	if (speed - increaseSpeed > speedLimit)
		speed -= increaseSpeed;
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_UP:		// up arrow key
			incrementSpeed(speed, 10);
			break;
		case GLUT_KEY_DOWN:		//down arrow key
			decrementSpeed(speed, 0.001);
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

	// gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

	drawSmallCircles(bubbleCount);
	glColor3f(0, 1, 0);
    drawSquare(squareLength);
	glColor3f(1, 0, 0);
    drawCircle(bigCircleRadius, 50);
    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void handleSideBarCollision(int idx) {
	if( squareLength - fabs(bubbles[idx].x) < smallCircleRadius ) {
        v[idx].x = -v[idx].x;
    }

    if( squareLength - fabs(bubbles[idx].y) < smallCircleRadius ) {
        v[idx].y = -v[idx].y;
    }
}

void handleInsideBigCircle(int idx) {
	 if( value(bubbles[idx]) < bigCircleRadius - smallCircleRadius )
        inside[idx] = true;
}

void handleSmallCircleCollision(int idx) {
	
	for(int other = 0; other < bubbleCount; other++) {
		if ( other == idx ) continue;
		if (!inside[other]) continue;
		point dist = bubbles[idx] - bubbles[other];
		point after1 = bubbles[idx] + scale(v[idx], 10);
		point after2 = bubbles[other] + scale(v[other], 10);
		point distAfter = after1 - after2;
		if( value(distAfter) > 2 * smallCircleRadius ) continue;
        if( 2 * smallCircleRadius - value(dist) < 0.1 ) {
			// printf("bichite bichite dhakka!");
            point r1r2 = bubbles[idx] - bubbles[other];
            point r1r2Perp = perpendicular(r1r2);
            point v1Hor = projection(v[idx], r1r2);
            point v1Ver = projection(v[idx], r1r2Perp);

			point r2r1 = bubbles[other] - bubbles[idx];
            point r2r1Perp = perpendicular(r2r1);
            point v2Hor = projection(v[other], r2r1);
            point v2Ver = projection(v[other], r2r1Perp);

			// with conservasion of momentum
            // v[idx] = v2Hor + v1Ver;
            // v[other] = v1Hor + v2Ver;

			// without conservasion of momentum
			point newV1 = v2Hor + v1Ver;
			v[idx] = scale(unitVector(newV1), value(v[idx]));
			point newV2 = v1Hor + v2Ver;
			v[other] = scale(unitVector(newV2), value(v[other]));

        }
    }
}

void handleSmallBigCircleCollition(int idx) {
	if( fabs(bigCircleRadius - smallCircleRadius  - value(bubbles[idx]) ) < 0.1 ) {
		if( dot(bubbles[idx], v[idx]) < 0 ) return;
		point posVector = bubbles[idx];
		point projOnPos = -projection(v[idx],  posVector);
		point perpProjOnPos = projection(v[idx], perpendicular(posVector));
		v[idx] = projOnPos + perpProjOnPos;
	}
}

void animate(){
	angle+=0.01;

	for(int idx = 0; idx < bubbleCount; idx++) {

        handleSmallBigCircleCollition(idx);
        if(!inside[idx]) {
            handleInsideBigCircle(idx);
            handleSideBarCollision(idx);
        }

        else {
            handleSmallCircleCollision(idx);
			handleSmallBigCircleCollition(idx);
        }

        if(!pause) {
            bubbles[idx] += scale(v[idx], speed);
        }

    }

	glutPostRedisplay();
}

void initSmallCircles() {
	point init(0.02, 0.01);
	for (int idx = 0; idx < bubbleCount; idx ++) {
		bubbles[idx].x = bubbles[idx].y = -(squareLength - smallCircleRadius);
		v[idx] = rotate(init, 20*idx);
		inside[idx] = false;
	}
}

void init(){
	//codes for initialization
	drawgrid = 0;
	drawaxes = 0;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;
	pause = false;

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
	initSmallCircles();
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
