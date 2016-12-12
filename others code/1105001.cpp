#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include<fstream>
#include<iostream>
#include "bitmap_image.hpp"

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#define tangent (0.4142135624)

using namespace std;

int textTure = 0;

class Vector
{
public:
    double x, y, z;

    Vector(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector() {
    }

    void normalize()
    {
        double r = sqrt(x*x + y*y + z*z);
        x = x / r;
        y = y / r;
        z = z / r;
    }

    double magnitude() {
        double r = sqrt(x*x + y*y + z*z);
        return r;
    }

    Vector operator+(const Vector& v)
    {
        Vector v1(x+v.x, y+v.y, z+v.z);
        return v1;
    }

    Vector operator-(const Vector& v)
    {
        Vector v1(x-v.x, y-v.y, z-v.z);
        return v1;
    }

    Vector operator* (double m)
    {
        Vector v(x*m, y*m, z*m);
        return v;
    }

    static double dot(Vector a, Vector b)
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static Vector cross(Vector a, Vector b)
    {
        Vector v(a.y*b.z - a.z*b.y, b.x*a.z - b.z*a.x, a.x*b.y - a.y*b.x);
        return v;
    }

    void print ()
    {
        cout << "Vector" << endl;
        cout << x << " " << y << " " << z << endl;
    }
};

class point
{
public:
	double x,y,z;
	point () {
	}
	point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
	}
	Vector operator-(const point& p) {
        Vector v(this->x-p.x, this->y-p.y, this->z-p.z);
        return v;
	}
	point operator+(const Vector& v) {
        point p(x+v.x, y+v.y, z+v.z);
        return p;
	}
};

void drawSphere(double radius,int slices,int stacks, double R, double G, double B)
{
    glColor3f(R, G, B);
	point points[100][100];
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

void drawPyramid(double x0,  double y0, double z0, double len, double height, double r, double g, double b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(x0, y0, z0);
        glVertex3f(x0+len, y0, z0);
        glVertex3f(x0+len/2.0, y0+len/2.0, z0+height);
        glVertex3f(x0+len, y0, z0);
        glVertex3f(x0+len, y0+len, z0);
        glVertex3f(x0+len/2.0, y0+len/2.0, z0+height);
        glVertex3f(x0+len, y0+len, z0);
        glVertex3f(x0, y0+len, z0);
        glVertex3f(x0+len/2.0, y0+len/2.0, z0+height);
        glVertex3f(x0, y0+len, z0);
        glVertex3f(x0, y0, z0);
        glVertex3f(x0+len/2.0, y0+len/2.0, z0+height);
    }
    glEnd();
    glBegin(GL_QUADS);
    {
        glVertex3f(x0, y0, z0);
        glVertex3f(x0+len, y0, z0);
        glVertex3f(x0+len, y0+len, z0);
        glVertex3f(x0, y0+len, z0);
    }
    glEnd();
}

class Sphere
{
public:
    double radius;
    double x, y, z;
    double r, g, b;
    double ambient, diffuse, specular, reflection, shininess;
    Sphere() {
    }
    Sphere(double radius, double x, double y, double z, double r, double g, double b) {
        this->radius = radius;
        this->x = x;
        this->y = y;
        this->z = z;
        this->r = r;
        this->g = g;
        this->b = b;
    }
    void draw() {
        glPushMatrix();
        glTranslated(x, y, z);
        drawSphere(radius, 20, 20, r, g, b);
        glPopMatrix();
    }
    void setProperties(double ambient, double diffuse, double specular, double reflection, double shininess) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->shininess = shininess;
        this->reflection = reflection;
    }
};

class Pyramid
{
public:
    double x, y, z;
    double len;
    double height;
    double r, g, b;
    double ambient, specular, diffuse, shininess, reflection;
    Pyramid () {
    }
    Pyramid (double x, double y, double z, double len, double height, double r, double g, double b) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->r = r;
        this->g = g;
        this->b = b;
        this->len = len;
        this->height = height;
    }
    void draw() {
        drawPyramid(x, y, z, len, height, r, g, b);
    }
    void setProperties(double ambient, double diffuse, double specular, double reflection, double shininess) {
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->shininess = shininess;
        this->reflection = reflection;
    }
};

