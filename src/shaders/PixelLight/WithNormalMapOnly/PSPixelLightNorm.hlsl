cbuffer ColorBuf : register(b1, space1) {
    float specularIntensity;
    float specularPower;
};

cbuffer TransformData : register(b1, space0) {
    matrix model;
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
Texture2D normalTex : register(t1);
SamplerState samplerState : register(s0);

float4 main(float3 worldPos : Position,
			float3 normal : Normal,
            float2 uv : TexCoord) : SV_TARGET {
    const float3 vectorToLight = lightPosition - worldPos;
    const float distanceToLight = length(vectorToLight);
    const float3 directionOfLight = vectorToLight / distanceToLight;

    const float attenuation = 1.0f /
        (attConst + attLin * distanceToLight + attQuad *
            (distanceToLight * distanceToLight));

    // Normal map
    float3 normalSample = normalTex.Sample(samplerState, uv).rgb;
    normal.x = normalSample.x * 2.0f - 1.0f;
    normal.y = -normalSample.y * 2.0f + 1.0f;
    normal.z = -normalSample.z;

    normal = mul(normal, (float3x3) model);

    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation
                           * max(0.0f, dot(directionOfLight, normal));

    // Specular highlight
    const float3 viewVector = normalize(cameraPosition - worldPos);
    const float3 reflectionVector =
        reflect(-normalize(directionOfLight), normalize(normal));

    const float3 specular = attenuation * (diffuseColor * diffuseIntensity)
        * specularIntensity * pow(
        max(0.0f, dot(reflectionVector, viewVector)), specularPower
    );

    return float4(
        saturate((diffuse + ambient) * tex.Sample(samplerState, uv).rgb + specular)
        , 1.0f
    );
}
