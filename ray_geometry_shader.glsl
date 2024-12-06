#version 330 core
layout(lines) in;               // Input: 2 vertices defining a line
layout(triangle_strip, max_vertices = 6) out; // Output: 6 vertices for a quad

uniform mat4 view;
uniform mat4 projection;
uniform float thickness; // Thickness of the ray

out vec3 fragColor; // Pass color to fragment shader

void main() {
    // Calculate the ray direction in view space
    vec3 rayDir = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);

    // Find a perpendicular vector for thickness (up vector in view space)
    vec3 up = vec3(0.0, 1.0, 0.0); 
    vec3 right = normalize(cross(rayDir, up)) * thickness;

    // Four corners of the thick line
    vec4 p1 = gl_in[0].gl_Position - vec4(right, 0.0); // Start left
    vec4 p2 = gl_in[0].gl_Position + vec4(right, 0.0); // Start right
    vec4 p3 = gl_in[1].gl_Position - vec4(right, 0.0); // End left
    vec4 p4 = gl_in[1].gl_Position + vec4(right, 0.0); // End right

    fragColor = vec3(0.0, 0.0, 1.0); // Blue color

    gl_Position = projection * view * p1; EmitVertex();
    gl_Position = projection * view * p2; EmitVertex();
    gl_Position = projection * view * p3; EmitVertex();
    gl_Position = projection * view * p4; EmitVertex();
    EndPrimitive();
}
