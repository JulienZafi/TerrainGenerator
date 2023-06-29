#version 410 core
layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoord;

out vec4 v_clipSpace;
out vec2 v_texturecoords;
out vec3 v_vertexToCamVector;
out vec3 v_lightToWaterVector;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_cameraPosition;
uniform vec3 u_lightPosition;

const float tiling = 4.0f;

void main()
{
    v_clipSpace = u_projection * u_view * u_model * vec4(i_position, 1.0f);
	gl_Position = v_clipSpace;
	v_texturecoords = vec2(i_position.x / 2.0f + 0.5f, i_position.y / 2.0f + 0.5f) * tiling;
	v_vertexToCamVector = u_cameraPosition - gl_Position.xyz;
}