/*// demo of a rotating textured cube, for third eition of our book!
Sandy Hill and Stephen Kelley: 7/24/2006. Show perspective views of three faces of the cube at each moment. Three faces use procedural texture ( a checkerboard), and the other three use bitmaps: the mandrill, an American flag, and ?
The challenge is to get each texture properly attached to each face. Each face is drawn s an OpenGL quad, with four vertices, of course. */
#include <cstdio>
#include <cstdlib>

#include "GlutWin.h"

#define GL_GENERATE_MIPMAP 0x8191// I have lame header files
// so far there is a link error: the definition of GlutWWin, the constructor for the class, seems to be missing
// It should be in GlutWin.h.
// yes, I just checked: it IS there.
// global variables ///////////////////////////////
// standard demo objects
GlutWin		*win;

// texture image data
unsigned char * pBitmapData;
int				width;
int				height;
int				bpp;
unsigned int	texture_id;

float theta = 0.0;

// load bitmap file ///////////////////////////////
bool loadBitmapFile( const char * fileName ) {

	FILE				* fp;
	BITMAPFILEHEADER	bmpFH;
	BITMAPINFOHEADER	bmpIH;
	unsigned char		temp;

	fp = fopen( fileName, "rb" ); // rb = read binary

	if( fp == NULL )
		return( false );

	// read in the file header
	fread( ( void * )&bmpFH, sizeof( BITMAPFILEHEADER ), 1, fp );

	if( bmpFH.bfType != 0x4D42 ) {

		fclose( fp );
		return( false );

	}

	// read in the info header
	fread( ( void * )&bmpIH, sizeof( BITMAPINFOHEADER ), 1, fp );

	// move the file stream to teh start of the image data
	fseek( fp, bmpFH.bfOffBits, SEEK_SET );

	// set size in bytes
	bmpIH.biSizeImage = bmpIH.biHeight * bmpIH.biWidth * ( bmpIH.biBitCount / 8 );

	// allocate mem for the image data
	pBitmapData = new unsigned char[ bmpIH.biSizeImage ];

	if( pBitmapData == NULL ){

		// if there was trouble allocating the mem
		fclose( fp );
		return( false );

	}

	// read from the stream ( 1 byte at a time, biSizeImage times )
	fread( ( void * )pBitmapData, 1, bmpIH.biSizeImage, fp );

	if( pBitmapData == NULL ) {

		fclose( fp );
		return( false );

	}

	for( int c = 0; c < bmpIH.biSizeImage; c += 3 ) {

		// swap the red and blue bytes
		temp					= pBitmapData[ c ];
		pBitmapData[ c ]		= pBitmapData[ c + 2 ];
		pBitmapData[ c + 2 ]	= temp;

	}

	fclose( fp );

	width	= bmpIH.biWidth;
	height	= bmpIH.biHeight;
	bpp		= bmpIH.biBitCount;

	return( true );

}

// init demo ////////////////////////////////////////
bool initdemo() {

	// initialize GLUT class
	win = new GlutWin( int 600, int 800,
						int 100, int 100,
						unsigned intGLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH,
						char * "Textured Cube Demo" );


	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	// enable depth testing
	glEnable( GL_DEPTH_TEST );

	glEnable( GL_TEXTURE_2D );

	// load data
	if( ! loadBitmapFile( "metaltex.bmp" ) )
		return( false );
	// get a texture id
	glGenTextures( 1, &texture_id );
	// bind and pass texure data into openGL
	glBindTexture( GL_TEXTURE_2D, texture_id );
	// set parameters to make mipmaps
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// create the textures
	glTexImage2D( GL_TEXTURE_2D, 0,
				  GL_RGB, width, height, 0,
				  GL_RGB, GL_UNSIGNED_BYTE, pBitmapData );

	// clear rendering surface
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // background is black
	glViewport(0, 0, 640, 480);

	// set the view 
	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity();

	gluPerspective( 75.0, 800.0/600.0, 0.01, 100.0 );
	
	gluLookAt( 5,5,5,0,0,0,0,1,0 );



	return( true );

}

void kb_input( unsigned char key, int x, int y ) {


	if( key == 'q' )
		exit( 0 );

}


void render() {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glRotatef( theta, 1, 1, 1 );

	theta += 0.1;

	glBindTexture( GL_TEXTURE_2D, texture_id );

	glBegin( GL_QUADS );

	glTexCoord2f( 0.0, 0.0 );	glVertex3f( -1.0, -1.0, -1.0 );
	glTexCoord2f( 1.0, 0.0 );	glVertex3f( -1.0,  1.0, -1.0 );
	glTexCoord2f( 1.0, 1.0 );	glVertex3f(  1.0,  1.0, -1.0 );
	glTexCoord2f( 0.0, 1.0 );	glVertex3f(  1.0, -1.0, -1.0 );

	glTexCoord2f( 0.0, 0.0 );	glVertex3f( -1.0, -1.0,  1.0 );
	glTexCoord2f( 1.0, 0.0 );	glVertex3f( -1.0,  1.0,  1.0 );
	glTexCoord2f( 1.0, 1.0 );	glVertex3f(  1.0,  1.0,  1.0 );
	glTexCoord2f( 0.0, 1.0 );	glVertex3f(  1.0, -1.0,  1.0 );

	glTexCoord2f( 0.0, 0.0 );	glVertex3f( -1.0, -1.0,  1.0 );
	glTexCoord2f( 1.0, 0.0 );	glVertex3f( -1.0, -1.0, -1.0 );
	glTexCoord2f( 1.0, 1.0 );	glVertex3f( -1.0,  1.0, -1.0 );
	glTexCoord2f( 0.0, 1.0 );	glVertex3f( -1.0,  1.0,  1.0 );

	glTexCoord2f( 0.0, 0.0 );	glVertex3f(  1.0, -1.0,  1.0 );
	glTexCoord2f( 1.0, 0.0 );	glVertex3f(  1.0, -1.0, -1.0 );
	glTexCoord2f( 1.0, 1.0 );	glVertex3f(  1.0,  1.0, -1.0 );
	glTexCoord2f( 0.0, 1.0 );	glVertex3f(  1.0,  1.0,  1.0 );

	glTexCoord2f( 0.0, 0.0 );	glVertex3f( -1.0, -1.0,  1.0 );
	glTexCoord2f( 1.0, 0.0 );	glVertex3f( -1.0, -1.0, -1.0 );
	glTexCoord2f( 1.0, 1.0 );	glVertex3f(  1.0, -1.0, -1.0 );
	glTexCoord2f( 0.0, 1.0 );	glVertex3f(  1.0, -1.0,  1.0 );

	glTexCoord2f( 0.0, 0.0 );	glVertex3f( -1.0,  1.0,  1.0 );
	glTexCoord2f( 1.0, 0.0 );	glVertex3f( -1.0,  1.0, -1.0 );
	glTexCoord2f( 1.0, 1.0 );	glVertex3f(  1.0,  1.0, -1.0 );
	glTexCoord2f( 0.0, 1.0 );	glVertex3f(  1.0,  1.0,  1.0 );

	glEnd();

	glutSwapBuffers();
	glutPostRedisplay();

}
int main() {

	// init
	if( initdemo() ) {

		glutDisplayFunc( render );
		glutKeyboardFunc( kb_input );

		glutMainLoop();

		delete win;
		delete [] pBitmapData;

	}

	return( 0 );

}
