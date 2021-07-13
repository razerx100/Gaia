cbuffer ColorBuf : register(b1, space1) {
	float3 color;
};

float4 main() : SV_TARGET{
    return float4(color, 1.0f);
}
