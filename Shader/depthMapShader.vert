#version 330 compatibility

void main( ){ 
	vec3 vert = gl_Vertex.xyz;
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );
}