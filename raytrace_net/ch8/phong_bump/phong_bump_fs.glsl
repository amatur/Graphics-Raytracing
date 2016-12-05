uniform sampler2D   color_map;
uniform sampler2D   normal_map;

uniform float kd;
uniform float ks;
uniform float ka;
uniform float spec_exp;

varying vec3 light1_vec;
varying vec3 light2_vec;

varying vec3 half_angle1;
varying vec3 half_angle2;

void main() {

   vec3 color      = texture2D( color_map, gl_TexCoord[0].xy ).rgb;
   vec3 normal     = texture2D( normal_map, gl_TexCoord[0].xy ).xyz;
   
   normal = normalize( normal * 2.0 - 1.0 );
   
   float diffuse   = kd * max( dot( normal, normalize( light1_vec ) ), 0.0 );
   diffuse   	  += kd * max( dot( normal, normalize( light2_vec ) ), 0.0 );
   
   float spec = ks * pow( max( dot( normal, normalize( half_angle1 ) ), 0.0 ), spec_exp );
   spec      += ks * pow( max( dot( normal, normalize( half_angle2 ) ), 0.0 ), spec_exp );
   
   color = ( color * ka ) + ( color * diffuse ) + spec;
   
   gl_FragColor = vec4( clamp( color, 0.0, 1.0 ), 1.0 );
   
}

