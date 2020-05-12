#version 330 core
layout (location = 5) in vec3 aPos;
layout (location = 6) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoord;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}