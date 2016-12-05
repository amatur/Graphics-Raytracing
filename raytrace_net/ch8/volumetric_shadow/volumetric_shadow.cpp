#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#include <GL/glew.h> // GL Extension Wrangling util

#include "GlutWin.h"
#include "Vector3.h"
#include "ShaderObj.h"
#include "Camera.h"
#include "volumetric_shadow.h"

#define GL_GENERATE_MIPMAP 0x8191// i have lame header files

// global variables ///////////////////////////////
// standard demo objects
GlutWin		*win = NULL;
ShaderObj	*pShaderObj = NULL;
Vector3		lightpos = Vector3( 2, -1, 0 );
Camera		*pCamera = NULL;

float theta = 0;
model_t		cube;
GLfloat		*cube_vertex_buffer = NULL;
unsigned int buffer_id;

void read_model() {

	std::ifstream i = std::ifstream( "cube.dat", ios::in );

	i >> cube.n_verts;
	cube.vertex_list = new Vector3[cube.n_verts];

	for( int v = 0; v < cube.n_verts; v++ ) {

		i >> cube.vertex_list[v].x >> cube.vertex_list[v].y >> cube.vertex_list[v].z;

	}

	i >> cube.n_norms;
	cube.normal_list = new Vector3[cube.n_norms];

	for( int n = 0; n < cube.n_norms; n++ ) {

		i >> cube.normal_list[n].x >> cube.normal_list[n].y >> cube.normal_list[n].z;

	}

	i >> cube.n_tris;
	cube.tri_list	 = new triangle_t[cube.n_tris];

	for( int t = 0; t < cube.n_tris; t++ ) {

		for( int n = 0; n < 3; n++ )
			i >> cube.tri_list[t].index[n];

		i >> cube.tri_list[t].norm_index;

	}

	cube.n_edges	= 3 * cube.n_tris / 2;
	cube.edge_list	= new edge_t[cube.n_edges];

	// calc adjacency info
	int e = 0;
	int tri = 0;
	int tri_edge = 0;
	
	while( e < cube.n_edges ) {

		// find edge
		edge_t	edge;

		edge.tri_index[0]	= tri;
		edge.vert_index[0]	= cube.tri_list[tri].index[tri_edge];
		edge.vert_index[1]	= cube.tri_list[tri].index[(++tri_edge)%3];

		if( tri_edge == 3 ) {
			tri++;
			tri_edge = 0;
		}

		// if not already in array
		bool edge_in_array = false;
		for( int c = 0; c < e; c++ ) {

			if( ( cube.edge_list[c].vert_index[0] == edge.vert_index[0] && 
				  cube.edge_list[c].vert_index[1] == edge.vert_index[1] ) ||
				( cube.edge_list[c].vert_index[0] == edge.vert_index[1] && 
				  cube.edge_list[c].vert_index[1] == edge.vert_index[0] ) ) {
				
				edge_in_array = true;
				break;
			}
		}

		if( edge_in_array )
			continue;

		// find other tri that shares this edge
		for( int d = 0; d < cube.n_tris; d++ ) {

			if( d != edge.tri_index[0] &&
				( cube.tri_list[d].index[0] == edge.vert_index[0] ||
				  cube.tri_list[d].index[1] == edge.vert_index[0] ||
				  cube.tri_list[d].index[2] == edge.vert_index[0] ) &&
			    ( cube.tri_list[d].index[0] == edge.vert_index[1] ||
				  cube.tri_list[d].index[1] == edge.vert_index[1] ||
				  cube.tri_list[d].index[2] == edge.vert_index[1] ) ) {

				// this tri shares the edge with current tri
				edge.tri_index[1] = d;
				break;

			}
		}

		// place on list
		cube.edge_list[e++] = edge;
	}

}

void init_vertex_buffer() {

	int num_verts = cube.n_verts * 2;

	cube_vertex_buffer = new GLfloat[ num_verts * 4 ];

	glGenBuffersARB( 1, &buffer_id );
	int offs = 0;

	for( int n = 0 ; n < cube.n_verts; n++ ) {

		cube_vertex_buffer[offs+0] = cube.vertex_list[n].x;
		cube_vertex_buffer[offs+1] = cube.vertex_list[n].y;
		cube_vertex_buffer[offs+2] = cube.vertex_list[n].z;
		cube_vertex_buffer[offs+3] = 1.0;

		cube_vertex_buffer[offs+(cube.n_verts*4)+0] = cube.vertex_list[n].x;
		cube_vertex_buffer[offs+(cube.n_verts*4)+1] = cube.vertex_list[n].y;
		cube_vertex_buffer[offs+(cube.n_verts*4)+2] = cube.vertex_list[n].z;
		cube_vertex_buffer[offs+(cube.n_verts*4)+3] = 0.0;

		offs += 4;

	}

	// Create buffer object
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, buffer_id );
	// Initialize data store of buffer object
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, num_verts * 4 * sizeof( GLfloat ), cube_vertex_buffer, GL_STATIC_DRAW_ARB );

	delete [] cube_vertex_buffer;
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
	pShaderObj->loadVertShader( "volum_shad_vs.glsl" );
	pShaderObj->loadFragShader( "volum_shad_fs.glsl" );

	ShaderObj::stringToIntMap_t s;

	return( pShaderObj->link( s ) );

}

