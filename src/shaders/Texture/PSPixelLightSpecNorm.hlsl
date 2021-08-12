cbuffer LightBuf : register(b0, space1) {
	float3 lightPosition;
    float3 viewPosition;
    float3 ambientLight;
    float3 diffuseLight;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

Texture2D diffuseTex : register(t0);
Texture2D specularTex : register(t1);
Texture2D normalTex : register(t2);
SamplerState samplerState : register(s0);

float4 main(float3 worldPosition : Position, float3 worldTangent : Tangent,
            float3 worldBiTangent : BiTangent, float3 worldNormal : Normal,
            float2 uv : TexCoord) : SV_TARGET {

    float3 lightDirection = lightPosition - worldPosition;
    float lightDistance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    float attenuation = 1.0f / (attConst + (attLin * lightDistance)
                        + (attQuad * pow(lightDistance, 2.0f)));
    float3 lightColor = diffuseLight * diffuseIntensity * attenuation;

    // Normal map
    float3x3 TBN = float3x3(
        normalize(worldTangent),
        normalize(worldBiTangent),
        normalize(worldNormal)
    );

    float3 normal = normalTex.Sample(samplerState, uv).xyz;
    normal = normal * 2.0f - 1.0f;

    normal = normalize(mul(normal, TBN));

    // DiffuseTexture
    float3 diffuseTexture = diffuseTex.Sample(samplerState, uv).xyz;

    // Diffuse
    float3 ambient = ambientLight * diffuseTexture;
    float3 diffuse = lightColor * max(dot(normal, lightDirection), 0.0f) * diffuseTexture;

    // Specular
    float3 viewDirection = normalize(viewPosition - worldPosition);
    float3 halfwayDirection = normalize(lightDirection + viewDirection);

    float4 specularSample = specularTex.Sample(samplerState, uv);
    float3 specularColor = specularSample.xyz;
    float specularPower = pow(2.0f, specularSample.w * 23.0f);

    float3 specular = lightColor * pow(
        max(dot(normal, halfwayDirection), 0.0f), specularPower
    ) * specularColor;

    return float4(diffuse + ambient + specular, 1.0f);
}