class LightSource
{
public:
    double x, y, z;
    LightSource () {
    }
    LightSource (double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    void draw() {
        glPushMatrix();
        glTranslated(x, y, z);
        drawSphere(5, 10, 10, 1, 1, 1);
        glPopMatrix();
    }
};

class Color {
public:
    double r, g, b;
    Color(double r, double g, double b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    Color() {
    }
};


Vector l(-1/sqrt(2.0), -1/sqrt(2.0), 0);
Vector r(-1/sqrt(2.0), 1/sqrt(2.0), 0);
Vector u(0, 0, 1);

point camera_position;

vector <Sphere> sphereVector;
vector <Pyramid> pyramidVector;
vector <LightSource> lightSourceVector;

int depthOfTracing;
int screenSize;
int nearDistance = 1;

Color **textureBuffer;

bool bln = true;
bitmap_image b_img ("texture.bmp");

unsigned int height, width;

bool reachesSource (point start, point nextToStart, LightSource source) {

    point sourcePos (source.x, source.y, source.z);
    Vector vv = sourcePos - start;
    Vector vvv = nextToStart - start;
    double tToCheck = vv.magnitude() / vvv.magnitude();

    double *tDepths = new double [1 + sphereVector.size() + pyramidVector.size()];
    for (int k = 0; k < 1 + sphereVector.size() + pyramidVector.size(); k++) {
        tDepths[k] = -1;
    }

    int xindex, yindex;
    int *whichOne = new int [pyramidVector.size()];

    Vector** normals = new Vector* [pyramidVector.size()];
    for (int k = 0; k < pyramidVector.size(); k++) {
        normals[k] = new Vector [5];
    }

    if (nextToStart.z != start.z) {
        double t = start.z / (start.z - nextToStart.z);
        if (t < 0) {
            ;
        } else {
            double X = start.x + t * (nextToStart.x - start.x);
            double Y = start.y + t * (nextToStart.y - start.y);
            //double distance = sqrt ( pow (X - camera_position.x, 2) + pow (Y - camera_position.y, 2) + pow (camera_position.z, 2) );
            //if (distance > farDistance) {
                //continue;
            //}

            if (X < 0) X = -X;
            if (Y < 0) Y = -Y;

            xindex = floor(X/30.0);
            yindex = floor(Y/30.0);

            tDepths[0] = t;
            // this part is rendering, requires xindex, yindex
            /**/
        }
    }

    // drawing Sphere
    double dx = nextToStart.x - start.x;
    double dy = nextToStart.y - start.y;
    double dz = nextToStart.z - start.z;

    for (int k = 0; k < sphereVector.size(); k++) {
        Sphere s = sphereVector[k];

        double a = dx*dx + dy*dy + dz*dz;
        double b = 2.0*dx*(start.x - s.x) + 2.0*dy*(start.y - s.y) + 2.0*dz*(start.z - s.z);
        double c = s.x*s.x + s.y*s.y + s.z*s.z + start.x*start.x + start.y*start.y
                + start.z*start.z - 2.0*(s.x*start.x + s.y*start.y + s.z*start.z)
                - s.radius*s.radius;

        double D = b*b - 4.0*a*c;
        if (D < 0) continue;

        double t = (-b-sqrt(D)) / (2.0*a);
        if (t < 0) continue;

        tDepths[1+k] = t;
        // this part is rendering
        /**/
    }

    // the pyramids
    for (int k = 0; k < pyramidVector.size(); k++) {
        Pyramid p = pyramidVector[k];
        double * tS = new double [5];
        for (int a = 0; a < 5; a++) {
            tS[a] = -1;
        }

        // determining the point of intersection at the bottom
        if (dz != 0) {
            double tt = (p.z - start.z) / dz;
            Vector vec (0, 0, -1);
            normals[k][0] = vec;

            double X = start.x + tt * dx;
            double Y = start.y + tt * dy;

            if (X < (p.x + p.len) && X > p.x && Y < (p.y + p.len) && Y > p.y) {
                tS[0] = tt;
            }
        }

        point V0(p.x, p.y, p.z);
        point V1(p.x + p.len, p.y, p.z);
        point V2(p.x + p.len, p.y + p.len, p.z);
        point V3(p.x, p.y + p.len, p.z);
        point V4(p.x, p.y, p.z);
        point arr[] = {V0, V1, V2, V3, V4};
        point C(p.x + p.len/2.0, p.y + p.len/2.0, p.z + p.height);

        for (int a = 1; a < 5; a++) {
            point A = arr[a-1];
            point B = arr[a];
            Vector v2 = C - A;
            Vector v1 = B - A;

            Vector normal = Vector::cross(v2, v1);
            normal.normalize();
            normals[k][a] = normal;

            Vector dir (dx, dy, dz);
            Vector v = (C-start);
            if (Vector::dot(dir, normal) == 0) {
                continue;
            }
            double t = Vector::dot(v, normal) / Vector::dot(dir, normal);

            point interSetctionPoint = start + dir * t;

            Vector v0 = C - A;
            v1 = B - A;
            v2 = interSetctionPoint - A;

            // Compute dot products
            double dot00 = Vector::dot(v0, v0);
            double dot01 = Vector::dot(v0, v1);
            double dot02 = Vector::dot(v0, v2);
            double dot11 = Vector::dot(v1, v1);
            double dot12 = Vector::dot(v1, v2);

            // Compute barycentric coordinates
            double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
            double U = (dot11 * dot02 - dot01 * dot12) * invDenom;
            double V = (dot00 * dot12 - dot01 * dot02) * invDenom;

            if ((U >= 0) && (V >= 0) && (U + V < 1)) {
                tS[a] = t;
            }
        }

        int index = -1;
        double minT = 999999999999.0;
        for (int b = 0; b < 5; b++) {
            if (tS[b] > 0 && tS[b] < minT) {
                minT = tS[b];
                index = b;
            }
        }

        if (index == -1) {
            continue;
        }
        if (tS[index] < 0) {
            continue;
        }

        double t = tS[index];
        tDepths[1+sphereVector.size()+k] = t;
        whichOne[k] = index;

        // this is rendering, requires normals
        /**/
    }
    double minDepth = 999999;
    int index = -1;
    for (int k = 0; k < 1+sphereVector.size()+pyramidVector.size(); k++) {
        if (tDepths[k] > 0 && tDepths[k] < minDepth) {
            minDepth = tDepths[k];
            index = k;
        }
    }

    if (index == -1) {
        return true;
    }
    if (tDepths[index] <= 0 || tDepths[index] > tToCheck) {
        return true;
    }

    return false;
}

Color nextLevel (point start, point nextToStart, int depth) {
    if (depth == 0) {
        Color c(0, 0, 0);
        return c;
    }
    // drawing the checker board
    double *tDepths = new double [1 + sphereVector.size() + pyramidVector.size()];
    for (int k = 0; k < 1 + sphereVector.size() + pyramidVector.size(); k++) {
        tDepths[k] = -1;
    }

    int xindex, yindex;
    int *whichOne = new int [pyramidVector.size()];

    Vector** normals = new Vector* [pyramidVector.size()];
    for (int k = 0; k < pyramidVector.size(); k++) {
        normals[k] = new Vector [5];
    }

    double S, T;

    if (nextToStart.z != start.z) {
        double t = start.z / (start.z - nextToStart.z);
        if (t < 0) {
            ;
        } else {
            double X = start.x + t * (nextToStart.x - start.x);
            double Y = start.y + t * (nextToStart.y - start.y);

            if (X < 0) {
                X = -X;
                xindex = floor(X/30.0) + 1;
                S = (X - xindex*30.0)/30.0;
                S = 1 + S;
            } else {
                xindex = floor(X/30.0);
                S = (X - xindex*30.0)/30.0;
            }
            if (Y < 0) {
                Y = -Y;
                yindex = floor(Y/30.0) + 1;
                T = (Y - yindex * 30.0) / 30.0;
                T = T + 1;
            } else {
                yindex = floor(Y/30.0);
                T = (Y - yindex * 30.0) / 30.0;
            }

            tDepths[0] = t;
        }
    }
    // drawing Sphere
    double dx = nextToStart.x - start.x;
    double dy = nextToStart.y - start.y;
    double dz = nextToStart.z - start.z;

    for (int k = 0; k < sphereVector.size(); k++) {
        Sphere s = sphereVector[k];

        double a = dx*dx + dy*dy + dz*dz;
        double b = 2.0*dx*(start.x - s.x) + 2.0*dy*(start.y - s.y) + 2.0*dz*(start.z - s.z);
        double c = s.x*s.x + s.y*s.y + s.z*s.z + start.x*start.x + start.y*start.y
                + start.z*start.z - 2.0*(s.x*start.x + s.y*start.y + s.z*start.z)
                - s.radius*s.radius;

        double D = b*b - 4.0*a*c;
        if (D < 0) continue;

        double t = (-b-sqrt(D)) / (2.0*a);
        if (t < 0) continue;

        tDepths[1+k] = t;
    }

    // the pyramids
    for (int k = 0; k < pyramidVector.size(); k++) {
        Pyramid p = pyramidVector[k];
        double * tS = new double [5];
        for (int a = 0; a < 5; a++) {
            tS[a] = -1;
        }

        // determining the point of intersection at the bottom
        if (dz != 0) {
            double tt = (p.z - start.z) / dz;
            Vector vec (0, 0, -1);
            normals[k][0] = vec;

            double X = start.x + tt * dx;
            double Y = start.y + tt * dy;

            if (X < (p.x + p.len) && X > p.x && Y < (p.y + p.len) && Y > p.y) {
                tS[0] = tt;
            }
        }

        point V0(p.x, p.y, p.z);
        point V1(p.x + p.len, p.y, p.z);
        point V2(p.x + p.len, p.y + p.len, p.z);
        point V3(p.x, p.y + p.len, p.z);
        point V4(p.x, p.y, p.z);
        point arr[] = {V0, V1, V2, V3, V4};
        point C(p.x + p.len/2.0, p.y + p.len/2.0, p.z + p.height);

        for (int a = 1; a < 5; a++) {
            point A = arr[a-1];
            point B = arr[a];
            Vector v2 = C - A;
            Vector v1 = B - A;

            Vector normal = Vector::cross(v2, v1);
            normal.normalize();
            normals[k][a] = normal;

            Vector dir (dx, dy, dz);
            Vector v = (C-start);
            if (Vector::dot(dir, normal) == 0) {
                continue;
            }
            double t = Vector::dot(v, normal) / Vector::dot(dir, normal);

            point interSetctionPoint = start + dir * t;

            Vector v0 = C - A;
            v1 = B - A;
            v2 = interSetctionPoint - A;

            // Compute dot products
            double dot00 = Vector::dot(v0, v0);
            double dot01 = Vector::dot(v0, v1);
            double dot02 = Vector::dot(v0, v2);
            double dot11 = Vector::dot(v1, v1);
            double dot12 = Vector::dot(v1, v2);

            // Compute barycentric coordinates
            double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
            double U = (dot11 * dot02 - dot01 * dot12) * invDenom;
            double V = (dot00 * dot12 - dot01 * dot02) * invDenom;

            if ((U >= 0) && (V >= 0) && (U + V < 1)) {
                tS[a] = t;
            }
        }

        int index = -1;
        double minT = 999999999999.0;
        for (int b = 0; b < 5; b++) {
            if (tS[b] > 0 && tS[b] < minT) {
                minT = tS[b];
                index = b;
            }
        }

        if (index == -1) {
            continue;
        }
        if (tS[index] < 0) {
            continue;
        }

        double t = tS[index];
        tDepths[1+sphereVector.size()+k] = t;
        whichOne[k] = index;

        // this is rendering, requires normals
        /**/
    }
    double minDepth = 999999;
    int index = -1;
    for (int k = 0; k < 1+sphereVector.size()+pyramidVector.size(); k++) {
        if (tDepths[k] > 0 && tDepths[k] < minDepth) {
            minDepth = tDepths[k];
            index = k;
        }
    }

    if (index == -1) {
        Color c(0, 0, 0);
        return c;
    }
    if (tDepths[index] <= 0) {
        Color c(0, 0, 0);
        return c;
    }

    double t = tDepths[index];
    double X = start.x + t * dx;
    double Y = start.y + t * dy;
    double Z = start.z + t * dz;
    point intersectionPoint (X, Y, Z);

    if (index == 0) {
        // checker board
        double multiplier = 0;
        if ((xindex + yindex) % 2 == 1) {
            ;
        } else {
            multiplier = 1;
        }
            //pixelBuffer[i][j] = textureBuffer [(int)(s*width)][(int)(t*height)];
        Vector normal (0, 0, 1);

        Vector dir (dx, dy, dz);
        dir.normalize();

        Vector R = dir - normal * (2.0*Vector::dot(normal,dir));
        R.normalize();

        double L = 0;
        for (int b = 0; b < lightSourceVector.size(); b++) {
            LightSource l = lightSourceVector[b];
            point lightSourcePos(l.x, l.y, l.z);
            Vector oppositeDirection = lightSourcePos - intersectionPoint;
            oppositeDirection.normalize();

            if (!reachesSource(intersectionPoint+oppositeDirection*0.05, intersectionPoint+oppositeDirection, l)) {
                continue;
            }

            double lambert = Vector::dot(normal, oppositeDirection);
                if (lambert < 0) lambert = 0;
                L += lambert;
            }

        point start = intersectionPoint + R*0.05;
        point nextToStart = intersectionPoint + R*1;
        Color c = nextLevel(start, nextToStart, depth-1);

        Color multiplierTexture (1, 1, 1);
        if (textTure == 1) {
            multiplierTexture = textureBuffer[(int)(S*width)][(int)(T*height)];
            multiplier = 1.0;
        }

        c.r += min( multiplierTexture.r*(0.3*multiplier + 0.4*L*multiplier) + c.r*0.3, 1.0);
        c.g += min( multiplierTexture.g*(0.3*multiplier + 0.4*L*multiplier) + c.g*0.3, 1.0);
        c.b += min( multiplierTexture.b*(0.3*multiplier + 0.4*L*multiplier) + c.b*0.3, 1.0);
        return c;

    } else if (index < 1+sphereVector.size()) {
        // sphere
        Sphere s = sphereVector[index - 1];
        point centre (s.x, s.y, s.z);
        Vector normal = intersectionPoint - centre;
        normal.normalize();

        double L = 0;
        double P = 0;

        Vector dir (dx, dy, dz);
        dir.normalize();
        Vector R = dir - normal * (2.0*Vector::dot(normal,dir));
        R.normalize();

        for (int b = 0; b < lightSourceVector.size(); b++) {
            LightSource l = lightSourceVector[b];
            point lightSourcePos(l.x, l.y, l.z);
            Vector oppositeDirection = lightSourcePos - intersectionPoint;
            oppositeDirection.normalize();

            if (!reachesSource(intersectionPoint, intersectionPoint+oppositeDirection, l)) {
                continue;
            }

            double lambert = Vector::dot(normal, oppositeDirection);
            if (lambert < 0) lambert = 0;
            L += lambert;

            double phong = Vector::dot(oppositeDirection, R);
            if (phong < 0) phong = 0;
            P += pow(phong, s.shininess);
        }

        point start_ = intersectionPoint + R*0.05;
        point nextToStart_ = intersectionPoint + R*1;
        Color c2 = nextLevel(start_, nextToStart_, depth-1);

        Color c;
        c.r = min(s.r*s.ambient + s.r*s.diffuse*L + s.specular*P + c2.r*s.reflection, 1.0);
        c.g = min(s.g*s.ambient + s.g*s.diffuse*L + s.specular*P + c2.g*s.reflection, 1.0);
        c.b = min(s.b*s.ambient + s.b*s.diffuse*L + s.specular*P + c2.b*s.reflection, 1.0);
        return c;
    } else {
        // pyramid
        Pyramid p = pyramidVector[index-1-sphereVector.size()];
        Vector normal = normals [index-1-sphereVector.size()][whichOne[index-1-sphereVector.size()]];
        normal.normalize();

        double L = 0;
        double P = 0;

        Vector dir (dx, dy, dz);
        dir.normalize();
        Vector R = dir - normal * (2.0*Vector::dot(normal,dir));
        R.normalize();

        for (int b = 0; b < lightSourceVector.size(); b++) {
            LightSource l = lightSourceVector[b];
            point lightSourcePos(l.x, l.y, l.z);
            Vector oppositeDirection = lightSourcePos - intersectionPoint;
            oppositeDirection.normalize();

            if (!reachesSource(intersectionPoint, intersectionPoint+oppositeDirection, l)) {
                continue;
            }

            double lambert = Vector::dot(normal, oppositeDirection);
            if (lambert < 0) lambert = 0;
            L += lambert;

            double phong = Vector::dot(oppositeDirection, R);
            if (phong < 0) phong = 0;
            P += pow(phong, p.shininess);
        }

        point start_ = intersectionPoint + R*0.05;
        point nextToStart_ = intersectionPoint + R*1;
        Color c2 = nextLevel(start_, nextToStart_, depth-1);

        Color c;
        c.r = min(p.r*p.ambient + p.r*p.diffuse*L + p.specular*P + c2.r*p.shininess, 1.0);
        c.g = min(p.g*p.ambient + p.g*p.diffuse*L + p.specular*P + c2.g*p.shininess, 1.0);
        c.b = min(p.b*p.ambient + p.b*p.diffuse*L + p.specular*P + c2.b*p.shininess, 1.0);
        return c;
    }
}

void renderImage() {

    cout << "Rendering started" << endl;

    Color **pixelBuffer = new Color* [screenSize];
    for (int i = 0; i < screenSize; i++) {
        pixelBuffer[i] = new Color [screenSize];
    }
    point ** pointBuffer = new point* [screenSize];
    for (int i = 0; i < screenSize; i++) {
        pointBuffer[i] = new point [screenSize];
    }
    point p = camera_position;
    p.x += l.x * nearDistance;
    p.y += l.y * nearDistance;
    p.z += l.z * nearDistance;

    double multiplier = nearDistance * tangent / (double)screenSize;
    //cout << multiplier << " " << nearDistance << " " << tangent << " " << screenSize;
    for (int i = screenSize / 2; i < screenSize; i++) {
        for (int j = screenSize / 2; j < screenSize; j++) {
            int incrementI = i - screenSize / 2;
            int incrementJ = j - screenSize / 2;
            int oppositeI = screenSize - i;
            int oppositeJ = screenSize - j;
            Vector vi = r * ((multiplier) * (2.0*incrementI + 1));
            Vector vj = u * (-1.0*(multiplier) * (2.0*incrementJ + 1));
            point p2 = p;
            p2.x += vi.x + vj.x;
            p2.y += vi.y + vj.y;
            p2.z += vi.z + vj.z;
            pointBuffer[i][j] = p2;
            p2 = p;
            p2.x += vi.x - vj.x;
            p2.y += vi.y - vj.y;
            p2.z += vi.z - vj.z;
            pointBuffer[i][oppositeJ] = p2;
            p2 = p;
            p2.x += -vi.x + vj.x;
            p2.y += -vi.y + vj.y;
            p2.z += -vi.z + vj.z;
            pointBuffer[oppositeI][j] = p2;
            p2 = p;
            p2.x += -vi.x - vj.x;
            p2.y += -vi.y - vj.y;
            p2.z += -vi.z - vj.z;
            pointBuffer[oppositeI][oppositeJ] = p2;
        }
    }

    if (textTure == 1) {
        height = b_img.height();
        width = b_img.width();
        textureBuffer = new Color* [width];
        for (int i = 0; i < width; i++) {
            textureBuffer[i] = new Color [height];
            for (int j = 0; j < height; j++) {
                unsigned char r, g, b;
                b_img.get_pixel(i, j, r, g, b);
                Color c(r/255.0, g/255.0, b/255.0);
                textureBuffer[i][j] = c;
            }
        }
    }

    int divider = screenSize / 10;
    int counter = 0;
    int targetValue = divider;
    for (int i = 0; i < screenSize; i++) {
        for (int j = 0; j < screenSize; j++) {

            if (i == targetValue) {
                counter += 10;
                targetValue += divider;
                cout << "Rendering " << (counter) << "% complete" << endl;
            }
            // drawing the checker board
            double *tDepths = new double [1 + sphereVector.size() + pyramidVector.size()];
            for (int k = 0; k < 1 + sphereVector.size() + pyramidVector.size(); k++) {
                tDepths[k] = -1;
            }

            int xindex, yindex;
            int* whichOne = new int [pyramidVector.size()];
            double S, T;

            Vector** normals = new Vector* [pyramidVector.size()];
            for (int k = 0; k < pyramidVector.size(); k++) {
                normals[k] = new Vector [5];
            }

            if (pointBuffer[i][j].z != camera_position.z) {
                double t = camera_position.z / (camera_position.z - pointBuffer[i][j].z);
                if (t < 0) {
                    Color c(0,0,0);
                    pixelBuffer[i][j] = c;
                } else {
                    double X = camera_position.x + t * (pointBuffer[i][j].x - camera_position.x);
                    double Y = camera_position.y + t * (pointBuffer[i][j].y - camera_position.y);
                    //double distance = sqrt ( pow (X - camera_position.x, 2) + pow (Y - camera_position.y, 2) + pow (camera_position.z, 2) );
                    //if (distance > farDistance) {
                        //continue;
                    //}

                    if (X < 0) {
                        X = -X;
                        xindex = floor(X/30.0) + 1;
                        S = (X - xindex * 30.0)/30.0;
                        S = -S;
                    } else {
                        xindex = floor(X/30.0);
                        S = (X - xindex * 30.0)/30.0;
                    }
                    if (Y < 0) {
                        Y = -Y;
                        yindex = floor(Y/30.0) + 1;
                        T = (Y - yindex * 30.0)/30.0;
                        T = -T;
                    } else {
                        yindex = floor(Y/30.0);
                        T = (Y - yindex * 30.0)/30.0;
                    }

                    tDepths[0] = t;
                    // this part is rendering, requires xindex, yindex
                    /**/
                }
            }

            // drawing Sphere
            double dx = pointBuffer[i][j].x - camera_position.x;
            double dy = pointBuffer[i][j].y - camera_position.y;
            double dz = pointBuffer[i][j].z - camera_position.z;

            for (int k = 0; k < sphereVector.size(); k++) {
                Sphere s = sphereVector[k];

                double a = dx*dx + dy*dy + dz*dz;
                double b = 2.0*dx*(camera_position.x - s.x) + 2.0*dy*(camera_position.y - s.y) + 2.0*dz*(camera_position.z - s.z);
                double c = s.x*s.x + s.y*s.y + s.z*s.z + camera_position.x*camera_position.x + camera_position.y*camera_position.y
                        + camera_position.z*camera_position.z - 2.0*(s.x*camera_position.x + s.y*camera_position.y + s.z*camera_position.z)
                        - s.radius*s.radius;

                double D = b*b - 4.0*a*c;
                if (D < 0) continue;

                double t = (-b-sqrt(D)) / (2.0*a);
                if (t < 0) continue;

                tDepths[1+k] = t;
                // this part is rendering
                /**/
            }

            // the pyramids
            for (int k = 0; k < pyramidVector.size(); k++) {
                Pyramid p = pyramidVector[k];
                double * tS = new double [5];
                for (int a = 0; a < 5; a++) {
                    tS[a] = -1;
                }

                // determining the point of intersection at the bottom
                if (dz != 0) {
                    double tt = (p.z - camera_position.z) / dz;
                    Vector vec (0, 0, -1);
                    normals[k][0] = vec;

                    double X = camera_position.x + tt * dx;
                    double Y = camera_position.y + tt * dy;

                    if (X < (p.x + p.len) && X > p.x && Y < (p.y + p.len) && Y > p.y) {
                        tS[0] = tt;
                    }
                }

                point V0(p.x, p.y, p.z);
                point V1(p.x + p.len, p.y, p.z);
                point V2(p.x + p.len, p.y + p.len, p.z);
                point V3(p.x, p.y + p.len, p.z);
                point V4(p.x, p.y, p.z);
                point arr[] = {V0, V1, V2, V3, V4};
                point C(p.x + p.len/2.0, p.y + p.len/2.0, p.z + p.height);

                for (int a = 1; a < 5; a++) {
                    point A = arr[a-1];
                    point B = arr[a];
                    Vector v2 = C - A;
                    Vector v1 = B - A;

                    Vector normal = Vector::cross(v2, v1);
                    normal.normalize();
                    normals[k][a] = normal;

                    Vector dir (dx, dy, dz);
                    Vector v = (C-camera_position);
                    if (Vector::dot(dir, normal) == 0) {
                        continue;
                    }
                    double t = Vector::dot(v, normal) / Vector::dot(dir, normal);

                    point interSetctionPoint = camera_position + dir * t;

                    Vector v0 = C - A;
                    v1 = B - A;
                    v2 = interSetctionPoint - A;

                    // Compute dot products
                    double dot00 = Vector::dot(v0, v0);
                    double dot01 = Vector::dot(v0, v1);
                    double dot02 = Vector::dot(v0, v2);
                    double dot11 = Vector::dot(v1, v1);
                    double dot12 = Vector::dot(v1, v2);

                    // Compute barycentric coordinates
                    double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
                    double U = (dot11 * dot02 - dot01 * dot12) * invDenom;
                    double V = (dot00 * dot12 - dot01 * dot02) * invDenom;

                    if ((U >= 0) && (V >= 0) && (U + V < 1)) {
                        tS[a] = t;
                    }
                }

                int index = -1;
                double minT = 999999999999.0;
                for (int b = 0; b < 5; b++) {
                    if (tS[b] > 0 && tS[b] < minT) {
                        minT = tS[b];
                        index = b;
                    }
                }

                if (index == -1) {
                    continue;
                }
                if (tS[index] < 0) {
                    continue;
                }

                double t = tS[index];
                tDepths[1+sphereVector.size()+k] = t;
                whichOne[k] = index;

                // this is rendering, requires normals
                /**/
            }
            double minDepth = 999999;
            int index = -1;
            for (int k = 0; k < 1+sphereVector.size()+pyramidVector.size(); k++) {
                if (tDepths[k] > 0 && tDepths[k] < minDepth) {
                    minDepth = tDepths[k];
                    index = k;
                }
            }

            if (index == -1) {
                continue;
            }
            if (tDepths[index] <= 0) {
                continue;
            }

            double t = tDepths[index];
            double X = camera_position.x + t * dx;
            double Y = camera_position.y + t * dy;
            double Z = camera_position.z + t * dz;
            point intersectionPoint (X, Y, Z);

            if (index == 0) {
                // checker board
                Color c2(0, 0, 0);
                pixelBuffer[i][j] = c2;
                int mult = 0;
                if ((xindex + yindex) % 2 == 1) {
                    ;
                } else {
                    mult = 1;
                }
                    //pixelBuffer[i][j] = textureBuffer [(int)(s*width)][(int)(t*height)];
                Vector normal (0, 0, 1);

                Vector dir (dx, dy, dz);
                dir.normalize();

                Vector R = dir - normal * (2.0*Vector::dot(normal,dir));
                R.normalize();

                double L = 0;
                for (int b = 0; b < lightSourceVector.size(); b++) {
                    LightSource l = lightSourceVector[b];
                    point lightSourcePos(l.x, l.y, l.z);
                    Vector oppositeDirection = lightSourcePos - intersectionPoint;
                    oppositeDirection.normalize();

                    if (!reachesSource(intersectionPoint+oppositeDirection*0.02, intersectionPoint+oppositeDirection, l)) {
                        continue;
                    }
                    double lambert = Vector::dot(normal, oppositeDirection);
                    if (lambert < 0) lambert = 0;
                    L += lambert;

                }

                point start = intersectionPoint + R*0.05;
                point nextToStart = intersectionPoint + R*1;
                Color c = nextLevel(start, nextToStart, depthOfTracing-1);

                Color multiplierTexture(1.0, 1.0, 1.0);
                if (textTure == 1) {
                    multiplierTexture = textureBuffer[(int)(S*width)][(int)(T*height)];
                    mult = 1;
                }

                pixelBuffer[i][j].r += min( multiplierTexture.r*(0.3*mult + mult*0.4*L) + c.r*0.3, 1.0);
                pixelBuffer[i][j].g += min( multiplierTexture.g*(0.3*mult + mult*0.4*L) + c.g*0.3, 1.0);
                pixelBuffer[i][j].b += min( multiplierTexture.b*(0.3*mult + mult*0.4*L) + c.b*0.3, 1.0);

            } else if (index < 1+sphereVector.size()) {
                // sphere
                Sphere s = sphereVector[index - 1];
                point centre (s.x, s.y, s.z);
                Vector normal = intersectionPoint - centre;
                normal.normalize();

                double L = 0;
                double P = 0;

                Vector dir (dx, dy, dz);
                dir.normalize();

                Vector R = dir - normal * (2.0*Vector::dot(normal,dir));
                R.normalize();

                for (int b = 0; b < lightSourceVector.size(); b++) {
                    LightSource l = lightSourceVector[b];
                    point lightSourcePos(l.x, l.y, l.z);
                    Vector oppositeDirection = lightSourcePos - intersectionPoint;
                    oppositeDirection.normalize();

                    if (!reachesSource(intersectionPoint+oppositeDirection*0.02 , intersectionPoint+oppositeDirection, l)) {
                        continue;
                    }

                    double lambert = Vector::dot(normal, oppositeDirection);
                    if (lambert < 0) lambert = 0;
                    L += lambert;

                    double phong = Vector::dot(oppositeDirection, R);
                    if (phong < 0) phong = 0;
                    P += pow(phong, s.shininess);
                }

                point start = intersectionPoint + R*0.05;
                point nextToStart = intersectionPoint + R*1;
                Color c = nextLevel(start, nextToStart, depthOfTracing-1);

                pixelBuffer[i][j].r = min(s.r*s.ambient + s.r*s.diffuse*L + s.specular*P + c.r*s.reflection, 1.0);
                pixelBuffer[i][j].g = min(s.g*s.ambient + s.g*s.diffuse*L + s.specular*P + c.g*s.reflection, 1.0);
                pixelBuffer[i][j].b = min(s.b*s.ambient + s.b*s.diffuse*L + s.specular*P + c.b*s.reflection, 1.0);
            } else {
                // pyramid
                Pyramid p = pyramidVector[index-1-sphereVector.size()];
                Vector normal = normals [index-1-sphereVector.size()][whichOne[index-1-sphereVector.size()]];
                normal.normalize();

                double L = 0;
                double P = 0;

                Vector dir = pointBuffer[i][j] - camera_position;
                dir.normalize();
                Vector R = dir - normal*(2.0*Vector::dot(normal,dir));
                R.normalize();
                for (int b = 0; b < lightSourceVector.size(); b++) {
                    LightSource l = lightSourceVector[b];
                    point lightSourcePos(l.x, l.y, l.z);
                    Vector oppositeDirection = lightSourcePos - intersectionPoint;
                    oppositeDirection.normalize();

                    if (!reachesSource(intersectionPoint+oppositeDirection*0.02, intersectionPoint+oppositeDirection, l)) {
                        continue;
                    }

                    double lambert = Vector::dot(normal, oppositeDirection);
                    if (lambert < 0) lambert = 0;
                    L += lambert;

                    double phong = Vector::dot(oppositeDirection, R);
                    if (phong < 0) phong = 0;
                    P += pow(phong, p.shininess);
                }

                point start = intersectionPoint + R*0.03;
                point nextToStart = intersectionPoint + R*1;
                Color c = nextLevel(start, nextToStart, depthOfTracing-1);

                pixelBuffer[i][j].r = min(p.r*p.ambient + p.r*p.diffuse*L + p.specular*P + c.r*p.reflection, 1.0);
                pixelBuffer[i][j].g = min(p.g*p.ambient + p.g*p.diffuse*L + p.specular*P + c.g*p.reflection, 1.0);
                pixelBuffer[i][j].b = min(p.b*p.ambient + p.b*p.diffuse*L + p.specular*P + c.b*p.reflection, 1.0);
            }
        }
    }

    bitmap_image image(screenSize, screenSize);
    for (int x = 0; x < screenSize; x++) {
        for (int y = 0; y < screenSize; y++) {
            image.set_pixel(x, y, pixelBuffer[x][y].r*255, pixelBuffer[x][y].g*255, pixelBuffer[x][y].b*255);
        }
    }
    image.save_image("out.bmp");

    cout << "Rendering complete" << endl;

}

void drawGrid() {
    if (textTure == 0) {
        glBegin(GL_QUADS);
        {
            for (int i = -50; i < 50; i++) {
                for (int j = -50; j < 50; j++) {
                    if ( (i+j) % 2 == 0 ) glColor3f(1, 1, 1);
                    else glColor3f(0, 0, 0);
                    glVertex3f(i*30, j*30, 0);
                    glVertex3f(i*30 + 30, j*30, 0);
                    glVertex3f(i*30 + 30, j*30 + 30, 0);
                    glVertex3f(i*30, j*30 + 30, 0);
                }
            }
        }
        glEnd();
    } else {

    }
}

void keyboardListener(unsigned char key, int x,int y){

    Vector prev_l(l);
    Vector prev_r(r);
    Vector prev_u(u);

    double angle_in_rad = 0.8 * pi / 180.0;

	switch(key){

        case '2':
            //newr=rcos + lsin
            r = prev_r * cos (angle_in_rad) + prev_l * sin (angle_in_rad);
            l = prev_r * (-sin (angle_in_rad)) + prev_l * cos (angle_in_rad);
            break;

        case '1':
            r = prev_r * cos (-angle_in_rad) + prev_l * sin (-angle_in_rad);
            l = prev_r * (-sin (-angle_in_rad)) + prev_l * cos (-angle_in_rad);
            break;

        case '3':
            l = prev_l * cos (angle_in_rad) + prev_u * sin (angle_in_rad);
            u = prev_l * (-sin (angle_in_rad)) + prev_u * cos (angle_in_rad);
            break;

        case '4':
            l = prev_l * cos (-angle_in_rad) + prev_u * sin (-angle_in_rad);
            u = prev_l * (-sin (-angle_in_rad)) + prev_u * cos (-angle_in_rad);
            break;

        case '5':
            r = prev_r * cos (angle_in_rad) + prev_u * sin (angle_in_rad);
            u = prev_r * (-sin (angle_in_rad)) + prev_u * cos (angle_in_rad);
            break;

        case '6':
            r = prev_r * cos (angle_in_rad) + prev_u * sin (-angle_in_rad);
            u = prev_r * (sin (angle_in_rad)) + prev_u * cos (angle_in_rad);
            break;
        case '0':
            renderImage();
            break;
        case ' ':
            textTure = 1 - textTure;
            break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_UP:		//down arrow key
			camera_position.x += 3*l.x;
			camera_position.y += 3*l.y;
			camera_position.z += 3*l.z;
			break;

		case GLUT_KEY_DOWN:		// up arrow key
			camera_position.x -= 3*l.x;
			camera_position.y -= 3*l.y;
			camera_position.z -= 3*l.z;
			break;

		case GLUT_KEY_RIGHT:
			camera_position.x += 3*r.x;
			camera_position.y += 3*r.y;
			camera_position.z += 3*r.z;
			break;

		case GLUT_KEY_LEFT:
			camera_position.x -= 3*r.x;
			camera_position.y -= 3*r.y;
			camera_position.z -= 3*r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    camera_position.x += 3*u.x;
			camera_position.y += 3*u.y;
			camera_position.z += 3*u.z;
			break;

		case GLUT_KEY_PAGE_DOWN:
		    camera_position.x -= 3*u.x;
			camera_position.y -= 3*u.y;
			camera_position.z -= 3*u.z;
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

	gluLookAt(camera_position.x, camera_position.y, camera_position.z,
                camera_position.x + l.x, camera_position.y + l.y, camera_position.z + l.z,
                    u.x, u.y, u.z);

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawGrid();
	for (int i = 0 ; i < sphereVector.size(); i++) {
        Sphere s = sphereVector[i];
        s.draw();
	}
	for (int i = 0; i < pyramidVector.size(); i++) {
        Pyramid p = pyramidVector[i];
        p.draw();
	}
	for (int i = 0; i < lightSourceVector.size(); i++) {
        LightSource l = lightSourceVector[i];
        l.draw();
	}

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	camera_position.x = 50;
	camera_position.y = 50;
	camera_position.z = 50;

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
	gluPerspective(45,	1,	1,	1000.0);
}

void parseFile() {
    ifstream file;
    file.open("description.txt");

    file >> depthOfTracing >> screenSize;

    int numObjects;
    file >> numObjects;

    while (numObjects--) {
        string str;
        file >> str;
        if (str == "sphere") {
            double x, y, z, r, R, G, B, ambient, diffuse, specular, reflection;
            int shininess;
            file >> x >> y >> z >> r >> R >> G >> B >> ambient >> diffuse >> specular >> reflection >> shininess;
            Sphere s(r, x, y, z, R, G, B);
            s.setProperties(ambient, diffuse, specular, reflection, shininess);
            sphereVector.push_back(s);
        }
        else if (str == "pyramid") {
            double x0, y0, z0, len, height, R, G, B, ambient, diffuse, specular, reflection;
            int shininess;
            file >> x0 >> y0 >> z0 >> len >> height >> R >> G >> B >> ambient >> diffuse >> specular >> reflection >> shininess;
            Pyramid p(x0, y0, z0, len, height, R, G, B);
            p.setProperties(ambient, diffuse, specular, reflection, shininess);
            pyramidVector.push_back(p);
        }
    }

    int numLightSources;
    file >> numLightSources;
    while(numLightSources--) {
        double x, y, z;
        file >> x >> y >> z;
        LightSource L(x, y, z);
        lightSourceVector.push_back(L);
    }

}

int main(int argc, char **argv){

    parseFile();

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
