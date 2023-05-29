#version 410 core

out vec4 f_fragColor;

in float v_height;

void main()
{
    float h = (v_height + 16.0f) / 32.0f;	// shift and scale the height in to a grayscale value
    f_fragColor = vec4(h, h, h, 1.0f);
}