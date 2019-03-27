
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
	float4 colour : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

PixelInput ColourVertexShader(VertexInput input)
{
	PixelInput output;

	//Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	//Calculate the position of the vertex against the matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Pass colour as is to pixel shader
	output.colour = input.colour;

	return output;
}