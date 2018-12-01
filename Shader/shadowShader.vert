#version 330 compatibility

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 viewProjection;
uniform mat4 viewView;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec3 vert = gl_Vertex.xyz;
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );

    vs_out.FragPos = vert;
    vs_out.Normal = normalize( gl_NormalMatrix * gl_Normal );
    vs_out.TexCoords = gl_MultiTexCoord0.st;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}