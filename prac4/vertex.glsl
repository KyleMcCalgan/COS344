#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 fragTexCoord;
out vec3 fragWorldPos;
out vec3 fragWorldNormal;

uniform mat4 MVP;
uniform mat4 Model;
uniform sampler2D displacementMap;
uniform bool applyTextureMaps;
uniform bool displacementTexEnabled;
uniform float maxDisplacement;

void main()
{
    vec3 localPosition = aPos;
    vec3 localNormal = normalize(aNormal);

    if (applyTextureMaps && displacementTexEnabled)
    {
        float displacementSample = texture(displacementMap, aTexCoord).r;
        float displacementAmount = (1.0 - displacementSample) * maxDisplacement;
        localPosition += localNormal * displacementAmount;
    }

    vec4 worldPos = Model * vec4(localPosition, 1.0);
    vec3 worldNormal = normalize((Model * vec4(localNormal, 0.0)).xyz);

    gl_Position = MVP * vec4(localPosition, 1.0);
    fragTexCoord = aTexCoord;
    fragWorldPos = worldPos.xyz;
    fragWorldNormal = worldNormal;
}
