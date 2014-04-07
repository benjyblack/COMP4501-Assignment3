#version 130

attribute vec3 vertex;
attribute vec2 textureCoordinate;
attribute vec3 normal;
	
uniform mat4x4 cameraSpaceToLPS;
uniform vec3 lightPositionCS;
//uniform vec3 lightDirectionCS; //Used in pixel shader...

//Base texture variables.
out vec4 clipSpace; 
out vec2 texCoord;

//Lighting variables...
out vec3 pixelPositionCS;
out vec3 pixelNormalCS;
out vec3 pixelToLightCS;
out vec3 pixelToCameraCS;

//Shadow variables...
out vec4 pixelPositionCPS;
out vec4 pixelPositionLPS;

#include "WilfShadowExperiment.choice"

void main () {
	texCoord = textureCoordinate; //Pass-through the texture coordinates for the base texture.
	
	gl_Position = clipSpace = gl_ModelViewProjectionMatrix * vec4 (vertex, 1.0); 
		
	//Lighting variables...; recall combinedColor (vec4 textureColor, vec3 pixelNormalCS, vec3 pixelToLightCS, vec3 pixelToCameraCS, float shadowBrightness).
	pixelPositionCS = (gl_ModelViewMatrix * vec4 (vertex, 1)).xyz;
	
	pixelNormalCS = gl_NormalMatrix * normal;
	pixelToLightCS = lightPositionCS - pixelPositionCS;
	pixelToCameraCS = /*vec3 (0,0,0)*/ - pixelPositionCS; 
	
	//Shadow specific variables.
	pixelPositionCPS = clipSpace; //CPS = camera projection space.
	pixelPositionLPS = cameraSpaceToLPS * vec4 (pixelPositionCS, 1.0); //LPS = light projection space.
	
}
