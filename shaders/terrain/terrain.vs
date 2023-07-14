#version 410 core
layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texCoord;
layout (location = 3) in vec3 i_tangent;
layout (location = 4) in vec3 i_bitangent;

out float v_height;
out vec3 v_fragmentPosition;
out vec2 v_texCoords;
out vec3 v_tangentLightDir;
out vec3 v_tangentViewPos;
out vec3 v_tangentFragPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec4 u_clipPlane;
uniform vec3 u_cameraPosition;
uniform vec3 u_lightDirection;

out vec3 v_normalVector;

void main()
{
    v_height = i_position.y;
	v_fragmentPosition = vec3(u_model * vec4(i_position, 1.0f));
	v_normalVector = mat3(transpose(inverse(u_model))) * i_normal;
	v_normalVector = normalize(v_normalVector);
	
	mat3 normalMatrix = transpose(inverse(mat3(u_model)));
	vec3 T = normalize(normalMatrix * i_tangent);
    vec3 N = normalize(normalMatrix * i_normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	v_tangentLightDir = TBN * u_lightDirection;
    v_tangentViewPos  = TBN * u_cameraPosition;
    v_tangentFragPos  = TBN * v_fragmentPosition;
	
	vec4 worldPosition = u_model * vec4(i_position, 1.0f);
	gl_ClipDistance[0]= dot(worldPosition, u_clipPlane);
    v_fragmentPosition = (u_view * u_model * vec4(i_position, 1.0f)).xyz;
    v_texCoords = i_texCoord;
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
}