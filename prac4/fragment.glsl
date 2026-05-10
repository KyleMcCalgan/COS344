#version 330 core

in vec2 fragTexCoord;
in vec3 fragWorldPos;
in vec3 fragWorldNormal;
out vec4 outColor;

uniform vec3 objectColor;
uniform float objectAlpha;
uniform bool showGrid;
uniform float gridDensity;
uniform bool applyLighting;
uniform bool applyTextureMaps;
uniform bool colourTexEnabled;
uniform bool alphaTexEnabled;
uniform vec3 lightPos;
uniform vec3 effectiveLightColor;
uniform vec3 cameraPos;
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform sampler2D colourMap;
uniform sampler2D alphaMap;

void main()
{
    vec3 finalColor = objectColor;
    float finalAlpha = objectAlpha;

    if (applyLighting)
    {
        vec3 normal = normalize(fragWorldNormal);
        vec3 lightDir = normalize(lightPos - fragWorldPos);
        vec3 viewDir = normalize(cameraPos - fragWorldPos);
        vec3 reflectDir = reflect(-lightDir, normal);

        float distanceToLight = length(lightPos - fragWorldPos);
        float attenuation = 1.0 / (lightConstant +
                                   lightLinear * distanceToLight +
                                   lightQuadratic * distanceToLight * distanceToLight);

        vec3 ambient = ambientStrength * objectColor;
        float diffuseFactor = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = objectColor * diffuseFactor * effectiveLightColor;

        float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * specFactor * effectiveLightColor;

        finalColor = ambient + (diffuse + specular) * attenuation;
    }

    if (applyTextureMaps && colourTexEnabled)
    {
        finalColor *= texture(colourMap, fragTexCoord).rgb;
    }

    if (showGrid)
    {
        vec2 gridUv = fract(fragTexCoord * gridDensity);
        float edgeDistance = min(min(gridUv.x, 1.0 - gridUv.x),
                                 min(gridUv.y, 1.0 - gridUv.y));
        float lineMask = 1.0 - smoothstep(0.0, 0.03, edgeDistance);
        finalColor = mix(finalColor, finalColor * 0.55, lineMask);
    }

    if (applyTextureMaps && alphaTexEnabled)
    {
        float alphaSample = texture(alphaMap, fragTexCoord).r;
        finalAlpha = mix(objectAlpha, 1.0, alphaSample);
    }

    outColor = vec4(finalColor, finalAlpha);
}
