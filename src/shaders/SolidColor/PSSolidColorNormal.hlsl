cbuffer ColorBuf : register(b1, space1) {
	float4 color;
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

float4 main(float3 viewPos : Position,
			float3 viewNormal : Normal) : SV_TARGET {
    const float3 vectorToLight = lightPosition - viewNormal;
    const float distanceToLight = length(vectorToLight);
    const float3 directionOfLight = vectorToLight / distanceToLight;

    const float attenuation = 1.0f /
        (attConst + attLin * distanceToLight + attQuad * (distanceToLight * distanceToLight));

    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation
                            * max(0.0f, dot(directionOfLight, viewNormal));

    // Specular highlight
    const float3 viewVector = normalize(cameraPosition - viewPos);
    const float3 reflectionVector =
                    reflect(-normalize(lightPosition), normalize(viewNormal));

    const float3 specular = attenuation * (diffuseColor * diffuseIntensity)
        * specularIntensity * pow(
        max(0.0f, dot(reflectionVector, viewVector)), specularPower
    );

    return float4(
        saturate((diffuse + ambient) * color.rgb + specular)
        , 1.0f
    );
}
