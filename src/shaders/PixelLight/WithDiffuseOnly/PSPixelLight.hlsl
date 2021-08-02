cbuffer ColorBuf : register(b1, space1) {
    float specularIntensity;
    float specularPower;
};

cbuffer LightBuf : register(b0, space1) {
	float3 lightPosition;
    float3 cameraPosition;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float3 viewPos : Position,
			float3 viewNormal : Normal,
            float2 uv : TexCoord) : SV_TARGET {
    const float3 vectorToLight = lightPosition - viewPos;
    const float distanceToLight = length(vectorToLight);
    const float3 directionOfLight = vectorToLight / distanceToLight;

    const float attenuation = 1.0f /
        (attConst + attLin * distanceToLight + attQuad *
            (distanceToLight * distanceToLight));

    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation
                            * max(0.0f, dot(directionOfLight, viewNormal));

    // Specular highlight
    const float3 viewVector = normalize(cameraPosition - viewPos);
    const float3 reflectionVector =
        reflect(-normalize(directionOfLight), normalize(viewNormal));

    const float3 specular = attenuation * (diffuseColor * diffuseIntensity)
        * specularIntensity * pow(
        max(0.0f, dot(reflectionVector, viewVector)), specularPower
    );

    return float4(
        saturate((diffuse + ambient) * tex.Sample(samplerState, uv).xyz + specular)
        , 1.0f
    );
}