void infinite_projection_mxe( float nearz, float left, float right, float top, float bottom ) {

	float x = ( 2.0 * nearz ) / ( right - left );
	float y = ( 2.0 * nearz ) / ( top - bottom );
	float a = ( right + left ) / ( right - left );
	float b = ( top + bottom ) / ( top - bottom );
	float c = -1.0;
	float d = -2.0 * nearz;

	double m[] = { x, 0, 0, 0,
				   0, y, 0, 0,
				   a, b, c, -1,
				   0, 0, d, 0 };

	glLoadMatrixd( m );


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

	glShadeModel( GL_SMOOTH );
	
	if( ! init_shaders() )
		return( false );

	read_model();
	init_vertex_buffer();

	// clear rendering surface
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // background is white
	glViewport(0, 0, 800, 600);

	// set the view 
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	infinite_projection_mxe( 0.1, -0.1*64/48.0, 0.1*64/48.0, 0.1, -0.1 );

	pCamera = new Camera( 5, 5, 5, 0, 0, 0, 0, 1, 0 );

	ShowCursor( false );
	SetCursorPos( 500, 400 );

	if( ! GLEW_ATI_separate_stencil ) {

		MessageBox( NULL, "you dont have ARB_vertex_Buffer_object", "INIT_ERROR", MB_OK );
		return false;

	}

	if( ! GLEW_EXT_stencil_wrap ) {

		MessageBox( NULL, "you dont have ARB_vertex_Buffer_object", "INIT_ERROR", MB_OK );
		return false;

	}

	//glEnable( GL_STENCIL_OP_SEPARATE_FACE_ATI );

	return( true );

}

void killdemo() {

	delete win;
	delete pShaderObj;

	delete [] cube_vertex_buffer;

	delete [] cube.vertex_list;
	delete [] cube.edge_list;
	delete [] cube.tri_list;

}

void render_shadow_cap() {



}

void markshadows() {

	glStencilOpSeparateATI( GL_FRONT, GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP );
	glStencilOpSeparateATI( GL_BACK, GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP );

	/*glActiveStencilFaceEXT( GL_FRONT );
	glStencilOp( GL_KEEP, GL_DECR_WRAP_EXT, GL_KEEP );
	glActiveStencilFaceEXT( GL_BACK );
	glStencilOp( GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP );*/
	glCullFace( GL_NONE );

	// determine backfaces
	for( int c = 0; c < cube.n_tris; c++ ) {

		cube.tri_list[c].back_face = ( lightpos.dot( cube.normal_list[ cube.tri_list[c].norm_index ] ) < 0 );

	}

	unsigned short * cap_index_buffer = new unsigned short [ cube.n_tris * 3 ];
	int offs = 0;

	// render caps
	for( int n = 0; n < cube.n_tris; n++ ) {

		if( cube.tri_list[n].back_face ) {

			for( int v = 0; v < 3; v++ )
				cap_index_buffer[offs++] = cube.tri_list[n].index[v] + cube.n_verts;


		} else {

			for( int v = 0; v < 3; v++ )
				cap_index_buffer[offs++] = cube.tri_list[n].index[v];

		}

	}

	glDrawElements( GL_TRIANGLES, cube.n_tris * 3, GL_UNSIGNED_SHORT, cap_index_buffer );

	delete [] cap_index_buffer;

	std::vector<unsigned int> index;
	index.resize( 0 );

	// render sides
	for( int e = 0; e < cube.n_edges; e++ ) {

		if( cube.tri_list[ cube.edge_list[e].tri_index[0] ].back_face != 
			cube.tri_list[ cube.edge_list[e].tri_index[1] ].back_face ) {
			// sillhouette edge
			int v0, v1;

			if( cube.tri_list[ cube.edge_list[e].tri_index[0] ].back_face ) {
				v0 = cube.edge_list[e].vert_index[0];
				v1 = cube.edge_list[e].vert_index[1];
			} else {
				v0 = cube.edge_list[e].vert_index[1];
				v1 = cube.edge_list[e].vert_index[0];
			}

			index.push_back( v0 );
			index.push_back( v1 );
			index.push_back( v1 + cube.n_verts );
			index.push_back( v0 + cube.n_verts );

		}

	}

	unsigned short * p = new unsigned short [ index.size() ];
	for( int d = 0; d < index.size(); d++ )
		p[d] = index[d];

	glDrawElements( GL_QUADS, index.size(), GL_UNSIGNED_SHORT, p );

	delete [] p;

}

