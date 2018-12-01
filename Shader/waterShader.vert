#version 330 compatibility

uniform vec4 uColor;
uniform float uLightX, uLightY, uLightZ;
uniform sampler2D uNoise2;
uniform float uDelta;
uniform float uRatio;
uniform float uNoiseAmp;
uniform float uNoiseFreq;
uniform float uTexRepeat;
uniform float uTimer;

flat out vec3 vNf;
out vec3 vNs;
flat out vec3 vLf;
out vec3 vLs;
flat out vec3 vEf;
out vec3 vEs;
out vec2 vST;
out vec4 vColor;
out vec3 vReflectVector;
out float fogFactor;

out vec4 vECposition;
out vec3 vLightPos;

const float PI = 3.14159265;

void main( ) {
	vLightPos = vec3( uLightX, uLightY, uLightZ );
	float DELTA = 1./uDelta;

	// Vertex in Eye Coordination
	vECposition = gl_ModelViewMatrix * gl_Vertex;
	
	// Vector from the Point to the Light Position
	vLf = vLightPos - vECposition.xyz;
	vLs = vLf;
	
	// Vector from the Point to the Eye Position 
	vEf = vec3( 0., 0., 0. ) - vECposition.xyz;
	vEs = vEf;
	
	// Vertex Color
	vColor = uColor;

	//Texture Coordinate
	vST = gl_MultiTexCoord0.st;

	// Vertex in Model Coordination
	vec3 vMCposition = gl_Vertex.xyz;
	
	float offset =uTimer/uNoiseFreq;

	// Surface normal 
	float heightOffset = 4* uNoiseAmp * texture2D(uNoise2,uNoiseFreq*vec2(vST.s,vST.t + offset)).r/uRatio;
	vMCposition = vec3 ( vMCposition.x, vMCposition.y+ heightOffset, vMCposition.z);
	float s = vST.s;
	float t = vST.t;
	vec2 stp0 = vec2( DELTA, 0. );
	vec2 st0p = vec2( 0., DELTA );
	float west = uNoiseAmp * texture2D( uNoise2, uNoiseFreq*(vec2(vST.s + offset,vST.t + offset)-stp0) ).r/uRatio;
	float east = uNoiseAmp * texture2D( uNoise2, uNoiseFreq*(vec2(vST.s + offset,vST.t + offset)+stp0) ).r/uRatio;
	float south = uNoiseAmp * texture2D( uNoise2, uNoiseFreq*(vec2(vST.s + offset,vST.t + offset)-st0p) ).r/uRatio;
	float north = uNoiseAmp * texture2D( uNoise2, uNoiseFreq*(vec2(vST.s + offset,vST.t + offset)+st0p) ).r/uRatio;
	vec3 stangent = vec3( DELTA, 0., ( east - west ) );
	vec3 ttangent = vec3( 0., DELTA,( north - south ) );
	vNf = normalize(  cross( stangent, ttangent )  );
	vNs = vNf;

	vReflectVector = reflect( -vEs, vNs );
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vMCposition, gl_Vertex.w );

	float LOG2 = 1.442695;
    float fogDensity = 0.5;
    float gl_FogFragCoord = abs(gl_Position.z);
    fogFactor = exp2( -fogDensity * fogDensity * gl_FogFragCoord * gl_FogFragCoord * LOG2 );
    fogFactor = clamp(fogFactor, 0.0, 1.0);

}
