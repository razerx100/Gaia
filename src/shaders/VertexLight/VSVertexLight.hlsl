cbuffer CVertexBuf : register(b0, space0) {
	matrix transform;
};

cbuffer CVertexBuff1 : register(b1, space0) {
    float3 dir;
}

static const float3 diffuse = { 1.0f, 1.0f, 1.0f };
static const float3 ambient = { 0.3f, 0.3f, 0.3f };
static const float3 color = { 0.8f, 0.85f, 1.0f };

struct VSOut {
    float3 color : Color;
    float4 position : SV_Position;
};

VSOut main(float3 data : Position, float3 normals : Normal){
    float3 d = diffuse * max(0.0f, dot(mul(normals, (float3x3)transform), dir));
    float3 c = saturate(color * (d + ambient));

    VSOut obj;
    obj.color = c;
    obj.position = mul(float4(data, 1.0f), transform);

	return obj;
}
