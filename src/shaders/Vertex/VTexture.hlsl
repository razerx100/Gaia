cbuffer cbuff {
	matrix transform;
};

struct VSOut {
	float2 coordinate : TexCoord;
	float4 position : SV_Position;
};

VSOut main(float3 position : Position, float2 coord : TexCoord) {
	VSOut outp;

	outp.position = mul(float4(position, 1.0f), transform);
	outp.coordinate = coord;

	return outp;
}
