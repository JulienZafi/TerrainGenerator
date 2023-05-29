#version 410 core
layout (location = 0) in vec3 i_position;

out float v_height;
out vec3 v_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_height = i_position.z;
    v_position = (u_view * u_model * vec4(i_position, 1.0f)).xyz;
    gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
}