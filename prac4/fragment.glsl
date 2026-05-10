#version 330 core

in vec2 fragTexCoord;
out vec4 outColor;

uniform vec3 objectColor;
uniform bool showGrid;
uniform float gridDensity;

void main()
{
    vec3 finalColor = objectColor;

    if (showGrid)
    {
        vec2 gridUv = fract(fragTexCoord * gridDensity);
        float edgeDistance = min(min(gridUv.x, 1.0 - gridUv.x),
                                 min(gridUv.y, 1.0 - gridUv.y));
        float lineMask = 1.0 - smoothstep(0.0, 0.03, edgeDistance);
        finalColor = mix(finalColor, finalColor * 0.55, lineMask);
    }

    outColor = vec4(finalColor, 1.0);
}
