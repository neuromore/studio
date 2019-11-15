#version 120

attribute float width;
attribute float height;
attribute float numElectrodes;

varying float widthV;
varying float heightV;
varying float numElectrodesV;

void main()
{
	// Passing variables
    widthV = width;
    heightV = height;
    numElectrodesV = numElectrodes;
    
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = ftransform();
}