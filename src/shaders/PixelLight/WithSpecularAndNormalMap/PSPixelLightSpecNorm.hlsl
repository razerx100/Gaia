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

Texture2D diffuseTex : register(t0);
Texture2D specularTex : register(t1);
Texture2D normalTex : register(t2);
SamplerState samplerState : register(s0);

float4 main(float3 viewPos : Position, float3 viewTangent : Tangent,
            float3 viewBiTangent : BiTangent, float3 viewNormal : Normal,
            float2 uv : TexCoord) : SV_TARGET {

    const float3 vectorToLight = lightPosition - viewPos;
    const float distanceToLight = length(vectorToLight);
    const float3 directionOfLight = vectorToLight / distanceToLight;

    const float attenuation = 1.0f /
        (attConst + attLin * distanceToLight + attQuad *
            (distanceToLight * distanceToLight));

    // Normal map
    /* const float3x3 tanView = float3x3(
        normalize(viewTangent),
        normalize(viewBiTangent),
        normalize(viewNormal)
    );

    const float3 normalSample = normalTex.Sample(samplerState, uv).xyz;
    viewNormal = normalSample * 2.0f - 1.0f;

    viewNormal = mul(viewNormal, tanView);*/

    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation
                            * max(0.0f, dot(directionOfLight, viewNormal));

    // Specular highlight
    const float3 viewVector = normalize(cameraPosition - viewPos);

    const float3 reflectionVector =
        reflect(-normalize(directionOfLight), normalize(viewNormal));

    const float4 specularSample = specularTex.Sample(samplerState, uv);
    const float3 specularReflectionColor = specularSample.xyz;
    const float specularPower = pow(2.0f, specularSample.w * 13.0f);

    const float3 specular = attenuation * (diffuseColor * diffuseIntensity) * pow(
        max(0.0f, dot(reflectionVector, viewVector)), specularPower
    );

    return float4(
        saturate((diffuse + ambient)
        * diffuseTex.Sample(samplerState, uv).xyz
        + specular * specularReflectionColor
        ),
        1.0f
    );
}
