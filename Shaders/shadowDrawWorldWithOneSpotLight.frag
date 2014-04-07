#version 130

#include "WilfShadowExperiment.choice"
      
uniform sampler2D Base;
uniform sampler2DShadow ShadowMap; //WhatLightSeesTexture; 
//uniform mat4x4 cameraSpaceToLPS; //Used in vertex shader...
uniform vec3 lightDirectionCS;

in vec4 clipSpace; 
in vec2 texCoord;

//Lighting variables...
in vec3 pixelPositionCS;
in vec3 pixelNormalCS;
in vec3 pixelToLightCS;
in vec3 pixelToCameraCS;

//Shadow variables...
in vec4 pixelPositionCPS;
in vec4 pixelPositionLPS;
uniform float coneAngleAsCosine;

out vec4 finalColor; 

#define USING_SHADOW_DRAWING
#include "WilfLightingLibrarySlightlyModified.all"
#include "WilfShadowMappingLibrary.all"

#define normalizeAllLightDirections() \
	/* Normalize all light directions... since interpolation changes lengths...*/ \
	vec3 normalCS = normalize (pixelNormalCS); \
	vec3 toLightCS = normalize (pixelToLightCS); \
	vec3 toCameraCS = normalize (pixelToCameraCS);

void main () {
	normalizeAllLightDirections ();
	vec4 textureColor = texture2D (Base, texCoord); //Pick up the base texture color...
		
	//Compute shadowBrightness via amountInShadowViaShadow2DProj...

	//Recall: canSeeLight (vec3 pixelToLightCS, vec3 pixelNormalCS, vec3 normalizedSpotlightDirection, float coneAngleAsCosine)
	float shadowBrightness = 
		canSeeLight (toLightCS, normalCS, lightDirectionCS, coneAngleAsCosine)
			? 0.5 + amountInShadowViaShadow2DProj (ShadowMap, pixelPositionLPS, LARGE_SHADOW_EPSILON)
			: 0.0; 
			
	//Compute the final color from the library routine... which requires all directions to be normalized...		
	//Recall: combinedColor (vec4 textureColor, vec3 pixelNormalCS, vec3 pixelToLightCS, vec3 pixelToCameraCS, float shadowBrightness).
	finalColor = combinedColor (textureColor, normalCS, toLightCS, toCameraCS, shadowBrightness);
}
