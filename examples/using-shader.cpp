#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

// this is the GLSL program to run on each VERTEX it decides on the screen
// position (2D), given the world position (3D)
const char* vertex_code = R"(
#version 400
layout(location = 0) in vec3 vertexPosition; // DONT CHANGE
layout(location = 1) in vec4 vertexColor;    // DONT CHANGE
layout(location = 2) in vec2 vertexTexture;  // DONT CHANGE
uniform mat4 al_ModelViewMatrix;             // DONT CHANGE
uniform mat4 al_ProjectionMatrix;            // DONT CHANGE
void main() {
  gl_Position = al_ProjectionMatrix * al_ModelViewMatrix * vec4(vertexPosition, 1.0);
}
)";

// this is the GLSL program to run on each FRAGMENT (drawable pixel) it decides
// on the color of each pixel, given various data (time, screen position, input
// color, etc.)
const char* fragment_code = R"(
#version 400
layout(location = 0) out vec4 fragmentColor;
uniform vec3 color;
uniform float time;
void main() {
  // https://people.freedesktop.org/~idr/OpenGL_tutorials/03-fragment-intro.html
  float distance = 350 + sin(time * 3) * 500;
  vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
  float dist_squared = dot(pos, pos);
  fragmentColor = (dist_squared < distance) ? vec4(color, 1.0) : vec4(1 - color.r, color.g * color.g, 1 - color.b * color.b, 1.0);
}
)";

struct AlloApp : App {
  ParameterColor color{"/color"};
  ShaderProgram shader;
  Mesh mesh;

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(color);
  }

  void onCreate() override {
    mesh.primitive(Mesh::TRIANGLE_STRIP);
    mesh.vertex(-1, 1);
    mesh.vertex(-1, -1);
    mesh.vertex(1, 1);
    mesh.vertex(1, -1);

    shader.compile(vertex_code, fragment_code);
    nav().pos(0, 0, 5);
  }

  double time = 0;
  void onAnimate(double dt) override {
    //
    time += dt;
  }

  void onDraw(Graphics& g) override {
    g.clear(color);
    g.shader(shader);
    Color c(color);
    g.shader().uniform("time", (float)time);
    g.draw(mesh);
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}