cbuffer ColorBuf : register(b0, space1) {
	float4 color;
};

float4 main() : SV_TARGET{
    return color;
}
