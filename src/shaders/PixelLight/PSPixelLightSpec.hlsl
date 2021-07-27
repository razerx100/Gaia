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

    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation
                           * max(0.0f, dot(directionOfLight, normal));

    // Specular highlight
    const float3 viewVector = normalize(cameraPosition - worldPos);

    const float3 reflectionVector =
        reflect(-normalize(directionOfLight), normalize(normal));

    const float4 specularSample = specularTex.Sample(samplerState, uv);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);

    const float3 specular = attenuation * (diffuseColor * diffuseIntensity) * pow(
        max(0.0f, dot(reflectionVector, viewVector)), specularPower
    );

    return float4(
        saturate((diffuse + ambient)
        * diffuseTex.Sample(samplerState, uv).rgb
        + specular * specularReflectionColor
        ),
        1.0f
    );
}
