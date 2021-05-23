cbuffer ColorBuf : register(b0, space1) {
	float4 faceColors[6];
};

cbuffer LightBuf : register(b1, space1) {
	float3 lightPosition;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

float4 main(float3 worldPos : Position,
			float3 normal : Normal,
			uint id : SV_PrimitiveID) : SV_TARGET {
    const float3 vectorToLight = lightPosition - worldPos;
    const float distanceToLight = length(vectorToLight);
    const float3 directionOfLight = vectorToLight / distanceToLight;

    const float attenuation = 1.0f /
        (attConst + attLin * distanceToLight + attQuad * (distanceToLight * distanceToLight));

    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation *
        max(0.0f, dot(directionOfLight, normal));

    return saturate(float4((diffuse + ambient), 1.0f) * faceColors[(id / 2) % 6]);
}
