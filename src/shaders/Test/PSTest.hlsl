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

cbuffer ColorBuf : register(b1, space1) {
	float4 color;
    float specularIntensity;
    float specularPower;
};

float4 main(float3 worldPosition : Position, float3 worldNormal : Normal)
    : SV_TARGET {

    float3 normal = normalize(worldNormal);
    float3 lightDirection = lightPosition - worldPosition;
    float lightDistance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    float attenuation = 1.0f / (attConst + (attLin * lightDistance)
                        + (attQuad * pow(lightDistance, 2.0f)));
    float3 lightColor = diffuseLight * diffuseIntensity * attenuation;

    // Diffuse
    float3 ambient = ambientLight * color.xyz;
    float3 diffuse = lightColor * max(dot(normal, lightDirection), 0.0f) * color.xyz;

    // Specular
    float3 viewDirection = normalize(viewPosition - worldPosition);
    float3 halfwayDirection = normalize(lightDirection + viewDirection);

    float3 specular = specularIntensity * lightColor * pow(
        max(dot(normal, halfwayDirection), 0.0f), specularPower
    );

    return float4(diffuse + ambient + specular, 1.0f);
}
