cbuffer CVertexBuf {
	matrix transform;
};

struct PSInput {
	float4 color : Color;
	float4 position : SV_POSITION;
};

PSInput main(float3 data : Position, float4 color : Color) {
	PSInput inp;
	inp.color = color;
	inp.position = mul(float4(data, 1.0f), transform);

	return inp;
}
