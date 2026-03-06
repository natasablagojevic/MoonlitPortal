//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

//#shader fragment
#version  330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D Scene;
uniform sampler2D Blur;

uniform bool useBloom;
uniform float exposure;
uniform float bloomIntensity;

void main()
{
    const float gamma = 1.25;

    vec3 base = texture(Scene, TexCoords).rgb;
    vec3 glow = texture(Blur, TexCoords).rgb;

    if (useBloom) {
        base += glow * bloomIntensity;
    }

    vec3 toneMapped = vec3(1.0f) - exp(-base * exposure);
    toneMapped = pow(toneMapped, vec3(1.0f / gamma));

    FragColor = vec4(glow, 1.0);
}