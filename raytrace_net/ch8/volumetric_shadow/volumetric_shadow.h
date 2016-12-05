#ifndef _VOLUMETRIC_SHADOW_H_
#define _VOLUMETRIC_SHADOW_H_ 

#include "Vector3.h"

// types needed
typedef Vector3 Point3;

struct vertex_t {

	float x, y, z, w;

};

struct triangle_t {

	int		index[3];
	int		norm_index;
	bool	back_face;

};

struct edge_t { 

	int		tri_index[2];
	int		vert_index[2];

};

struct light_t {

	Vector3	position;

};

struct box_t {

	Vector3	max, min;

};

struct sphere_t {

	vertex_t	center;
	float		radius;

};

struct model_t {

	Vector3		* vertex_list;
	edge_t		* edge_list;
	triangle_t	* tri_list;
	Vector3		* normal_list;

	int			n_norms;
	int			n_tris;
	int			n_edges;
	int			n_verts;

	sphere_t	bounding_sphere;
	box_t		bounding_box;

};



#endif