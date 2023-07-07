#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_textureCoords;

out vec3 v_normal;
out vec4 v_clipspaceCoords;
out vec2 v_textureCoords;
out vec3 v_toCameraVector;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform vec3 u_cameraPosition;

uniform float u_tiling;

void main()
{
	vec4 worldPosition = u_model * vec4(i_position, 1.0);
	v_toCameraVector = u_cameraPosition - worldPosition.xyz;

    v_textureCoords = i_textureCoords * u_tiling;
	v_normal = i_normal;

	v_clipspaceCoords = u_projection * u_view * u_model * vec4(i_position, 1.0);
	gl_Position = v_clipspaceCoords;
}