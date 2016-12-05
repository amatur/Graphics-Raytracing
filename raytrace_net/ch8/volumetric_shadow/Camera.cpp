#include <math.h>
#include <windows.h>
#include <GL\gl.h>

#include "Camera.h"


Camera::Camera( float posX, float posY, float posZ, 
		   		  float viewX, float viewY, float viewZ,
				  float upX, float upY, float upZ ) {

	Position( posX,  posY,  posZ, 
			  viewX,  viewY,  viewZ,
			  upX,  upY,  upZ );

}

void Camera::Move( float speed ) {

	Vector3 d = view - pos;

	// noralize the vector
	d.normalize();

	pos.x = pos.x + d.x * speed;
	pos.z = pos.z + d.z * speed;
	pos.y = pos.y + d.y * speed;
	view.x = view.x + d.x * speed;
	view.z = view.z + d.z * speed;
	view.y = view.y + d.y * speed;

}

void Camera::Strafe( float speed ) {

	Vector3 d = view - pos;
	Vector3 temp = view;

	//temp.x = float( pos.x + cos( 90 ) * d.x - sin( 90 ) * d.z );
	//temp.z = float( pos.z + sin( 90 ) * d.x + cos( 90 ) * d.z );

	temp.x = float( pos.x - d.z );
	temp.z = float( pos.z + d.x );

	d = temp - pos;

	d.normalize();

	pos.x = pos.x + d.x * speed;
	pos.z = pos.z + d.z * speed;
	view.x = view.x + d.x * speed;
	view.z = view.z + d.z * speed;

}



void Camera::Position( float posX, float posY, float posZ, 
					    float viewX, float viewY, float viewZ,
						float upX, float upY, float upZ ) {

	pos		= Vector3( posX, posY, posZ );
	view	= Vector3( viewX, viewY, viewZ );
	up		= Vector3( upX, upY, upZ );

}

void Camera::Rotate( float x, float y, float z ) {

	Vector3 d = view - pos;

	//d.Normalize();

	if( x ) {
		
		view.y = float( pos.y + cos( x ) * d.y - sin( x ) * d.z );
		view.z = float( pos.z + sin( x ) * d.y + cos( x ) * d.z );

	}

	if( y ) {

		view.x = float( pos.x + cos( y ) * d.x - sin( y ) * d.z );
		view.z = float( pos.z + sin( y ) * d.x + cos( y ) * d.z );

	}

	if( z ) {

		view.x = float( pos.x + sin( z ) * d.y + cos( z ) * d.x );
		view.y = float( pos.y + cos( z ) * d.y - sin( z ) * d.x );

	}
}

