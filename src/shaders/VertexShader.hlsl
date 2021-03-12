float4 main(float2 data : POSITION) : SV_POSITION {
	return float4(data.x, data.y, 0.0f, 1.0f);
}