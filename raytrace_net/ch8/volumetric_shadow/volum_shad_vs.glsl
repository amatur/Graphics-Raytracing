uniform vec4 light;


void main() {

	gl_Position = gl_ModelViewProjectionMatrix * ( gl_Vertex.w == 0.0 ? 
						       vec4( (gl_Vertex.xyz * light.w) - light.xyz, 0 ) :
						       gl_Vertex );

}	
    