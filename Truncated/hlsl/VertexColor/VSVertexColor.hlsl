cbuffer CVertexBuf : register(b0, space0) {
	matrix transform;
};

struct VSOut {
	float4 color : Color;
	float4 position : SV_POSITION;
};

VSOut main(float3 data : Position, float4 color : Color) {
	VSOut obj;

	obj.position = mul(float4(data, 1.0f), transform);
	obj.color = color;

	return obj;
}
