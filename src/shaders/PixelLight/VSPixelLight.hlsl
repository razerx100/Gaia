cbuffer Projection : register(b0, space0) {
	matrix modelWithProjection;
};

cbuffer TransformData : register(b1, space0) {
	matrix model;
    matrix view;
};

struct VSOut {
    float3 worldPos : Position;
    float3 normal : Normal;
    float4 position : SV_Position;
};

VSOut main(float3 position : Position, float3 normals : Normal){
    VSOut vso;

    vso.worldPos = (float3) mul(float4(position, 1.0f), model);
    vso.normal = mul(normals, (float3x3) model);
    vso.position = mul(float4(position, 1.0f), modelWithProjection);

	return vso;
}
