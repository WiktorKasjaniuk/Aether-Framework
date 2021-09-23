#version 460 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoords;

uniform mat3 u_vp;
uniform vec2 u_positions[20];

void main()
{
	gl_Position = vec4(u_vp * vec3(u_positions[gl_InstanceID] + a_position, 1.0), 1.0);
}