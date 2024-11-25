#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D textureDiffuse; // Diffuse texture
uniform float transparency;       // Transparency level

void main()
{
    vec4 texColor = texture(textureDiffuse, TexCoords);
    // Apply transparency to the texture color
    FragColor = vec4(texColor.rgb, texColor.a * transparency); 
}
