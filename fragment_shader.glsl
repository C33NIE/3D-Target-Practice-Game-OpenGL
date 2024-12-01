#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;    // Position of the light source
uniform vec3 viewPos;     // Position of the camera/viewer
uniform vec3 lightColor;  // Color of the light
uniform vec3 objectColor; // Color of the object

void main() {
    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    
    // Calculate the direction vector from the fragment to the light source
    vec3 lightDir = normalize(lightPos - FragPos);

    // Diffuse shading: Lambertian reflectance
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Ambient lighting (a small constant light)
    vec3 ambient = 0.1 * lightColor;

    // Combine results
    vec3 result = (ambient + diffuse) * objectColor;

    // Final output color
    FragColor = vec4(result, 1.0);
}
