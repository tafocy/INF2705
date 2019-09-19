#version 410

in Attribs {
    vec4 couleur;
} AttribsIn;

out vec4 FragColor;

void main( void )
{
    // la couleur du fragment est la couleur interpolée
    FragColor = AttribsIn.couleur;
}
