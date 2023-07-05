#version 410 core
layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_texCoord;

out float v_height;
out vec3 v_position;
out vec2 v_texCoords;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec4 u_clipPlane;

out vec3 v_fragmentPosition;
out vec3 v_normalVector;

void main()
{
    v_height = i_position.y;
	v_fragmentPosition = vec3(u_model * vec4(i_position, 1.0f));
	v_normalVector = mat3(transpose(inverse(u_model))) * i_normal;
	v_normalVector = normalize(v_normalVector);
	
	vec4 worldPosition = u_model * vec4(i_position, 1.0f);
	gl_ClipDistance[0]= dot(worldPosition, u_clipPlane);
    v_position = (u_view * u_model * vec4(i_position, 1.0f)).xyz;
    v_texCoords = i_texCoord;
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
}