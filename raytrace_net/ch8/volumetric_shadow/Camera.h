#ifndef _Camera_H
#define _Camera_H

#include "Vector3.h"

#define FRUSTUM_RIGHT	0
#define FRUSTUM_LEFT	1
#define FRUSTUM_BOTTOM	2
#define FRUSTUM_TOP		3
#define FRUSTUM_FAR		4
#define FRUSTUM_NEAR	5

class Camera {

public:

	Camera( void ) {}
	Camera( float posX, float posY, float posZ, 
			 float viewX, float viewY, float viewZ,
			 float upX, float upY, float upZ );
	~Camera( void ) {}

	// co-ordinates
	Vector3 up, view, pos;

	// viewing frustum
	float fFrustum[ 6 ][ 4 ];

	// frustum functions
	void CalculateViewingFrustum( void );
	bool VertexInFrustum( int x, int y, int z );
	bool BoxInFrustum( int x, int y, int z, float fSize );
	bool BBoxInFrustum( float x, float y, float z, float x2, float y2, float z2 );

	// movement functions
	void Rotate( float x, float y, float z );
	void Move( float speed );
	void Strafe( float speed );

	// initialize / change the camera's position and view
	void Position( float posX, float posY, float posZ, 
				   float viewX, float viewY, float viewZ,
				   float upX, float upY, float upZ );

};

#endif