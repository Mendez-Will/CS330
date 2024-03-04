#version 420 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 TexCoord;

// Texture sampler
uniform sampler2D texture1;
uniform sampler2D texture2;

// Speficies number of textures
uniform int numTextures; 

void main()
{
    if (numTextures == 1)
    {
        FragColor = texture(texture1, TexCoord) * ourColor;
    }
    else
    {
        FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5) * ourColor;
    }
}
