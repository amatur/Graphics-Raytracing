#include <cstdio>
#include <cstdlib>

#include <GL/glew.h> // GL Extension Wrangling util

#include "GlutWin.h"
#include "Vector3.h"
#include "ShaderObj.h"
#include "Camera.h"

#define GL_GENERATE_MIPMAP 0x8191// i have lame header files

// global variables ///////////////////////////////
// standard demo objects
GlutWin		*win = NULL;

ShaderObj	*pShaderObj = NULL;

Camera		*pCamera;


// texture image data
unsigned char * pBitmapData = NULL;
int				width;
int				height;
int				bpp;
unsigned int	color_map_id;
unsigned int	normal_map_id;

Vector3 light1;
Vector3 light2;

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

bool init_shaders() {

	if( glewInit() != GLEW_OK ) {

		MessageBox( NULL, "GLEW failed to init", "INIT ERROR", MB_OK );
		return false;

	}

	if( ! GLEW_ARB_vertex_shader || ! GLEW_ARB_fragment_shader ) {

		MessageBox( NULL, "you don't have ARB_vertex_shader", "INIT ERROR", MB_OK );
		return false;

	}

	if( ! GLEW_ARB_vertex_buffer_object ) {

		MessageBox( NULL, "you dont have ARB_vertex_Buffer_object", "INIT_ERROR", MB_OK );
		return false;

	}

	pShaderObj = new ShaderObj();

	// compile
	pShaderObj->loadVertShader( "phong_bump_vs.glsl" );
	pShaderObj->loadFragShader( "phong_bump_fs.glsl" );

	// create map of bindings
	ShaderObj::stringToIntMap_t	s;

	s[ "tan" ]	= 1;
	s[ "tex" ]	= 2;
	s[ "norm" ]	= 3;

	return( pShaderObj->link( s ) );

}

bool init_textures() {

	glEnable( GL_TEXTURE_2D );

	// load data
	if( ! loadBitmapFile( "Fieldstone.bmp" ) )
		return( false );
	// get a texture id
	glGenTextures( 1, &color_map_id );
	// bind and pass texure data into openGL
	glBindTexture( GL_TEXTURE_2D, color_map_id );
	// set parameters to make mipmaps
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// create the textures
	glTexImage2D( GL_TEXTURE_2D, 0,
				  GL_RGB, width, height, 0,
				  GL_RGB, GL_UNSIGNED_BYTE, pBitmapData );

	// load data
	if( ! loadBitmapFile( "FieldstoneBumpDOT3.bmp" ) )
		return( false );
	// get a texture id
	glGenTextures( 1, &normal_map_id );
	// bind and pass texure data into openGL
	glBindTexture( GL_TEXTURE_2D, normal_map_id );
	// set parameters to make mipmaps
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	// create the textures
	glTexImage2D( GL_TEXTURE_2D, 0,
				  GL_RGB, width, height, 0,
				  GL_RGB, GL_UNSIGNED_BYTE, pBitmapData );

	return( true );

}



// init demo ////////////////////////////////////////
bool initdemo() {

	// initialize GLUT class
	win = new GlutWin( 600, 800,
						100, 100,
						GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH,
						"Blinn-Phong Dot3 Demo" );


	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	// enable depth testing
	glEnable( GL_DEPTH_TEST );

	init_textures();
	
	if( ! init_shaders() )
		return( false );

	// clear rendering surface
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // background is white
	glViewport(0, 0, 640, 480);

	// set the view 
	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity();

	gluPerspective( 75.0, 800.0/600.0, 0.01, 1000.0 );
	
	//gluLookAt( 2,2,2, 0,0,0, 0,1,0 );
	pCamera = new Camera();

	pCamera->Position( 200.0, 200.0, 200.0,
					   0.0, 0.0, 0.0, 
					   0.0, 1.0, 0.0 );

	ShowCursor( false );
	SetCursorPos( 500, 400 );

	return( true );

}

