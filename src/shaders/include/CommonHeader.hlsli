float4 CalculateClipPosition(
    float3 position, float4x4 model, float4x4 view, float4x4 projection
) {
    return mul(float4(position, 1.0f), mul(mul(model, view), projection));
}