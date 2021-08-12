#include <CommonHeader.hlsli>

cbuffer Transforms : register(b0, space0) {
	matrix model;
    matrix view;
    matrix projection;
};

struct VSOut {
    float3 worldPosition : Position;
    float3 worldNormal : Normal;
    float2 uv : TexCoord;
    float4 position : SV_Position;
};

VSOut main(float3 position : Position, float3 normal : Normal,
            float2 uv : TexCoord){
    VSOut vso;

    vso.worldPosition = (float3) mul(float4(position, 1.0f), model);
    vso.worldNormal = mul(normal, (float3x3) model);
    vso.position = CalculateClipPosition(position, model, view, projection);
    vso.uv = uv;

	return vso;
}
