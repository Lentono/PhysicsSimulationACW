
//Globals
Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
	float4 diffuseColour;
	float3 lightDirection;
	float padding;
};

//Type definitions
struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 LightPixelShader(PixelInput input) : SV_TARGET
{
	float4 textureColour;
float lightIntensity;
float4 colour;

	//Receive pixel sample from the texture2D using the samplerstate and receive the colours needed
	textureColour = shaderTexture.Sample(sampleType, input.tex);

	//Invert the light direction
	//lightDirection = -lightDirection;

	//Calculate light on pixel
	lightIntensity = saturate(dot(input.normal, -lightDirection));

	//Get final colour
	colour = saturate(diffuseColour * lightIntensity);

	//Combine colour and texture colour
	colour = colour * textureColour;

	return colour;
}