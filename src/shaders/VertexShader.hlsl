struct VSOut {
	float4 color : Color;
	float4 position : SV_POSITION;
};

VSOut main(float2 data : Position, float4 color : Color) {
	VSOut vout;
	vout.position = float4(data.x, data.y, 0.0f, 1.0f);
	vout.color = color;
	return vout;
}