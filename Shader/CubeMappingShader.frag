#version 330 compatibility

in vec3 vSTP;

uniform samplerCube uCubeMap;

void main()
{
    gl_FragColor = texture(uCubeMap, vSTP);
}
