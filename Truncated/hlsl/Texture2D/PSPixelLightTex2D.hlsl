cbuffer ColorBuf : register(b0, space1) {
    float specularIntensity;
    float specularPower;
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

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float3 worldPos : Position,
			float3 normal : Normal,
            float2 uv : TexCoord) : SV_TARGET {
    const float3 vectorToLight = lightPosition - worldPos;
    const float distanceToLight = length(vectorToLight);
    const float3 directionOfLight = vectorToLight / distanceToLight;

    const float attenuation = 1.0f /
        (attConst + attLin * distanceToLight + attQuad * (distanceToLight * distanceToLight));

    const float3 diffuseData = attenuation * diffuseColor * diffuseIntensity;

    const float3 diffuse = diffuseData * max(0.0f, dot(directionOfLight, normal));

    // Specular highlight
    const float3 vn = normalize(vectorToLight);

    const float3 r = reflect(normalize(-lightPosition), normalize(normal));

    const float3 specular = diffuseData * specularIntensity * pow(
        max(0.0f, dot(r, vn)), specularPower
    );

    return saturate(float4((diffuse + ambient + specular), 1.0f) * tex.Sample(samplerState, uv));
}
