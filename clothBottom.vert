#version 330 core
layout (location = 7) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0);
}