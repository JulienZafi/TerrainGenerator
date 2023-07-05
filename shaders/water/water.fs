#version 410 core

#define SATURATE(X) clamp(X, 0.0, 1.0)

in vec3 v_normal;
in vec4 v_clipspaceCoords;
in vec2 v_textureCoords;
in vec4 v_position;

out vec4 fragColor;

uniform vec3 u_cameraPosition;
uniform vec3 u_lightColor;
uniform vec3 u_lightPosition;

uniform sampler2D u_reflectionTexture;
uniform sampler2D u_refractionTexture;
uniform sampler2D u_dudvmap;
uniform sampler2D u_normalMap;
uniform sampler2D u_depthMap;

uniform float u_waveLevel;
uniform float u_moveFactor;
uniform float u_distanceFactor;
uniform float u_grain;
uniform float u_specularFactor;


/*
* Utility functions
*/
float random3D(in vec3 st)
{
	return fract(sin(dot(st.xyz, vec3(12.9898, 78.233, 141.1525))) * 43758.5453123);
}

float interpolate(float a, float b, float x) {  // cosine interpolation
	float ft = x * 3.1415927f;
	float f = (1. - cos(ft)) * 0.5;
	return  a * (1. - f) + b * f;
}

float interpolatedNoise(int ind, float x, float y, float z) 
{
	int integer_X = int(floor(x));
	float fractional_X = fract(x);
	int integer_Y = int(floor(y));
	float fractional_Y = fract(y);
    int integer_Z = int(floor(z));
    float fractional_Z = fract(z);
    
	vec3 randomInput = vec3(integer_X, integer_Y, integer_Z);
	float v1 = random3D(randomInput + vec3(0.0, 0.0, 0.0));
	float v2 = random3D(randomInput + vec3(1.0, 0.0, 0.0));
	float v3 = random3D(randomInput + vec3(0.0, 1.0, 0.0));
	float v4 = random3D(randomInput + vec3(1.0, 1.0, 0.0));
    
    float v5 = random3D(randomInput + vec3(0.0, 0.0, 1.0));
	float v6 = random3D(randomInput + vec3(1.0, 0.0, 1.0));
	float v7 = random3D(randomInput + vec3(0.0, 1.0, 1.0));
	float v8 = random3D(randomInput + vec3(1.0, 1.0, 1.0));
    
    
	float i1 = interpolate(v1, v2, fractional_X);
	float i2 = interpolate(v3, v4,  fractional_X);
    
    float i3 = interpolate(v5, v6, fractional_X);
    float i4 = interpolate(v7, v8, fractional_X);
    
    float y1 = interpolate(i1, i2, fractional_Y);
    float y2 = interpolate(i3, i4, fractional_Y);
    
    
	return interpolate(y1, y2, fractional_Z);
}

float perlin(float x, float y, float z){
	
    int numOctaves = 3;
	float persistence = 0.5;
	float total = 0.,
		frequency = 0.025,
		amplitude = 6.0;
	for (int i = 0; i < numOctaves; ++i) {
		frequency *= 2.;
		amplitude *= persistence;
		
		x += u_moveFactor * 13.0;
		y += u_moveFactor * 7.0;

		total += interpolatedNoise(0, x * frequency, y * frequency, z * frequency) * amplitude;
		//total += interpolatedNoise(0, y * frequency, x * frequency, (z+0.5) * frequency) * amplitude;
	}
	
	return pow(total, 1.0);
}

vec3 computeNormals(vec3 WorldPos)
{
	float st = 0.35;
	float dhdu = (perlin((WorldPos.x + st), WorldPos.z, u_moveFactor*10.0) - perlin((WorldPos.x - st), WorldPos.z, u_moveFactor*10.0))/(2.0*st);
	float dhdv = (perlin( WorldPos.x, (WorldPos.z + st), u_moveFactor*10.0) - perlin(WorldPos.x, (WorldPos.z - st), u_moveFactor*10.0))/(2.0*st);

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	vec3 norm = mix(n, v_normal, 0.5); 
	norm = normalize(norm);
	return norm;
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1,311.7, 194.1729)),dot(p,vec3(269.5,183.3, 72.0192)), dot(p,vec3(183.3,72.0192,311.7))))*43758.5453);
}

float worley(vec3 st) {
	float color = 0.0;

    // Scale
    st *= 0.5;

    // Tile the space
    vec3 i_st = floor(st);
    vec3 f_st = fract(st);

    float m_dist = 1.;  // minimun distance

    for (int y= -1; y <= 1; y++) 
	{
        for (int x= -1; x <= 1; x++) 
		{
			for (int z = -1; z<=1 ; z++) 
			{
				// Neighbor place in the grid
				vec3 neighbor = vec3(float(x),float(y), float(z));

				// Random position from current + neighbor place in the grid
				vec3 point = random3(i_st + neighbor);


				// Vector between the pixel and the point
				vec3 diff = neighbor + point - f_st;

				// Distance to the point
				float dist = pow(length(diff), 1.0);

				// Keep the closer distance
				m_dist = min(m_dist, dist);
			}
        }
    }

    // Draw the min distance (distance field)
    color += m_dist;

    return color;
}

