#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_textureCoords;

out vec3 v_normal;
out vec4 v_clipspaceCoords;
out vec2 v_textureCoords;
out vec4 v_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_textureCoords = i_textureCoords;
	v_normal = i_normal;
	

	v_clipspaceCoords = u_projection * u_view * u_model * vec4(i_position, 1.0);
	gl_Position = v_clipspaceCoords;
	v_position = gl_Position;
}