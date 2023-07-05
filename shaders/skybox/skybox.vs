#version 410 core
layout (location = 0) in vec3 i_position;

out vec3 v_texCoords;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    v_texCoords = i_position;
    gl_Position = u_projection * u_view * vec4(i_position, 1.0);
}  