void draw_faces() {

	glBegin( GL_QUADS );

		// front
		glVertexAttrib3fARB( 3, 0.0, 0.0, 1.0 );
		// tangent passing
		glVertexAttrib3fARB( 1, 1, 0, 0 );

		// tex coord passing
		glVertexAttrib2fARB( 2, 0, 1 );
		glVertex3f( -100, -100, 100 );

		glVertexAttrib2fARB( 2, 1, 1 );
		glVertex3f( 100, -100, 100 );

		glVertexAttrib2fARB( 2, 1, 0 );
		glVertex3f( 100, 100, 100 );

		glVertexAttrib2fARB( 2, 0, 0 );
		glVertex3f( -100, 100, 100 );

		// back
		glVertexAttrib3fARB( 3, 0.0, 0.0, -1.0 );
		// tangent passing
		glVertexAttrib3fARB( 1, 1, 0, 0 );

		// tex coord passing
		glVertexAttrib2fARB( 2, 0, 1 );
		glVertex3f( -100, -100, -100 );

		glVertexAttrib2fARB( 2, 1, 1 );
		glVertex3f( 100, -100, -100 );

		glVertexAttrib2fARB( 2, 1, 0 );
		glVertex3f( 100, 100, -100 );

		glVertexAttrib2fARB( 2, 0, 0 );
		glVertex3f( -100, 100, -100 );

		// top
		glVertexAttrib3fARB( 3, 0.0, 1.0, 0.0 );
		// tangent passing
		glVertexAttrib3fARB( 1, 1, 0, 0 );

		// tex coord passing
		glVertexAttrib2fARB( 2, 0, 0 );
		glVertex3f( -100, 100, -100 );

		glVertexAttrib2fARB( 2, 1, 0 );
		glVertex3f( 100, 100, -100 );

		glVertexAttrib2fARB( 2, 1, 1 );
		glVertex3f( 100, 100, 100 );

		glVertexAttrib2fARB( 2, 0, 1 );
		glVertex3f( -100, 100, 100 );

		// base
		glVertexAttrib3fARB( 3, 0.0, -1.0, 0.0 );
		// tangent passing
		glVertexAttrib3fARB( 1, 1, 0, 0 );

		// tex coord passing
		glVertexAttrib2fARB( 2, 0, 0 );
		glVertex3f( -100, -100, -100 );

		glVertexAttrib2fARB( 2, 1, 0 );
		glVertex3f( 100, -100, -100 );

		glVertexAttrib2fARB( 2, 1, 1 );
		glVertex3f( 100, -100, 100 );

		glVertexAttrib2fARB( 2, 0, 1 );
		glVertex3f( -100, -100, 100 );

		// left
		glVertexAttrib3fARB( 3, -1.0, 0.0, 0.0 );
		// tangent passing
		glVertexAttrib3fARB( 1, 0, 0, -1 );

		// tex coord passing
		glVertexAttrib2fARB( 2, 0, 1 );
		glVertex3f( -100, -100, 100 );

		glVertexAttrib2fARB( 2, 1, 1 );
		glVertex3f( -100, -100, -100 );

		glVertexAttrib2fARB( 2, 1, 0 );
		glVertex3f( -100, 100, -100 );

		glVertexAttrib2fARB( 2, 0, 0 );
		glVertex3f( -100, 100, 100 );

		// right
		glVertexAttrib3fARB( 3, 1.0, 0.0, 0.0 );
		// tangent passing
		glVertexAttrib3fARB( 1, 0, 0, -1 );

		// tex coord passing
		glVertexAttrib2fARB( 2, 0, 1 );
		glVertex3f( 100, -100, 100 );

		glVertexAttrib2fARB( 2, 1, 1 );
		glVertex3f( 100, -100, -100 );

		glVertexAttrib2fARB( 2, 1, 0 );
		glVertex3f( 100, 100, -100 );

		glVertexAttrib2fARB( 2, 0, 0 );
		glVertex3f( 100, 100, 100 );


	glEnd();

}



