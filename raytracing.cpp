#define _SCL_SECURE_NO_WARNINGS
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
#include "bitmap_image.hpp"
using namespace std;

#define pi (2*acos(0.0))

/** -------- Epsilon check ------------ **/
double removeNegZero(double theFloat, int precision)
{
	if ((theFloat < 0.0f) &&
		(-log10(abs(theFloat)) > precision))
	{
		return -theFloat;
	}
	else
	{
		return theFloat;
	}
}
int compare(double v1, double v2, double e = 0.00001){
	v1 = removeNegZero(v1, 7);
	v2 = removeNegZero(v2, 7);

	if (abs(v1 - v2) < e){
		// v1 == v2
		return 0;
	}
	else if (v1 > v2){
		return 1;
	}
	else if (v1 < v2){
		return -1;
	}
	return -999;
}

class Vector {
public:
	double x, y, z;
	Vector(double x = 0.0, double y = 0.0, double z = 0.0) {
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
	double magnitude()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}
	Vector normalize()
	{
		double mag = magnitude();
		assert(mag != 0);
		return Vector(x / mag, y / mag, z / mag);
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
		output << vec.x << " " << vec.y << " " << vec.z;
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

/** -------- CAMERA CONTROL related ------------ **/
int drawgrid;
int drawaxes;
/* speed */
double ROTATE_SPEED = 3; // angle in degree
/* position and up, right, look direction */
//Point pos(100, 100, 0);
//Vector u(0, 0, 1), r(-1 / sqrt(2.0), 1 / sqrt(2.0), 0), l(-1 / sqrt(2.0), -1 / sqrt(2.0), 0);
Point pos(100, 100, 2);
Vector u(0, 0, 1), l(-1 / sqrt(2.0), -1 / sqrt(2.0), 0), r(-1 / sqrt(2.0), 1 / sqrt(2.0), 0);
// -------------------------------------------- //

/** ---------- RAY TRACE related -------------- **/
bitmap_image image;
void raytraceMain();
// -------------------------------------------- //

class Color{
public:
	double r, g, b;
	Color(double r = 0.0, double g = 0.0, double b = 0.0) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
	friend ostream &operator<<(ostream &output, const Color &col)
	{
		output << col.r << " " << col.g << " " << col.b;
		return output;
	}
};
class Intersection{
public:
	Vector normal;
	Color col;
	double t;
};
class Ray {
public:
	Point origin;
	Vector direction;
	Intersection intersection;

	Ray(Point eye, Vector dir, Color backColor) {
		origin = eye;
		direction = dir.normalize();
		intersection.col = backColor;
		intersection.t = 99999;
	}

	friend ostream &operator<<(ostream &output, const Ray &ray)
	{
		output << "ray origin = " << ray.origin << "  direction = " << ray.direction << "  t = " << ray.intersection.t;
		return output;
	}
};

class Obj{
public:
	string name;
	Color color;
	double ambient, diffuse, specular, reflection, shininess;
	bool intersect(Ray &ray) {
		return false;
	}
};
class Board : public Obj{
public:
	//plane is in point normal form
	Vector n;	//normal (unit)
	Point pO;	//point on plane

	bool intersect(Ray& ray)
	{
		// for ray: p = rO + rD * t
		// for plane: (p - pO).n = 0 

		// assuming vectors are all normalized
		assert(compare(ray.direction.magnitude(), 1) == 0);
		assert(compare(n.magnitude(), 1) == 0);


		double denom = n.dot(ray.direction);
		// if dot product is 0 then no intersection
		if (abs(denom) > 1e-6) {
			Vector diff = pO - ray.origin;
			double t = diff.dot(n) / denom;
			//double t = -ray.direction.z / pos.z;
			if (t >= 0 && t <= ray.intersection.t){
				ray.intersection.t = t;
				Point iP = ray.origin + (ray.direction * t);
				int x = iP.x;
				int y = iP.y;
				if (abs(x % 10) >= 5 && abs(y % 10) >= 5 || abs(x % 10)  < 5 && abs(y % 10)  < 5){
					ray.intersection.col = Color(0, 0, 0);
				}
				else{
					ray.intersection.col = Color(1.0, 1.0, 1.0);
				}
				
				return true;
				//ray.intersection.obj = *this;
			}
		}

		return false;
	}
};
class Sphere : public Obj{
public:
	double radius;
	Point center;

	bool intersect(Ray &ray) {
		//float dx = center.x - ray.origin.x;
		//float dy = center.y - ray.origin.y;
		//float dz = center.z - ray.origin.z;
		Vector d = center - ray.origin;
		double v = ray.direction.dot(d);

		// Do the following quick check to see if there is even a chance
		// that an intersection here might be closer than a previous one
		//if (v - radius > ray.intersection.t){
		//	return false;
		//}

		// Test if the ray actually intersects the sphere
		double t = radius * radius + v * v - d.x * d.x - d.y * d.y - d.z * d.z;
		if (t < 0){
			return false;
		}

		// Test if the intersection is in the positive
		// ray direction and it is the closest so far
		t = v - ((double)sqrt(t));
		if ((t > ray.intersection.t) || (t < 0)){
			return false;
		}
		ray.intersection.t = t;
		ray.intersection.col = this->color;
		return true;
	}
};
class Triangle : public Obj{
public:
	Point p0, p1, p2;
	double a, b, c;
	Vector normal;
	Triangle(Point p0, Point p1, Point p2, Color c){
		this->p0 = p0;
		this->p1 = p1;
		this->p2 = p2;
		this->color = c;
		//double denom = (p1 - p0).cross(p)
		//a =			


	}
	Triangle(){
	}
	bool intersect(Ray& ray)
	{

		//Vector e0 = p1 - p0;
		//Vector e1 = p2 - p0;
		//Vector N = e0.cross(e1);
		//N = N.normalize();
		//
		//// implementing the single/double sided feature
		//if (ray.direction.dot(N) > 0)
		//	return false; // back-facing surface



		// compute plane's normal
		//Vector p01 = p1 - p0;
		//Vector p02 = p2 - p0;
		// no need to normalize
		// simply compute the cross product of AB and AC. 
		normal = (p1 - p0).cross(p2 - p0); // N
		normal = normal.normalize();

		// Step 1: finding P
		// check if ray and plane are parallel ?
		double NdotRayDirection = normal.dot(ray.direction);
		double kEpsilon = 1e-6;
		if (fabs(NdotRayDirection) < kEpsilon) // almost 0
			return false; // they are parallel so they don't intersect !

		// compute d parameter using equation 2
		double d = normal.dot(p0 - Point(0, 0, 0));

		// compute t (equation 3)
		double t = (normal.dot(ray.origin - Point(0, 0, 0)) + d) / NdotRayDirection;
		// check if the triangle is in behind the ray
		if (t < 0) return false; // the triangle is behind

		// compute the intersection point using equation 1
		Point P = ray.origin + t * ray.direction;

		// Step 2: inside-outside test
		Vector C; // vector perpendicular to triangle's plane

		// edge 0
		Vector edge0 = p1 - p0;
		Vector Pp0 = P - p0;
		C = edge0.cross(Pp0);
		if (normal.dot(C) < 0) return false; // P is on the right side

		// edge 1
		Vector edge1 = p2 - p1;
		Vector vp1 = P - p1;
		C = edge1.cross(vp1);
		if (normal.dot(C) < 0) return false; // P is on the right side

		// edge 2
		Vector edge2 = p0 - p2;
		Vector vp2 = P - p2;
		C = edge2.cross(vp2);
		if (normal.dot(C) < 0) return false; // P is on the right side;


		ray.intersection.t = t;
		ray.intersection.col = this->color;
		return true; // this ray hits the triangle
	}
};
class PyramidBase : public Obj
{
public:
	//plane is in point normal form
	Vector n;	//normal (unit)
	Point pO;	//point on plane (lowest point)
	double width;
	PyramidBase(){}
	PyramidBase(Point pO, double width, Color c){
		this->n = Vector(0.0, 0.0, -1.0);
		this->pO = pO;
		this->color = c;
		this->width = width;
	}
	bool intersect(Ray& ray)
	{
		// assuming vectors are all normalized
		assert(compare(ray.direction.magnitude(), 1) == 0);
		assert(compare(n.magnitude(), 1) == 0);

		double denom = n.dot(ray.direction);
		// if dot product is 0 then no intersection
		if (abs(denom) > 1e-6) {
			Vector diff = pO - ray.origin;
			double t = diff.dot(n) / denom;
			if (t >= 0 && t <= ray.intersection.t){
				
				Point iP = ray.origin + (ray.direction * t);
				int x = iP.x;
				int y = iP.y;
				if (x >= pO.x && x <= pO.x + width && y >= pO.y && y <= pO.y + width){
					ray.intersection.t = t;
					ray.intersection.col = this->color;
					return true;
				}
			}
		}
		return false;
	}
};
class Pyramid : public Obj{
public:
	Point lowestPoint;
	double width;
	double height;
	Triangle t[4];
	PyramidBase base;
	Pyramid(Point lp, double w, double h, Color color, double ambient, double diffuse, double specular, double reflection, double shininess){
		lowestPoint = lp;
		width = w;
		height = h;
		Point p0(lp.x + 0, lp.y + 0, lp.z + 0); //front left vertex
		Point p1(lp.x + w, lp.y + 0, lp.z + 0); //front right vertex
		Point p2(lp.x + w, lp.y + w, lp.z + 0); //back right vertex
		Point p3(lp.x + 0, lp.y + w, lp.z + 0);//back left vertex
		Point top(lp.x + w/2.0, lp.y + w/2.0, lp.z + h); //top vertex		
		t[0] = Triangle(p0, p1, top, color);
		t[1] = Triangle(p1, p2, top, color);
		t[2] = Triangle(p2, p3, top, color);
		t[3] = Triangle(p3, p0, top, color);
		base = PyramidBase(lp, w, color);
		this->color = color;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->reflection = reflection;
		this->shininess = shininess;
	}
};

class Scene{
public:
	int levelRecursion;
	int height;
	int width;
	int numObj;
	int numLightSrc;
	double fov;
	vector<Sphere> spheres;
	vector<Pyramid> pyramids;
	Board checkerboard;
	vector<Point> lightSources;
	Color backColor;
};
Scene s;

bool trace(Ray &r) {
	//double MAX_T = 9999999999999;
	//r.intersection.t = MAX_T;
	bool foundTraced = false;
	
	for (vector<Sphere>::iterator it = s.spheres.begin(); it != s.spheres.end(); ++it){
		Sphere object = *it;
		if (object.intersect(r) == true){
			foundTraced = true;
		}
	}

	for (vector<Pyramid>::iterator it = s.pyramids.begin(); it != s.pyramids.end(); ++it){
		Pyramid p = *it;
		for (int i = 0; i < 4; i++)
		{
			if (p.t[i].intersect(r) == true){
				foundTraced = true;
			}
		}
		if (p.base.intersect(r) == true){
			foundTraced = true;
		}
	}

	Board object = s.checkerboard;
	if (object.intersect(r) == true){
		foundTraced = true;
	}

	return (foundTraced);
}

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
void drawPyramid(Pyramid& p){
	glBegin(GL_TRIANGLES); {
		for (int i = 0; i < 4; i++)
		{
			glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(p.t[i].p0.x, p.t[i].p0.y, p.t[i].p0.z);
			glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(p.t[i].p1.x, p.t[i].p1.y, p.t[i].p1.z);
			glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(p.t[i].p2.x, p.t[i].p2.y, p.t[i].p2.z);
		}
	}glEnd();

	Point lp = p.lowestPoint;
	double w = p.width;
	double h = p.height;
	Point p0(lp.x + 0, lp.y + 0, lp.z + 0); //front left vertex
	Point p1(lp.x + w, lp.y + 0, lp.z + 0); //front right vertex
	Point p2(lp.x + w, lp.y + w, lp.z + 0); //back right vertex
	Point p3(lp.x + 0, lp.y + w, lp.z + 0);//back left vertex
	Point top(lp.x + w / 2.0, lp.y + w / 2.0, lp.z + h); //top vertex
	glBegin(GL_QUADS); {
		glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(p0.x, p0.y, p0.z);
		glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(p1.x, p1.y, p1.z);
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(p2.x, p2.y, p2.z);
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(p3.x, p3.y, p3.z);
	}glEnd();
}
void drawScene(){
	for (vector<Sphere>::iterator it = s.spheres.begin(); it != s.spheres.end(); ++it){
		Sphere object = *it;
		glPushMatrix(); {
			glColor3f(object.color.r, object.color.g, object.color.b);
			glTranslatef(object.center.x, object.center.y, object.center.z); //move to axis center
			glutSolidSphere(object.radius, 100, 100);
		}
		glPopMatrix();

	}
	for (vector<Pyramid>::iterator it = s.pyramids.begin(); it != s.pyramids.end(); ++it){
		Pyramid p = *it;
		drawPyramid(p);
	}
}

void printCamera(){
	ofstream cameraPosition;
	cameraPosition.open("camera.txt");
	cameraPosition << l << endl;
	cameraPosition << pos << endl;
	cameraPosition << u << endl;
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
		raytraceMain();
		cout << "Drawn." <<endl;
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
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects	
	drawAxes();
	drawGrid();
	drawScene();
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
	gluPerspective(s.fov, 1, 1, 9999999);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

void inputSceneParameters(){
	ifstream sceneFileIn("description.txt");
	string line;

	if (sceneFileIn.is_open())
	{
		getline(sceneFileIn, line);		//Line 1: level of recursion
		stringstream(line) >> s.levelRecursion;
		getline(sceneFileIn, line);		//Line 2: number of pixels along both axes
		stringstream(line) >> s.height;
		s.width = s.height;
		getline(sceneFileIn, line);		//Line 3 : blank


		getline(sceneFileIn, line);		//Line 4 : number of objects
		stringstream(line) >> s.numObj;
		getline(sceneFileIn, line);		//Line 5 : blank
		for (int i = 0; i < s.numObj; i++)
		{
			while (1){
				getline(sceneFileIn, line);
				if (line != "\n") break;
			}
			//getline(sceneFileIn, line);	//0: object name
			if (line == "sphere"){
				Sphere o;
				o.name = "sphere";
				getline(sceneFileIn, line);	//1. center
				stringstream(line) >> o.center.x >> o.center.y >> o.center.z;
				getline(sceneFileIn, line);	//2. radius		
				stringstream(line) >> o.radius;
				getline(sceneFileIn, line);	//-3: color
				stringstream(line) >> o.color.r >> o.color.g >> o.color.b;
				getline(sceneFileIn, line);	//-2: ambient diffuse specular reflection coefficient
				stringstream(line) >> o.ambient >> o.diffuse >> o.specular >> o.reflection;
				getline(sceneFileIn, line);	//-1: shininess
				stringstream(line) >> o.shininess;
				s.spheres.push_back(o);
			}
			else if (line == "pyramid"){
				Point lowestPoint;
				Color color;
				double width, height, ambient, diffuse, specular, reflection, shininess;				
				getline(sceneFileIn, line);	//1. lowest point co-ordinate
				stringstream(line) >> lowestPoint.x >> lowestPoint.y >> lowestPoint.z;
				getline(sceneFileIn, line);	//2. width, height
				stringstream(line) >> width >> height;
				getline(sceneFileIn, line);	//-3: color
				stringstream(line) >> color.r >> color.g >> color.b;
				getline(sceneFileIn, line);	//-2: ambient diffuse specular reflection coefficient
				stringstream(line) >> ambient >> diffuse >> specular >> reflection;
				getline(sceneFileIn, line);	//-1: shininess
				stringstream(line) >> shininess;
				Pyramid o(lowestPoint, width, height, color, ambient, diffuse, specular, reflection, shininess);
				o.name = "pyramid";
				s.pyramids.push_back(o);
			}
			getline(sceneFileIn, line);	//blank
		}
		getline(sceneFileIn, line); //number of light sources	
		stringstream(line) >> s.numLightSrc;
		for (int i = 0; i < s.numLightSrc; i++)
		{
			Point p;
			getline(sceneFileIn, line); //pos
			stringstream(line) >> p.x >> p.y >> p.z;
			s.lightSources.push_back(p);
		}
		sceneFileIn.close();
	}
	s.backColor.r = 0;
	s.backColor.g = 0;
	s.backColor.b = 0;

	s.checkerboard.n = Vector(0.0, 0.0, -1.0);
	s.checkerboard.pO = Point(1.0, 1.0, 0.0);
	s.fov = 45;
}
void raytraceMain(){
	int rows = s.height;
	int columns = s.width;
	Color** pixelBuffer;
	pixelBuffer = new Color*[rows];
	for (int j = 0; j < rows; j++)
	{
		pixelBuffer[j] = new Color[columns];
	}
	ifstream sceneFileIn("camera.txt");
	Vector l, u;
	Point eye;
	if (sceneFileIn.is_open())
	{
		string line;
		getline(sceneFileIn, line);	//look direction
		stringstream(line) >> l.x >> l.y >> l.z;
		getline(sceneFileIn, line); //eye position
		stringstream(line) >> eye.x >> eye.y >> eye.z;
		getline(sceneFileIn, line); //up direction
		stringstream(line) >> u.x >> u.y >> u.z;
		sceneFileIn.close();
	}

	// Compute viewing matrix that maps a
	// screen coordinate to a ray direction
	Vector Du = l.cross(u).normalize();
	Vector Dv = l.cross(Du).normalize();
	double fov = s.fov;
	double fl = (double)(s.width / (2 * tan((0.5*fov)*pi / 180.0)));
	Vector Vp = l.normalize();
	Vp.x = Vp.x*fl - 0.5f*(s.width*Du.x + s.height*Dv.x);
	Vp.y = Vp.y*fl - 0.5f*(s.width*Du.y + s.height*Dv.y);
	Vp.z = Vp.z*fl - 0.5f*(s.width*Du.z + s.height*Dv.z);

	for (int j = 0; j < s.height; j++) {
		for (int i = 0; i < s.width; i++) {
			Vector dir = Vector(i*Du.x + j*Dv.x + Vp.x, i*Du.y + j*Dv.y + Vp.y, i*Du.z + j*Dv.z + Vp.z);
			Ray ray(eye, dir, s.backColor);
			trace(ray);
			pixelBuffer[j][i] = ray.intersection.col;
		}
	}

	//draw output to bmp file
	image = bitmap_image(s.width, s.height);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			unsigned char r, g, b;
			r = (unsigned char)(pixelBuffer[i][j].r * 255);
			g = (unsigned char)(pixelBuffer[i][j].g * 255);
			b = (unsigned char)(pixelBuffer[i][j].b * 255);
			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;

			image.set_pixel(j, i, r, g, b);
		}
	}
	image.save_image("out.bmp");
}

int main(int argc, char **argv){
	inputSceneParameters();

	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Raytracing");

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
