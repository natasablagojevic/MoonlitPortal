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

uniform sampler2D image;
uniform bool horizontal;

const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texOff = 1.0 / vec2(textureSize(image, 0));
    vec3 result = texture(image, TexCoords).rgb * weight[0];

    for (int i = 1; i < 5; ++i) {
        if (horizontal) {
            result += texture(image, TexCoords + vec2(texOff.x * i, 0.0f)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(texOff.x * i, 0.0f)).rgb * weight[i];
        } else {
            result += texture(image, TexCoords + vec2(0.0f, texOff.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0f, texOff.y * i)).rgb * weight[i];
        }
    }

//    if (horizontal) {
//        for (int i = 1; i < 5; ++i) {
//            result += texture(image, TexCoords + vec2(texOff.x * i, 0.0f)).rgb * weight[i];
//            result += texture(image, TexCoords - vec2(texOff.x * i, 0.0f)).rgb * weight[i];
//        }
//    } else {
//        for (int i = 1; i < 5; ++i) {
//            result += texture(image, TexCoords + vec2(0.0f, texOff.y * i)).rgb * weight[i];
//            result += texture(image, TexCoords - vec2(0.0f, texOff.y * i)).rgb * weight[i];
//        }
//    }


    FragColor = vec4(result, 1.0f);

}
