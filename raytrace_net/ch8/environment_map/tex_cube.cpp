#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>

#include "GlutWin.h"

#define GL_GENERATE_MIPMAP 0x8191// i have lame header files

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
	win = new GlutWin( 600, 800,
						100, 100,
						GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH,
						"Environment Map Demo" );


	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	// see if cubemapping is available
	if( glewInit() != GLEW_OK ) {

		MessageBox( NULL, "GLEW failed to init, check you have glew32.dll", "INIT ERROR", MB_OK );
		return false;

	}

	if( ! GLEW_EXT_texture_cube_map ) {

		MessageBox( NULL, "you don't have the cube map extension", "INIT ERROR", MB_OK );
		return false;

	}

	// enable depth testing
	glEnable( GL_DEPTH_TEST );

	glEnable( GL_TEXTURE_3D );

	// pass texure params into openGL
	glTexParameteri( GL_TEXTURE_CUBE_MAP_EXT, GL_GENERATE_MIPMAP, GL_TRUE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	// create the textures
	for( int c = 0; c < 6; c++ ) {

		char f[10];
		sprintf( f, "b%d.bmp", c );
		
		// load data
		if( ! loadBitmapFile( f ) )
			return( false );

		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + c, 0,
					  GL_RGB, width, height, 0,
					  GL_RGB, GL_UNSIGNED_BYTE, pBitmapData );

	}

	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_NORMALIZE);

	// clear rendering surface
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // background is white
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

	glEnable( GL_TEXTURE_CUBE_MAP_EXT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glRotatef( theta, 1, 0, -1 );
	glRotatef( -theta, 0, -1, 0 );

	theta += 0.1;

	glutSolidTeapot( 3 );

	glDisable( GL_TEXTURE_CUBE_MAP_EXT );
 
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
