#version 460 core

layout(location = 0) out vec4 a_color;

in vec2 v_texcoords;

uniform vec4 u_color;

void main()
{ 
	a_color = u_color;
}