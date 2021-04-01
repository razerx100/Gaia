cbuffer ColorBuf : register(b1) {
	float4 faceColors[6];
};
float4 main(uint id : SV_PrimitiveID) : SV_TARGET{
	return faceColors[id / 2];
}

/*struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(PSInput input) : SV_TARGET
{
	return input.color;
}*/
