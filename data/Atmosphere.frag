uniform vec4 center;
uniform vec4 color;

void main() {
    float distance = distance(gl_FragCoord.xy, center.xy);
    float mixValue = 1.0;

    if (distance > center.z)
        mixValue = 1.0 - ((distance - center.z) / (center.w));

    gl_FragColor = mix(gl_Color, color, mixValue);
}
