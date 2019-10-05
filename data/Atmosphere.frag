uniform vec4 center;
uniform vec4 color;
uniform float alpha;

void main() {
    vec2 distanceVec = gl_FragCoord.xy - center.xy;
    float distance = length(distanceVec);
    float direction = atan(distanceVec.y, distanceVec.x);

    distance += sin(direction * 8.0) * 8.0 * ((sin(direction + alpha) + 1.0) * 0.5) * 0.25;

    float mixValue = 1.0 - exp(-(distance - center.z) / center.w);

    gl_FragColor = mix(color, gl_Color, mixValue);
}
