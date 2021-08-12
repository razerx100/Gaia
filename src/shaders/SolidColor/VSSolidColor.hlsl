#include <CommonHeader.hlsli>

cbuffer Transforms : register(b0, space0) {
	matrix model;
    matrix view;
    matrix projection;
};

float4 main(float3 position : Position) : SV_Position {
    return CalculateClipPosition(position, model, view, projection);
}
