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
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

PixelInput LightVertexShader(VertexInput input)
{
	PixelInput output;

	//Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	//Calculate the position of the vertex against the matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Pass texture coords to pixel shader
	output.tex = input.tex;

	//Calculate normal vector against world matrix
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	//Normalize normal vector
	output.normal = normalize(output.normal);

	return output;
}