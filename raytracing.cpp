#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iomanip>
#include<fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <GL/glut.h>
#include <vector>
#include <assert.h>
using namespace std;

#define pi (2*acos(0.0))

class Color{
public:
	double r, g, b;
	/*Color(double r=0.0, double g=0.0, double b=0.0){
	this->r = r;
	this->g = g;
	this->b = b;
	}*/
	friend ostream &operator<<(ostream &output, const Color &col)
	{
		output << "Color (" << col.r << "\t " << col.g << "\t " << col.b << ")";
		return output;
	}
};

class Vector {
public:
	double x, y, z;
	Vector(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	//pass in a vector, pass in a scalar, return the product
	friend Vector operator*(float num, Vector const &vec)
	{
		return Vector(vec.x * num, vec.y * num, vec.z * num);
	}

	Vector operator*(const double& scalar) const
	{
		return Vector(x*scalar, y*scalar, z*scalar);
	}
	Vector operator+(const Vector &v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector operator-(const Vector &v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	double magnitude(const Vector &v)
	{
		return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	}
	Vector normal(const Vector &v)
	{
		double mag = magnitude(v);
		return Vector(v.x / mag, v.y / mag, v.z / mag);
	}

	/* a is the angle in degree, r is the axis to rotate against */
	Vector rotate(double a, Vector r) // 
	{
		Vector i(*this);
		Vector u = r.cross(i);
		i = i * cosf(a*pi / 180);
		u = u * sinf(a*pi / 180);
		return i + u;
	}
	double dot(const Vector &vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}
	Vector cross(const Vector &vec) const
	{
		return Vector(y * vec.z - z * vec.y,
			z * vec.x - x * vec.z,
			x * vec.y - y * vec.x);
	}
	friend ostream &operator<<(ostream &output, const Vector &vec)
	{
		output << "(" << vec.x << "\t " << vec.y << "\t " << vec.z << ")";
		return output;
	}
};

class Point {
public:
	double x, y, z;
	Point(double x = 0.0, double y = 0.0, double z = 0.0) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Point operator+(const Vector &v) const
	{
		return Point(x + v.x, y + v.y, z + v.z);
	}
	Point operator-(const Vector &v) const
	{
		return Point(x - v.x, y - v.y, z - v.z);
	}

	Vector operator-(const Point &b) const
	{
		return Vector(x - b.x, y - b.y, z - b.z);
	}
	friend ostream &operator<<(ostream &output, const Point &p)
	{
		double prec = output.precision();
		//output << removeNegZero(p.x, prec) << " " << removeNegZero(p.y, prec) << " " << removeNegZero(p.z, prec);
		output << (p.x) << " " << (p.y) << " " << (p.z);
		return output;
	}
};
// -------------------------------------------- //



/*scene related*/
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

/** ---- CAMERA CONTROL related ---- **/

/* speed */
double ROTATE_SPEED = 3; // angle in degree

/* position and up, right, look direction */
Point pos(100, 100, 0);
Vector u(0, 0, 1), r(-1 / sqrt(2.0), 1 / sqrt(2.0), 0), l(-1 / sqrt(2.0), -1 / sqrt(2.0), 0);
// -------------------------------------------- //


void drawAxes()
{
	if (drawaxes == 1)
	{
		glBegin(GL_LINES); {
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(100, 0, 0);
			glVertex3f(-100, 0, 0);

			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0, -100, 0);
			glVertex3f(0, 100, 0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0, 0, 100);
			glVertex3f(0, 0, -100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if (drawgrid == 1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES); {
			for (i = -8; i <= 8; i++){

				if (i == 0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i * 10, -90, 0);
				glVertex3f(i * 10, 90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i * 10, 0);
				glVertex3f(90, i * 10, 0);
			}
		}glEnd();
	}
}


void myDraw(){
	glPushMatrix(); {
		glColor3f(3, 1, 1);
		glTranslatef(0, 0, 0); //move to axis center
		glutSolidSphere(10, 100, 100);
	}
	glPopMatrix();
}

void printCamera(){
	ofstream cameraPosition;
	cameraPosition.open("camera.txt");
	cameraPosition << l << endl;
	cameraPosition << pos << endl;
	cameraPosition.close();
}

void keyboardListener(unsigned char key, int x, int y){
	switch (key){
		// rotate/ look right
	case '1':
		l = l.rotate(ROTATE_SPEED, u);
		r = r.rotate(ROTATE_SPEED, u);
		break;

		// rotate/look left
	case '2':
		l = l.rotate(-ROTATE_SPEED, u);
		r = r.rotate(-ROTATE_SPEED, u);
		break;

		//look up
	case '3':
		l = l.rotate(ROTATE_SPEED, r);
		u = u.rotate(ROTATE_SPEED, r);
		break;

		//look down
	case '4':
		l = l.rotate(-ROTATE_SPEED, r);
		u = u.rotate(-ROTATE_SPEED, r);
		break;

		// tilt clockwise
	case '5':
		r = r.rotate(-ROTATE_SPEED, l);
		u = u.rotate(-ROTATE_SPEED, l);
		break;

		// tilt counterclockwise
	case '6':
		r = r.rotate(ROTATE_SPEED, l);
		u = u.rotate(ROTATE_SPEED, l);
		break;
	case '0':
		printCamera();
		break;
	case 'g':
		drawgrid = 1 - drawgrid;
		break;

	default:
		break;
	}
}


void specialKeyListener(int key, int x, int y){
	switch (key){

		//1. Move Forward
	case GLUT_KEY_UP:		// up arrow key
		pos = pos + l;

		break;
		//2. Move Backward
	case GLUT_KEY_DOWN:		//down arrow key
		pos = pos - l;
		break;
	case GLUT_KEY_RIGHT:
		pos = pos + r * 2;
		break;
	case GLUT_KEY_LEFT:
		pos = pos - r * 2;
		break;

		//move up
	case GLUT_KEY_PAGE_UP:
		pos = pos + u;
		break;
	case GLUT_KEY_PAGE_DOWN:
		pos = pos - u;
		break;

	case GLUT_KEY_INSERT:
		break;

	default:
		break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch (button){
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			drawaxes = 1 - drawaxes;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		//........
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
	glClearColor(0, 0, 0, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH); // Enable smooth shading
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

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);

	//gluLookAt(200 * cos(cameraAngle), 200 * sin(cameraAngle), cameraHeight, 0, 0, 0, 0, 0, 1);
	//gluLookAt(0,0,70,	0,0,0,	0,1,1);

	// for fully controllable camera


	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects	
	drawAxes();
	drawGrid();
	myDraw();
	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid = 1;
	drawaxes = 1;

	//clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80, 1, 1, 1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}



class Vector4 {
public:
	double x, y, z, w;
	Vector4(double x = 0.0, double y = 0.0, double z = 0.0, double w = 0.0) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4(Vector v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = 0;
	}

	friend Vector4 operator*(double num, Vector4 const &vec)
	{
		return Vector4(vec.x * num, vec.y * num, vec.z * num);
	}

	Vector4 operator*(const double& scalar) const
	{
		return Vector4(x*scalar, y*scalar, z*scalar, w*scalar);
	}
	Vector4 operator+(const Vector4 &v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	Vector4 operator-(const Vector4 &v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	double dot(const Vector4 &vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}

	double operator*(const Vector4 &vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}

	friend ostream &operator<<(ostream &output, const Vector4 &vec)
	{
		output << "(" << vec.x << "\t " << vec.y << "\t " << vec.z << "\t " << vec.w << ")";
		return output;
	}

	Vector4 normalize() const
	{
		double ax, ay, az, aw;
		if (w != 0){
			ax = x / w;
			ay = y / w;
			az = z / w;
			aw = 1;
			return Vector4(ax, ay, az, aw);
		}
		else{
			return Vector4(x, y, z, w);
		}

	}

	Point point(){
		assert(w == 1);
		return Point(x, y, z);
	}

	Vector vector(){
		assert(w == 0);
		return Vector(x, y, z);
	}
};

class Obj{
public:
	string name;
	Color color;
	double ambient, diffuse, specular, reflection, shininess;
	bool intersect;
};
class Sphere : public Obj{
public:
	double radius;
	Vector4 center;
	/*Sphere(double radius, Vector4 center){
		this->radius = radius;
		this->center = center;
	}*/	
};
class Pyramid : public Obj{
public:
	Vector4 lowestPoint;
	double width;
	double height;
};

class Scene{
public:
	int levelRecursion;
	int numPixel;
	int numObj;
	int numLightSrc;
	vector<Obj> objects;
};
Scene s;

void inputSceneParameters(){
	ifstream sceneFileIn("description.txt");
	string line;

	if (sceneFileIn.is_open())
	{
		getline(sceneFileIn, line);		//Line 1: level of recursion
		stringstream(line) >> s.levelRecursion;
		getline(sceneFileIn, line);		//Line 2: number of pixels along both axes
		stringstream(line) >> s.numPixel;
		getline(sceneFileIn, line);		//Line 3 : blank
		getline(sceneFileIn, line);		//Line 4 : number of objects
		stringstream(line) >> s.numObj;
		getline(sceneFileIn, line);		//Line 5 : blank
		for (int i = 0; i < s.numObj; i++)
		{
			getline(sceneFileIn, line);	//0: object name
			if (line == "sphere"){
				Sphere sphere;
				getline(sceneFileIn, line);	//1. center
				stringstream(line) >> sphere.center.x >> sphere.center.y >> sphere.center.z;
				getline(sceneFileIn, line);	//2. radius		
				stringstream(line) >> sphere.radius;
				
				s.objects.push_back(sphere);
			}else if (line == "pyramid"){
				getline(sceneFileIn, line);	//1. lowest point co-ordinate
				getline(sceneFileIn, line);	//2. width, height

				Pyramid pyramid;
				s.objects.push_back(pyramid);
			}
			getline(sceneFileIn, line);	//-3: color
			getline(sceneFileIn, line);	//-2: ambient diffuse specular reflection coefficient
			getline(sceneFileIn, line);	//-1: shininess
		}

		//stringstream(line) >> s.fovY >> s.aspectRatio >> s.near >> s.far;
		getline(sceneFileIn, line);		//Line 5 : screen_width screen_height
		//stringstream(line) >> s.screenWidth >> s.screenHeight;
		getline(sceneFileIn, line);		//Line 6 : r g b
		//stringstream(line) >> s.backColor.r >> s.backColor.g >> s.backColor.b;
		sceneFileIn.close();
	}
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Assignment 1(a)");

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
