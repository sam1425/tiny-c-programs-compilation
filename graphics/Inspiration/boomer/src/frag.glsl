#version 130
out mediump vec4 color;
in mediump vec2 texcoord;
uniform sampler2D tex;
uniform vec2 cursorPos;
uniform vec2 windowSize;
uniform float flShadow;
uniform float flRadius;
uniform float cameraScale;

void main()
{
    vec4 cursor = vec4(cursorPos.x, windowSize.y - cursorPos.y, 0.0, 1.0);
    float dist = length(cursor.xy - gl_FragCoord.xy);
    float radius = flRadius * cameraScale;
    float feather = max(10.0, radius * 0.25); // 25% of radius is feathered, min 10px
    float shadowFactor = smoothstep(radius - feather, radius, dist) * flShadow;
    color = mix(texture(tex, texcoord), vec4(0.0, 0.0, 0.0, 1.0), shadowFactor);
}

