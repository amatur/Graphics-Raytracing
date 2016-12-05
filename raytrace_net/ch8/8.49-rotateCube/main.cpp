//demo program for Fig. 8.48: the rotating cube with texture on each face. 
//Non-interactive. Hill/Kelley, 3e, July 2006
//to see top face change glTranslated() - y argument up or down

#include "windows.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"
#include "RGBpixmap.h"

//######################## GLOBALS ########################
RGBpixmap pix[6];    // make six (empty) pixmaps
float xSpeed = 0, ySpeed = 1,  xAngle = 0.02, yAngle = 0.02;
//<<<<<<<<<<<<<<<<<<<<<<<<<<< myinit >>>>>>>>>>>>>>>>>>>>>>>>>>>.
void myInit(void)
{	
	glClearColor(1.0f,1.0f,1.0f,1.0f);  // background is white 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	pix[0].makeCheckerboard();        	// make pixmap procedurally 
	pix[0].setTexture(2001);              	// create texture

	pix[1].readBMPFile("Mandrill.bmp");  // make pixmap from image
	pix[1].setTexture(2002);		// create texture

	pix[2].readBMPFile("Mandrill.bmp");  // make pixmap from image
	pix[2].setTexture(2003);		// create texture

	pix[3].makeCheckerboard();  // make pixmap from image
	pix[3].setTexture(2004);		// create texture

	pix[4].readBMPFile("red.bmp");  // make pixmap from image
	pix[4].setTexture(2005);		// create texture

	pix[5].makeCheckerboard();  // make pixmap from image
	pix[5].setTexture(2006);		// create texture


	glViewport(0, 0, 640, 480); // set up the viewing system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 640.0/ 480, 1.0, 30.0); // set camera shape
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, -0.2, -4); // move camera back
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<< display >>>>>>>>>>>>>>>>>>>>>>
void display(void)
{
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glPushMatrix();
	glRotated(xAngle, 1.0,0.0,0.0);  glRotated(yAngle, 0.0,1.0,0.0); // rotate
		
	glBindTexture(GL_TEXTURE_2D,2001); 	// top face: 'fake' checkerboard
	glBegin(GL_QUADS);
	glTexCoord2f(-1.0, -1.0); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(-1.0, 2.0); glVertex3f(-1.0f, 1.0f,  1.0f);
	glTexCoord2f(2.0, 2.0); glVertex3f( 1.0f, 1.0f,  1.0f);
	glTexCoord2f(2.0, -1.0); glVertex3f( 1.0f, 1.0f, -1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,2002); 	// right face: mandrill
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(1.0f, -1.0f,  1.0f);
	glTexCoord2f(0.0, 2.0); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(2.0, 2.0); glVertex3f(1.0f,  1.0f, -1.0f);
	glTexCoord2f(2.0, 0.0); glVertex3f(1.0f,  1.0f,  1.0f);
	glEnd();	

	glBindTexture(GL_TEXTURE_2D,2003); 	// left face: mandrill
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f, 1.0f,  -1.0f);
	glTexCoord2f(2.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(2.0, 2.0); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0, 2.0); glVertex3f(-1.0f,  -1.0f,  -1.0f);
	glEnd();	

	glBindTexture(GL_TEXTURE_2D,2004); 	// bottom face: 'fake' checkerboard
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0, -2.0); glVertex3f(1.0f, -1.0f,  -1.0f);
	glTexCoord2f(-2.0, -2.0); glVertex3f( -1.0f, -1.0f,  -1.0f);
	glTexCoord2f(-2.0, 1.0); glVertex3f( -1.0f, -1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,2005); 	// box front: red
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 2.0); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(2.0, 0.0); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0, -2.0); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(-2.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,2006); 	// box back: 'fake' checkerboard
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();

	glFlush();
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< spinner >>>>>>>>>>>>>>>>>>>>>
void spinner(void)
{ // alter angles by small amount
	xAngle += xSpeed;  yAngle += ySpeed;
	display();
} 
//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void main(int argc, char **argv)
{
	
	glutInit(&argc, argv);          // initialize the toolkit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // set the display mode
	glutInitWindowSize(600, 800);     // set the window size
	glutInitWindowPosition(100, 150); // set the window position on the screen
	glutCreateWindow("Rotating Cube Demo"); // open the screen window(with its exciting title)
	glutDisplayFunc(display);     // register the redraw function
	myInit();                   
	glutIdleFunc(spinner);
	
	glutMainLoop(); 		     // go into a perpetual loop
} 
