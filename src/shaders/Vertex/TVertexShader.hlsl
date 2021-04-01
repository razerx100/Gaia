/*cbuffer CVertexBuf : register(b0) {
	matrix transform;
};

float4 main(float3 data : Position) : SV_POSITION{
	return mul(transform, float4(data, 1.0f));
}*/

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput main(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color;

    return result;
}
