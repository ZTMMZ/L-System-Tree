#version 330 compatibility
uniform float uLightX, uLightY, uLightZ;
uniform sampler2D uTerrainTex;
uniform float uDelta;
uniform float uRatio;
uniform vec4 uColor;
flat out vec3 vNf;
out vec3 vNs;
flat out vec3 vLf;
out vec3 vLs;
flat out vec3 vEf;
out vec3 vEs;

out vec4 vColor;
out vec2 vST;
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
	

	// Surface normal T
	float heightOffset = texture2D(uTerrainTex,vST).r/uRatio;
	vMCposition = vec3 ( vMCposition.x, vMCposition.y + heightOffset, vMCposition.z);
	float s = vST.s;
	float t = vST.t;
	vec2 stp0 = vec2( DELTA, 0. );
	vec2 st0p = vec2( 0., DELTA );
	float west = texture2D( uTerrainTex, vST-stp0 ).r/uRatio;
	float east = texture2D( uTerrainTex, vST+stp0 ).r/uRatio;
	float south = texture2D( uTerrainTex, vST-st0p ).r/uRatio;
	float north = texture2D( uTerrainTex, vST+st0p ).r/uRatio;
	vec3 stangent = vec3( DELTA, ( east - west ), 0. );
	vec3 ttangent = vec3( 0., DELTA,( north - south ) );
	vNf = normalize(  cross( stangent, ttangent )  );
	vNs = vNf;

	gl_Position = gl_ModelViewProjectionMatrix * vec4( vMCposition, gl_Vertex.w );

}
