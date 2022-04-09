
#version 330 core

out vec4 FragColor;
varying vec4 color;
varying vec2 texCoord;
uniform sampler2D uSampler;

void main() {
    vec4 texColor = vec4(1.0,1.0,1.0,1.0);
    texColor = texture2D(uSampler, texCoord) * 0.8;
    //FragColor = vec4(color.xyz, 1.0);
    FragColor = texColor + vec4(color.xyz, 1.0) * 0.2;
}
