//#version 140
#version 330 core

//in vec4 vertex; // <vec2 position, vec2 texCoords>
in vec3 vPosition;
in vec2 vTexCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;
uniform vec4 color;

void main()
{
	float scale = 0.02f;
	TexCoords = vTexCoords;
	ParticleColor = color;
	gl_Position = projection * view * vec4((vPosition * scale) + offset, 1.0);
}
