#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform vec3 uViewDir;
uniform float uFogDensity;
uniform float uFogStart;
uniform float uFogEnd;
uniform vec4 uFogColor;
uniform vec3 uLightDir;
uniform float uAmbient;

out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);
    vec4 albedo = texel * colDiffuse * fragColor;

    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(uLightDir);
    float diffuse = max(dot(normal, lightDirection), 0.0);
    float lighting = clamp(uAmbient + diffuse * (1.0 - uAmbient), 0.0, 1.0);
    vec3 litColor = albedo.rgb * lighting;

    // Depth is measured along the camera forward vector, not in absolute Z.
    // This keeps the screen-space result stable while the runner advances.
    vec3 fromCamera = fragPosition - viewPos;
    float depth = max(dot(fromCamera, normalize(uViewDir)), 0.0);
    float horizonFog = smoothstep(uFogStart, uFogEnd, depth);
    float fogFactor = clamp(horizonFog * 0.18, 0.0, 0.18);

    vec3 finalRgb = mix(litColor, uFogColor.rgb, fogFactor);
    finalColor = vec4(finalRgb, albedo.a);
}
