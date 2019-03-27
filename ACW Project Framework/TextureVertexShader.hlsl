//Global
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//Type definitions
struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInput TextureVertexShader(VertexInput input)
{
	PixelInput output;

	//Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	//Calculate the position of the vertex against the matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Pass colour as is to pixel shader
	output.tex = input.tex;

	return output;
}