uniform vec3 light1_dir;
uniform vec3 light2_dir;

uniform vec3 eye_pos;

varying vec3 light1_vec;
varying vec3 light2_vec;

varying vec3 half_angle1;
varying vec3 half_angle2;

attribute vec3 tan;
attribute vec3 norm;
attribute vec2 tex;

void main() {

   gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;
   gl_TexCoord[0] = vec4( tex, 0.0, 0.0 );
   
   vec3 normal   = ( /*gl_NormalMatrix **/ norm ).xyz;
   vec3 tangent  = ( /*gl_NormalMatrix **/ tan ).xyz;
   vec3 binormal = cross( normal, tangent );
   
   vec3 v_temp   = eye_pos - gl_Vertex.xyz;

   vec3 l1_temp   = light1_dir - gl_Vertex.xyz;
   vec3 l2_temp   = light2_dir - gl_Vertex.xyz;

   vec3 view_vec = vec3( dot( tangent, v_temp ),
			 dot( binormal, v_temp ),
			 dot( normal, v_temp ) );
   
   light1_vec.x   = dot( tangent, l1_temp );
   light1_vec.y   = dot( binormal, l1_temp );
   light1_vec.z   = dot( normal, l1_temp );

   light2_vec.x   = dot( tangent, l2_temp );
   light2_vec.y   = dot( binormal, l2_temp );
   light2_vec.z   = dot( normal, l2_temp );
   
   half_angle1	 = light1_vec + view_vec;
   half_angle2	 = light2_vec + view_vec;
 
}


