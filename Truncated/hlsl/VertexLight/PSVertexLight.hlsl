cbuffer ColorBuf : register(b0, space1) {
	float4 faceColors[6];
};

float4 main(float4 light : Light, uint id : SV_PrimitiveID) : SV_TARGET{
    return saturate(light * faceColors[(id / 2) % 6]);
}