void render() {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_TEXTURE_2D );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( pCamera->pos.x, pCamera->pos.y, pCamera->pos.z, 
			   pCamera->view.x, pCamera->view.y, pCamera->view.z, 
			   pCamera->up.x, pCamera->up.y, pCamera->up.z );

	// use shader object
	pShaderObj->bind();

	theta += 0.001f;

	light1.x = 160.0 * sin( theta );
	light1.y = 160.0 * cos( theta );
	light1.z = 150.0;

	light2.x = 160.0 * sin( theta + ( 3.141592 ) );
	light2.y = 160.0 * cos( theta + ( 3.141592 ) );
	light2.z = -150.0;

	// get positions of uniforms
	int color_map_loc = pShaderObj->getUniformLocation( "color_map" );
	int normal_map_loc = pShaderObj->getUniformLocation( "normal_map" );
	int eye_pos_loc = pShaderObj->getUniformLocation( "eye_pos" );
	int light1_dir_loc = pShaderObj->getUniformLocation( "light1_dir" );
	int light2_dir_loc = pShaderObj->getUniformLocation( "light2_dir" );

	int kd_loc = pShaderObj->getUniformLocation( "kd" );
	int ks_loc = pShaderObj->getUniformLocation( "ks" );
	int ka_loc = pShaderObj->getUniformLocation( "ka" );
	int spec_exp_loc = pShaderObj->getUniformLocation( "spec_exp" );

	// set light vars
	glUniform1fARB( kd_loc, 0.70 );
	glUniform1fARB( ks_loc, 0.50 );
	glUniform1fARB( ka_loc, 0.00 );
	glUniform1fARB( spec_exp_loc, 150.0 );

	// pass eye_pos, light_dir
	glUniform3fARB( eye_pos_loc, pCamera->pos.x, pCamera->pos.y, pCamera->pos.z );
	glUniform3fARB( light1_dir_loc, light1.x, light1.y, light1.z );
	glUniform3fARB( light2_dir_loc, light2.x, light2.y, light2.z );

	// set up texture units
	glActiveTexture( GL_TEXTURE0 + 1 );
	glBindTexture( GL_TEXTURE_2D, color_map_id );
	glUniform1iARB( color_map_loc, 1 );
	
	glActiveTexture( GL_TEXTURE0 + 2 );
	glBindTexture( GL_TEXTURE_2D, normal_map_id );
	glUniform1iARB( normal_map_loc, 2 );

	draw_faces();

	glUseProgramObjectARB( NULL );

	// draw lights as points
	glPointSize( 10.0 );
	glColor4f( 1.0, 1.0, 1.0, 0.0 ) ;

	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, NULL );

	glBegin( GL_POINTS );

		glVertex3f( light1.x, light1.y, light1.z );
		glVertex3f( light2.x, light2.y, light2.z );

	glEnd();

	glutSwapBuffers();
	glutPostRedisplay();

}

void kb_input( unsigned char key, int x, int y ) {

	switch( key ) {

	case 'w': pCamera->Move( 10 ); break;
	case 's': pCamera->Move( -10 ); break;
	case 'a': pCamera->Strafe( -10 ); break;
	case 'd': pCamera->Strafe( 10 ); break;
	case 'q': exit( 0 );

	}

	cout << pCamera->pos.x << "  " << pCamera->pos.y << "  " << pCamera->pos.z << "  " << endl;


}

void mouse_motion( int x, int y ) {

	static int mx = 500;
	static int my = 400;

	float dx = x - mx;
	float dy = my - y;

	my = y;
	mx = x;

	pCamera->view.y -= dy * 10.0f;

	if( ( pCamera->view.y - pCamera->pos.y ) > 700 ) 
		 pCamera->view.y = pCamera->pos.y + 700;

	if( ( pCamera->view.y - pCamera->pos.y ) < -700 ) 
		 pCamera->view.y = pCamera->pos.y - 700;

	pCamera->Rotate( 0, dx / 100.0f, 0 );
	
	//SetCursorPos( 500, 400 );

}


int main() {

	// init
	if( initdemo() ) {

		glutDisplayFunc( render );
		glutKeyboardFunc( kb_input );
		glutPassiveMotionFunc( mouse_motion );

		glutMainLoop();

		delete win;
		delete [] pBitmapData;

	}

	return( 0 );

}
