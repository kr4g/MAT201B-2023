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

// This is the Lorenz system

struct MyApp : App {
  static const int P = 8;
  static const int D = 3;
  Parameter p[P]{
      {"N", "p", 10000, 0, 100000},   // p[0] = N (simulation steps)
      {"h", "p", 0.01, 0, 0.018},     // p[1] = h (simulation time step)
      {"x0", "p", 0, -D, D},          // p[2] = x0 | initial
      {"y0", "p", 0.1, -D, D},        // p[3] = y0 | conditions
      {"z0", "p", 0, -D, D},          // p[4] = z0 |
      {"rho", "p", 28, 0, 56},        // p[5] = rho   | simulation
      {"sigma", "p", 10, 0, 20},      // p[6] = sigma | parameters
      {"beta", "p", 8.0f / 3, 0, 4},  // p[7] = beta  |
  };

  Parameter width{"Width", 0.07, 0, 0.2};

  Mesh lorenz;
  bool light{false};

  void recompute() {
    lorenz.reset();
    lorenz.primitive(Mesh::LINE_STRIP);
    lorenz.vertex(p[2], p[3], p[4]);
    for (int i = 0; i < (int)p[0]; i++) {
      Vec3f _(lorenz.vertices().back());
      // Lorenz
      float rho(p[5]), sigma(p[6]), beta(p[7]), h(p[1]);
      Vec3f f(sigma * (_.y - _.x),      //
              _.x * (rho - _.z) - _.y,  //
              _.x * _.y - beta * _.z);
      lorenz.vertex(_ + h * f);
      // the line above is Euler's method!
    }

    lorenz.ribbonize(width, true);
    lorenz.primitive(Mesh::TRIANGLE_STRIP);
    lorenz.generateNormals();
  }

  void onCreate() override {
    recompute();
    nav().pos(0, 0, 10);
  }

  void onDraw(Graphics& g) override {
    g.clear(0);

    // draw lorenz
    g.depthTesting(light);
    g.lighting(light);
    g.blendTrans();
    g.color(1);
    g.scale(0.1);
    g.draw(lorenz);

    // draw red ends
    g.pointSize(8);
    Mesh m{Mesh::POINTS};
    m.vertex(lorenz.vertices()[0]);
    m.vertex(lorenz.vertices().back());
    g.color(1, 0, 0);
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
