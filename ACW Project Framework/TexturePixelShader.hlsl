
//Globals
Texture2D shaderTexture;
SamplerState sampleType;

//Type definitions
struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PixelInput input) : SV_TARGET
{
	float4 textureColour;

    //Receive pixel sample from the texture2D using the samplerstate and receive the colours needed
	textureColour = shaderTexture.Sample(sampleType, input.tex);

	return textureColour;
}