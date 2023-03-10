#version 400

in Fragment {
  vec4 color;
  vec2 mapping;
}
fragment;

layout(location = 0) out vec4 fragmentColor;

void main() {
  float r = dot(fragment.mapping, fragment.mapping);
  if (r > 1) discard;
  vec3 color = fragment.color.rgb;
  fragmentColor = vec4(color, 1 - r * r);
}