void main()
{   
    float distFromPos = distance(v_position.xyz, u_cameraPosition); 
	vec2 fogDist = vec2(4000.0, 12000.0);
	float fogFactor = SATURATE((fogDist.y - distFromPos) / (fogDist.y - fogDist.x));

	vec2 ndc = (v_clipspaceCoords.xy / v_clipspaceCoords.w) / 2.0 + 0.5;
	vec2 distortion1 = texture(u_dudvmap, vec2(v_textureCoords.x + u_moveFactor, v_textureCoords.y) * u_grain).rg * 2.0 - 1.0;
	vec2 distortion2 = texture(u_dudvmap, vec2(v_textureCoords.x + u_moveFactor, v_textureCoords.y - u_moveFactor) * u_grain).rg * 2.0 - 1.0;
	vec2 totalDistortion = distortion1 + distortion2;
	
	float dhdu = (perlin((v_position.x + u_waveLevel), v_position.z, u_moveFactor * 10.0) - perlin((v_position.x - u_waveLevel), v_position.z, u_moveFactor * 10.0))/(2.0 * u_waveLevel);
	float dhdv = (perlin(v_position.x, (v_position.z + u_waveLevel), u_moveFactor * 10.0) - perlin(v_position.x, (v_position.z - u_waveLevel), u_moveFactor * 10.0))/(2.0 * u_waveLevel);

	float floorY = texture(u_refractionTexture, ndc).a;
	float waterDepth = 1.0 - floorY;
	float waterDepthClamped = SATURATE(waterDepth * 5.0);
	
	totalDistortion = vec2(dhdu, dhdv) * u_distanceFactor * waterDepth;

	vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y);
	reflectionTexCoords += totalDistortion;
	reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
	reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.999, -0.001);
	vec4 reflectionColor = texture(u_reflectionTexture, reflectionTexCoords);

	vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
	refractionTexCoords += totalDistortion;
	refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);
	vec4 refractionColor = texture(u_refractionTexture, refractionTexCoords);
		
	vec3 toCameraVector =  v_position.xyz - u_cameraPosition;
	float fresnelFactor = max(dot(normalize(-toCameraVector), vec3(0.0, 1.0, 0.0)), 0.0);
	fresnelFactor = pow(fresnelFactor, 2.0);
	vec4 refr_reflCol = mix(reflectionColor, refractionColor, fresnelFactor);
	
	// calculate diffuse illumination
	totalDistortion = normalize(totalDistortion);
	vec3 X = vec3(1.0, totalDistortion.r, 1.0);
	vec3 Z = vec3(0.0, totalDistortion.g, 1.0);
	vec3 norm = texture(u_normalMap, totalDistortion).rgb;
	norm = vec3(norm.r * 2 - 1, norm.b * 1.5, norm.g * 2 - 1);
	norm = computeNormals(v_position.xyz);
	norm = mix(norm, vec3(0.0, 1.0, 0.0), 0.25);
	vec3 lightDir = normalize(u_lightPosition - v_position.xyz);
	float diffuseFactor = max(0.0, dot(lightDir, norm.rgb));
	float diffuseConst = 0.5;
	vec3 diffuse = diffuseFactor * diffuseConst * u_lightColor;
	
	// calculate specular illumination 
	vec3 viewDir = normalize(u_cameraPosition - v_position.xyz);
	vec3 reflectDir = reflect(-lightDir,  normalize(mix(norm, normalize(viewDir * 0.8 + vec3(0.0, 1.0, 0.0)), 0.2)) );  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512.);
	vec3 specular = spec * u_lightColor * u_specularFactor;  
	
	// Combine Light
	vec4 color = vec4(0.2, 0.71, 0.85, 1.0);
	
	vec4 fogColor = vec4(u_lightColor.x, u_lightColor.y, u_lightColor.z, 1.0);
	fragColor =  mix(mix(refr_reflCol, color * 0.8, 0.1) * 0.8 + vec4(diffuse + specular, 1.0), fogColor, (1 - fogFactor));
	float foam = perlin(v_position.x * 4.0, v_position.z * 4.0, u_moveFactor * 10.0 ) * 0.25;
	foam = mix(foam*pow((1.0 - waterDepth), 8.0), foam * 0.01, 0.0);
	fragColor.rgb *= 0.95;
	fragColor.a = waterDepthClamped;
	fragColor = refr_reflCol;
}
