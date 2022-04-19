
#version 330 core

out vec4 FragColor;
in vec4 color;
in vec2 texCoord;
uniform sampler2D uSampler;

void main() {
    vec4 texColor = vec4(1.0,1.0,1.0,1.0);
    texColor = texture2D(uSampler, texCoord);
    if(texColor.a < 0.1) discard;
    FragColor = texColor * color;
}
