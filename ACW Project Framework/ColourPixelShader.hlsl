
//Type definitions
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

float4 ColourPixelShader(PixelInput input) : SV_TARGET
{
	return input.colour;
}