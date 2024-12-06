#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;  // In case you use textures

uniform vec3 objectColor;  // The color of the object (you can use textures if needed)
uniform sampler2D texture1;  // Optional: Texture if you want to map it

void main() {
    // Simply output the object's color or texture
    FragColor = texture(texture1, TexCoords) * vec4(objectColor, 1.0);
}