void Camera::CalculateViewingFrustum( void ) {

	float mModelView[16];
	float mProjection[16];
	float mClip[16];
	float t;

	glGetFloatv( GL_MODELVIEW_MATRIX, mModelView );
	glGetFloatv( GL_PROJECTION_MATRIX, mProjection );

	//mClip = mProjection * mModelView;

	mClip[ 0] = mModelView[ 0] * mProjection[ 0] + mModelView[ 1] * mProjection[ 4] + mModelView[ 2] * mProjection[ 8] + mModelView[ 3] * mProjection[12];
    mClip[ 1] = mModelView[ 0] * mProjection[ 1] + mModelView[ 1] * mProjection[ 5] + mModelView[ 2] * mProjection[ 9] + mModelView[ 3] * mProjection[13];
    mClip[ 2] = mModelView[ 0] * mProjection[ 2] + mModelView[ 1] * mProjection[ 6] + mModelView[ 2] * mProjection[10] + mModelView[ 3] * mProjection[14];
    mClip[ 3] = mModelView[ 0] * mProjection[ 3] + mModelView[ 1] * mProjection[ 7] + mModelView[ 2] * mProjection[11] + mModelView[ 3] * mProjection[15];

    mClip[ 4] = mModelView[ 4] * mProjection[ 0] + mModelView[ 5] * mProjection[ 4] + mModelView[ 6] * mProjection[ 8] + mModelView[ 7] * mProjection[12];
    mClip[ 5] = mModelView[ 4] * mProjection[ 1] + mModelView[ 5] * mProjection[ 5] + mModelView[ 6] * mProjection[ 9] + mModelView[ 7] * mProjection[13];
    mClip[ 6] = mModelView[ 4] * mProjection[ 2] + mModelView[ 5] * mProjection[ 6] + mModelView[ 6] * mProjection[10] + mModelView[ 7] * mProjection[14];
    mClip[ 7] = mModelView[ 4] * mProjection[ 3] + mModelView[ 5] * mProjection[ 7] + mModelView[ 6] * mProjection[11] + mModelView[ 7] * mProjection[15];

    mClip[ 8] = mModelView[ 8] * mProjection[ 0] + mModelView[ 9] * mProjection[ 4] + mModelView[10] * mProjection[ 8] + mModelView[11] * mProjection[12];
    mClip[ 9] = mModelView[ 8] * mProjection[ 1] + mModelView[ 9] * mProjection[ 5] + mModelView[10] * mProjection[ 9] + mModelView[11] * mProjection[13];
    mClip[10] = mModelView[ 8] * mProjection[ 2] + mModelView[ 9] * mProjection[ 6] + mModelView[10] * mProjection[10] + mModelView[11] * mProjection[14];
    mClip[11] = mModelView[ 8] * mProjection[ 3] + mModelView[ 9] * mProjection[ 7] + mModelView[10] * mProjection[11] + mModelView[11] * mProjection[15];

    mClip[12] = mModelView[12] * mProjection[ 0] + mModelView[13] * mProjection[ 4] + mModelView[14] * mProjection[ 8] + mModelView[15] * mProjection[12];
    mClip[13] = mModelView[12] * mProjection[ 1] + mModelView[13] * mProjection[ 5] + mModelView[14] * mProjection[ 9] + mModelView[15] * mProjection[13];
    mClip[14] = mModelView[12] * mProjection[ 2] + mModelView[13] * mProjection[ 6] + mModelView[14] * mProjection[10] + mModelView[15] * mProjection[14];
    mClip[15] = mModelView[12] * mProjection[ 3] + mModelView[13] * mProjection[ 7] + mModelView[14] * mProjection[11] + mModelView[15] * mProjection[15];

	/* Extract the numbers for the RIGHT plane */
   fFrustum[0][0] = mClip[ 3] - mClip[ 0];
   fFrustum[0][1] = mClip[ 7] - mClip[ 4];
   fFrustum[0][2] = mClip[11] - mClip[ 8];
   fFrustum[0][3] = mClip[15] - mClip[12];

   /* Normalize the result */
   t = sqrt( fFrustum[0][0] * fFrustum[0][0] + fFrustum[0][1] * fFrustum[0][1] + fFrustum[0][2] * fFrustum[0][2] );
   fFrustum[0][0] /= t;
   fFrustum[0][1] /= t;
   fFrustum[0][2] /= t;
   fFrustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   fFrustum[1][0] = mClip[ 3] + mClip[ 0];
   fFrustum[1][1] = mClip[ 7] + mClip[ 4];
   fFrustum[1][2] = mClip[11] + mClip[ 8];
   fFrustum[1][3] = mClip[15] + mClip[12];

   /* Normalize the result */
   t = sqrt( fFrustum[1][0] * fFrustum[1][0] + fFrustum[1][1] * fFrustum[1][1] + fFrustum[1][2] * fFrustum[1][2] );
   fFrustum[1][0] /= t;
   fFrustum[1][1] /= t;
   fFrustum[1][2] /= t;
   fFrustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   fFrustum[2][0] = mClip[ 3] + mClip[ 1];
   fFrustum[2][1] = mClip[ 7] + mClip[ 5];
   fFrustum[2][2] = mClip[11] + mClip[ 9];
   fFrustum[2][3] = mClip[15] + mClip[13];

   /* Normalize the result */
   t = sqrt( fFrustum[2][0] * fFrustum[2][0] + fFrustum[2][1] * fFrustum[2][1] + fFrustum[2][2] * fFrustum[2][2] );
   fFrustum[2][0] /= t;
   fFrustum[2][1] /= t;
   fFrustum[2][2] /= t;
   fFrustum[2][3] /= t;

   /* Extract the TOP plane */
   fFrustum[3][0] = mClip[ 3] - mClip[ 1];
   fFrustum[3][1] = mClip[ 7] - mClip[ 5];
   fFrustum[3][2] = mClip[11] - mClip[ 9];
   fFrustum[3][3] = mClip[15] - mClip[13];

   /* Normalize the result */
   t = sqrt( fFrustum[3][0] * fFrustum[3][0] + fFrustum[3][1] * fFrustum[3][1] + fFrustum[3][2] * fFrustum[3][2] );
   fFrustum[3][0] /= t;
   fFrustum[3][1] /= t;
   fFrustum[3][2] /= t;
   fFrustum[3][3] /= t;

   /* Extract the FAR plane */
   fFrustum[4][0] = mClip[ 3] - mClip[ 2];
   fFrustum[4][1] = mClip[ 7] - mClip[ 6];
   fFrustum[4][2] = mClip[11] - mClip[10];
   fFrustum[4][3] = mClip[15] - mClip[14];

   /* Normalize the result */
   t = sqrt( fFrustum[4][0] * fFrustum[4][0] + fFrustum[4][1] * fFrustum[4][1] + fFrustum[4][2] * fFrustum[4][2] );
   fFrustum[4][0] /= t;
   fFrustum[4][1] /= t;
   fFrustum[4][2] /= t;
   fFrustum[4][3] /= t;

   /* Extract the NEAR plane */
   fFrustum[5][0] = mClip[ 3] + mClip[ 2];
   fFrustum[5][1] = mClip[ 7] + mClip[ 6];
   fFrustum[5][2] = mClip[11] + mClip[10];
   fFrustum[5][3] = mClip[15] + mClip[14];

   /* Normalize the result */
   t = sqrt( fFrustum[5][0] * fFrustum[5][0] + fFrustum[5][1] * fFrustum[5][1] + fFrustum[5][2] * fFrustum[5][2] );
   fFrustum[5][0] /= t;
   fFrustum[5][1] /= t;
   fFrustum[5][2] /= t;
   fFrustum[5][3] /= t;

}

