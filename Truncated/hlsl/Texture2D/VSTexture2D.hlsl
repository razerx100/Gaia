cbuffer CVertexBuf : register(b0, space0) {
	matrix transform;
};

struct VSOut {
	float2 uv : TexCoord;
	float4 position : SV_POSITION;
};

VSOut main(float3 data : Position, float2 uv : TexCoord) {
	VSOut obj;

	obj.position = mul(float4(data, 1.0f), transform);
	obj.uv = uv;

	return obj;
}
