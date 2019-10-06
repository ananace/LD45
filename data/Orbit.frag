uniform vec4 center;
uniform float alpha;
uniform float scale;

void main() {
    vec2 directionVec = gl_FragCoord.xy - center.xy;
    float distance = length(directionVec);

    float direction = atan(directionVec.y, directionVec.x);

    if (distance < (center.z / scale) + center.w && distance > (center.z / scale) - center.w)
    {
        float mix = 1.0 - mod(direction * (180.0 / 3.14159), 2.0);
        if (mix >= 0.1)
          mix = 1.0;

        // gl_FragColor = mix(gl_Color, vec4(1.0, 1.0, 1.0, 1.0), 0.5);
        gl_FragColor = mix(gl_Color, vec4(1.0, 1.0, 1.0, 0.25), mix);

        return;
    }

    gl_FragColor = gl_Color;
}
