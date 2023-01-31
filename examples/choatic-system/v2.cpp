// Karl Yerkes
// MAT201B
//
// Some interesting chaotic systems...
//
// * https://en.wikipedia.org/wiki/List_of_chaotic_maps
// * https://en.wikipedia.org/wiki/Logistic_map
// * https://en.wikipedia.org/wiki/Lorenz_system
// * https://en.wikipedia.org/wiki/Chua%27s_circuit
// * https://www.youtube.com/watch?v=m_Z-SIxqYcI
//   + See: "Dripping Faucet As a Model Chaotic System"
// * https://en.wikipedia.org/wiki/Duffing_equation
//
// Read _Chaos: Making a New Science_ (James Gleick)
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  static const int P = 9;
  static const int D = 2;
  Parameter p[P]{
      {"N", "p", 1000, 0, 10000},  // p[0] = N (simulation steps)
      {"h", "p", 0.01, 0, 0.018},  // p[1] = h (simulation time step)
      {"x0", "p", -0.72, -D, D},   // p[2] = x0 | initial
      {"y0", "p", -0.64, -D, D},   // p[3] = y0 | conditions
      {"z0", "p", 0, -D, D},       // p[4] = z0 |
      {"a", "p", 0.9, -3, 3},      // p[5] = a
      {"b", "p", -0.6, -3, 3},     // p[6] = b
      {"c", "p", 2.0, -3, 3},      // p[7] = c
      {"d", "p", 0.9, -3, 3},      // p[8] = d
  };
  // a = 0.9, b = -0.6013, c = 2.0, d = 0.50
  // a = 0.3, b = -0.6000, c = 2.0, d = 0.27

  Parameter width{"Width", 0.01, 0, 0.2};

  Mesh mesh;
  bool light{false};

  void recompute() {
    mesh.reset();
    mesh.primitive(Mesh::LINE_STRIP);

    mesh.vertex(p[2], p[3], p[4]);

    for (int i = 0; i < (int)p[0]; i++) {
      Vec3f _(mesh.vertices().back());
      float a(p[5]), b(p[6]), c(p[7]), d(p[8]), h(p[1]);
      mesh.vertex(_.x * _.x - _.y * _.y + a * _.x + b * _.y,  // x
                  2 * _.x * _.y + c * _.x + d * _.y,          // y
                  _.z + h                                     // z
      );
    }

    mesh.ribbonize(width, true);
    mesh.primitive(Mesh::TRIANGLE_STRIP);
    mesh.generateNormals();
  }

  void onCreate() override {
    recompute();
    nav().pos(0, 0, 10);
  }

  void onDraw(Graphics& g) override {
    g.clear(0);

    // draw mesh
    g.depthTesting(light);
    g.lighting(light);
    g.blendTrans();
    g.color(1);
    // g.scale(0.1);
    g.draw(mesh);

    // draw red ends
    g.pointSize(8);
    Mesh m{Mesh::POINTS};
    m.vertex(mesh.vertices()[0]);
    m.vertex(mesh.vertices().back());
    m.color(0, 1, 1);
    m.color(1, 0, 0);
    g.meshColor();
    g.draw(m);
  }

  bool onMouseDrag(Mouse const&) override {
    recompute();
    return true;
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    for (auto& e : p) {
      gui.add(e);
      parameterServer() << e;
    }
    gui.add(width);
  }

  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      light = !light;
    }
    return false;
  }
  void onSound(AudioIOData& io) override {}
  void onAnimate(double) override {}
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
