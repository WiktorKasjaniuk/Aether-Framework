#version 460 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoords;

out vec2 v_texcoords;
out vec2 v_position;

uniform mat3 u_mvp;

void main()
{
	gl_Position = vec4(u_mvp * vec3(a_position, 1.0), 1.0);
	v_texcoords = a_texcoords;
	v_position = gl_Position.xy;
}