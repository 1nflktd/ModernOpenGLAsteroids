// #version 140
#version 330 core

in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    // descartar alpha pq gl blend n esta ok (pq ? n sei)
    vec4 texColor = texture(sprite, TexCoords);
    if (texColor.a < 0.1)
        discard;
    //color = (texture(sprite, TexCoords) * ParticleColor);
    color = texColor * ParticleColor;
}
