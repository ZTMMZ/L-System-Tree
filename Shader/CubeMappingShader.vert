#version 330 compatibility

out vec3 vSTP;

void main() {
    vSTP = gl_Vertex.xyz;
    gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;  

}
