#version 460 core

layout(location = 0) out vec4 a_color;

in vec2 v_texcoords;
in vec2 v_position;

uniform float u_filter;
uniform sampler2D u_texture;
uniform vec4 u_color;

void main()
{ 
	// default
	if (u_filter == -1.0)
		a_color = u_color * texture(u_texture, v_texcoords);
		
	// night vision
	else if (u_filter == 0.0) {
		a_color.rba = vec3(0.0, 0.0, u_color.a);
		a_color.g = (u_color * texture(u_texture, v_texcoords)).g;
	}

	// grayscale
	else if (u_filter == 1.0) {
		a_color = u_color * texture(u_texture, v_texcoords);
		float avarage = (a_color.r + a_color.g + a_color.b) / 3.0;
		a_color.rgb = vec3(avarage);
		a_color.a = u_color.a;
	}

	// position
	else {
		a_color = u_color * texture(u_texture, v_texcoords);
		a_color.g *= v_position.y;
		a_color.b *= v_position.x;
	}
}