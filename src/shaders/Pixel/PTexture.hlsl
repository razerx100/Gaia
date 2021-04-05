Texture2D tex;

SamplerState samplestate;

float4 main(float2 coordinate : TexCoord) : SV_Target {
	return tex.Sample(samplestate, coordinate);
}