void render_scene() {

	glColor4f( 0.0, 1.0, 0.0, 1.0 );

	glBegin( GL_QUADS );

	glNormal3f( 0, 1, 0 );
	glVertex3f( -10, -10, 10 );
	glVertex3f( 10, -10, 10 );
	glVertex3f( 10, -10, -10 );
	glVertex3f( -10, -10, -10 );
	
	glNormal3f( 0, 0, 1 );
	glVertex3f( -10, 10, -10 );
	glVertex3f( 10, 10, -10 );
	glVertex3f( 10, -10, -10 );
	glVertex3f( -10, -10, -10 );

	glNormal3f( 0, 0, -1 );
	glVertex3f( -10, 10, 10 );
	glVertex3f( 10, 10, 10 );
	glVertex3f( 10, -10, 10 );
	glVertex3f( -10, -10, 10 );
	
	glNormal3f( -1, 0, 0 );
	glVertex3f( 10, 10, -10 );
	glVertex3f( 10, 10, 10 );
	glVertex3f( 10, -10, 10 );
	glVertex3f( 10, -10, -10 );
	
	glNormal3f( 1, 0, 0 );
	glVertex3f( -10, 10, -10 );
	glVertex3f( -10, 10, 10 );
	glVertex3f( -10, -10, 10 );
	glVertex3f( -10, -10, -10 );

	glEnd();

	glColor4f( 1.0, 0.0, 0.0, 1.0 );


	glBegin( GL_TRIANGLES );

	for( int t = 0; t < cube.n_tris; t++ )
		for( int v = 0; v < 3; v++ )
			glVertex3f( cube.vertex_list[ cube.tri_list[t].index[v] ].x,
						cube.vertex_list[ cube.tri_list[t].index[v] ].y,
						cube.vertex_list[ cube.tri_list[t].index[v] ].z );

	glEnd();

	glPointSize( 20 );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glBegin( GL_POINTS );
	glVertex3f( lightpos.x, lightpos.y, lightpos.z );
	glEnd();


}

void render() {

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( pCamera->pos.x, pCamera->pos.y, pCamera->pos.z, 
			   pCamera->view.x, pCamera->view.y, pCamera->view.z, 
			   pCamera->up.x, pCamera->up.y, pCamera->up.z );

	glBlendFunc( GL_ONE, GL_ZERO );

	glDepthMask( 0xFF );
	glDepthFunc( GL_LEQUAL );

	glEnable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );

	float globamb[]	= { 0.6, 0.6, 0.6, 1.0 };
	float black[]	= { 0.0, 0.0, 0.0, 0.0 };

	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globamb );

	render_scene();

	float spc[] = { 0, 1, 0, 1 };
	float dif[] = { 0, 1, 0, 1 };
	float pos[] = { lightpos.x, lightpos.y, lightpos.z, 1.0 };
	float dir[] = { -lightpos.x, -lightpos.y, -lightpos.z, 1.0 };

	glEnable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_DIFFUSE );

	glLightfv( GL_LIGHT0, GL_DIFFUSE, dif );
	glLightfv( GL_LIGHT0, GL_SPECULAR, spc );
	glLightfv( GL_LIGHT0, GL_POSITION, pos );
	glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, dir );
	glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 100 );

	glEnable( GL_LIGHT0 );
	glBlendFunc( GL_ONE, GL_ZERO );
	glEnable( GL_STENCIL_TEST );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, black );

	glDisable( GL_LIGHTING );
	glDepthMask( 0x00 );


	// use shader object
	pShaderObj->bind();

	theta += 0.01;
	lightpos.x = 2.0 * sin( theta );
	lightpos.y = 2.0 * cos( theta );
	lightpos.z = 2.0 * cos( theta );

	int loc = pShaderObj->getUniformLocation( "light" );
	glUniform4fARB( loc, lightpos.x, lightpos.y, lightpos.z, 0.0 );

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, buffer_id );

	glVertexPointer( 4, GL_FLOAT, 0, NULL );
	glEnableClientState( GL_VERTEX_ARRAY );

	glClear( GL_STENCIL_BUFFER_BIT );
	glColorMask( 0,0,0,0 );
	glStencilFunc( GL_ALWAYS, 0, ~0 );
	glStencilMask( ~0 );

	glStencilFuncSeparateATI( GL_ALWAYS, GL_ALWAYS, 0, ~0 );

	// render
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	markshadows();

	glColorMask( 1,1,1,1);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	markshadows();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glDisableClientState( GL_VERTEX_ARRAY );
	glUseProgramObjectARB( NULL );

	glStencilFunc( GL_EQUAL, 0, ~0 );
	glStencilOpSeparateATI( GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP );
	glStencilOpSeparateATI( GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP );

	glDepthFunc( GL_EQUAL );
	glColorMask( 1,1,1,1 );
	//glCullFace( GL_BACK );

	
	glStencilFuncSeparateATI( GL_EQUAL, GL_EQUAL, 0, ~0 );

	
	glEnable( GL_LIGHTING );

	render_scene();

	glutSwapBuffers();
	glutPostRedisplay();

	glPopAttrib();

}


void kb_input( unsigned char key, int x, int y ) {

	switch( key ) {

	case 'w': pCamera->Move( 1.0 ); break;
	case 's': pCamera->Move( -1.0 ); break;
	case 'a': pCamera->Strafe( -1.0 ); break;
	case 'd': pCamera->Strafe( 1.0 ); break;
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

	pCamera->view.y -= dy / 10.0f;

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

	}

	killdemo();

	return( 0 );

}
