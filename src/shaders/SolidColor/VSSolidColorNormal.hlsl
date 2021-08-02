cbuffer Projection : register(b0, space0) {
	matrix viewWithProjection;
};

cbuffer TransformData : register(b1, space0) {
	matrix view;
};

struct VSOut {
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 position : SV_Position;
};

VSOut main(float3 position : Position, float3 normals : Normal){
    VSOut vso;

    vso.viewPos = (float3) mul(float4(position, 1.0f), view);
    vso.viewNormal = mul(normals, (float3x3) view);
    vso.position = mul(float4(position, 1.0f), viewWithProjection);

	return vso;
}
