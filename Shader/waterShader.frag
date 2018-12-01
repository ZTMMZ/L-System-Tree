#version 330 compatibility

uniform float uKa, uKd, uKs;
uniform vec4 uSpecularColor;
uniform vec4 uAmbientColor;
uniform vec4 uDiffuseColor;
uniform float uShininess;
uniform bool uFlat;
uniform sampler2D uTerrainContrastTex;
uniform sampler2D uReflectTex;
uniform float uAttenuation;

flat in vec3 vNf;
in vec3 vNs;
flat in vec3 vLf;
in vec3 vLs;
flat in vec3 vEf;
in vec3 vEs;
in vec4 vColor;
in vec2 vST;
in vec3 vReflectVector;
in float fogFactor;
in vec4  vECposition;
in vec3  vLightPos;

void main( ) {
	
	float terrainDepth = texture2D(uTerrainContrastTex,vST).r;
	vec3 reflectColor = texture(uReflectTex,vST).rgb;
	//vec3 spoondriftColor = vec3(0.11, 0.75, 0.68);
	gl_FragColor = vec4(mix(vColor.rgb, texture(uReflectTex,vST).rgb,0.5), 1-terrainDepth-0.6);
	//vec3 newColor = mix( spoondriftColor, mix (vColor.rgb, reflectColor ,0.5), terrainDepth-0.3);
	//gl_FragColor = vec4(newColor, terrainDepth-0.4);

	//Lighting
	vec3 Normal;
	vec3 Light;
	vec3 Eye;

	if( uFlat ) {
		Normal = normalize(vNf);
		Light = normalize(vLf);
		Eye = normalize(vEf);
	} else {
		Normal = normalize(vNs);
		Light = normalize(vLs);
		Eye = normalize(vEs);
	}

	vec4 ambient = uKa * gl_FragColor * uAmbientColor;

	float attenuation = uAttenuation * (1.0 / distance(vLightPos,vECposition.xyz));
	float d = max( dot(Normal,Light), 0. );
	vec4 diffuse = uKd * d * gl_FragColor * uDiffuseColor * attenuation;

	float sp = 0.;
	if( dot(Normal,Light) > 0. ) {
		vec3 ref = normalize( 2. * Normal * dot(Normal,Light) - Light );
		sp = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec4 specular = uKs * sp * uSpecularColor;

	gl_FragColor = vec4( ambient.rgb + diffuse.rgb + specular.rgb, gl_FragColor.a );

	vec4 fogColor = vec4(0.0,0.4,0.6,1.0);
    gl_FragColor = mix(fogColor, gl_FragColor, fogFactor ); 

}