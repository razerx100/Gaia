cbuffer CVertexBuf : register(b0, space0) {
	matrix viewWithProjection;
};

float4 main(float3 data : Position) : SV_Position {
	return mul(float4(data, 1.0f), viewWithProjection);
}