bool Camera::VertexInFrustum( int x, int y, int z ) {

	for( int plane = 0; plane < 6; plane++ ) {

		// if dist from point to plane is negative
		// then the point is "behind" that plane
		// and hence NOT in the view frustum
		if( fFrustum[plane][0]*x + fFrustum[plane][1]*y + fFrustum[plane][2]*z + fFrustum[plane][3] <= 0 )
			return( false );

	}

	return( true );

}

bool Camera::BoxInFrustum( int x, int y, int z, float fSize ) {

	for( int plane = 0; plane < 6; plane++ ) {

		if( fFrustum[plane][0] * (x - fSize) + fFrustum[plane][1] * (y - fSize) + fFrustum[plane][2] * (z - fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x + fSize) + fFrustum[plane][1] * (y - fSize) + fFrustum[plane][2] * (z - fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x - fSize) + fFrustum[plane][1] * (y + fSize) + fFrustum[plane][2] * (z - fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x + fSize) + fFrustum[plane][1] * (y + fSize) + fFrustum[plane][2] * (z - fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x - fSize) + fFrustum[plane][1] * (y - fSize) + fFrustum[plane][2] * (z + fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x + fSize) + fFrustum[plane][1] * (y - fSize) + fFrustum[plane][2] * (z + fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x - fSize) + fFrustum[plane][1] * (y + fSize) + fFrustum[plane][2] * (z + fSize) + fFrustum[plane][3] > 0 )
			continue;
		if( fFrustum[plane][0] * (x + fSize) + fFrustum[plane][1] * (y + fSize) + fFrustum[plane][2] * (z + fSize) + fFrustum[plane][3] > 0 )
			continue;
		
		return( false );

	}

	return( true );

}

bool Camera::BBoxInFrustum( float x, float y, float z, float x2, float y2, float z2 ) {
	// Go through all of the corners of the box and check then again each plane
	// in the frustum.  If all of them are behind one of the planes, then it most
	// like is not in the frustum.
	for( int i = 0; i < 6; i++ ) {

		if( fFrustum[i][0] * x  + fFrustum[i][1] * y  + fFrustum[i][2] * z  + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x2 + fFrustum[i][1] * y  + fFrustum[i][2] * z  + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x  + fFrustum[i][1] * y2 + fFrustum[i][2] * z  + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x2 + fFrustum[i][1] * y2 + fFrustum[i][2] * z  + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x  + fFrustum[i][1] * y  + fFrustum[i][2] * z2 + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x2 + fFrustum[i][1] * y  + fFrustum[i][2] * z2 + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x  + fFrustum[i][1] * y2 + fFrustum[i][2] * z2 + fFrustum[i][3] > 0 )  continue;
		if( fFrustum[i][0] * x2 + fFrustum[i][1] * y2 + fFrustum[i][2] * z2 + fFrustum[i][3] > 0 )  continue;

		// If we get here, it isn't in the frustum
		return( false );
	}

	// Return a true for the box being inside of the frustum
	return( true );
}


	




