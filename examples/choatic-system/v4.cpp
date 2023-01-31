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
#include "al/math/al_Random.hpp"

using namespace al;
using namespace std;

string slurp(string fileName) {
  fstream file(fileName);
  string returnValue = "";
  while (file.good()) {
    string line;
    getline(file, line);
    returnValue += line + "\n";
  }
  return returnValue;
}

struct MyApp : App {
  static const int P = 8;
  static const int D = 3;
  Parameter p[P]{
      {"N", "p", 10000, 0, 100000},  // p[0] = N (simulation steps)
      {"h", "p", 0.01, 0, 0.018},    // p[1] = h (simulation time step)
      {"x0", "p", 0, -D, D},         // p[2] = x0 | initial
      {"y0", "p", 0, -D, D},         // p[3] = y0 | conditions
      {"z0", "p", 0, -D, D},         // p[4] = z0 |
      {"a", "p", 0, -D, D},          // p[5] = a  | simulation
      {"b", "p", 0, -D, D},          // p[6] = b  | parameters
      {"c", "p", 0, -D, D},          // p[7] = c  |
  };

  Parameter width{"Width", 0.07, 0, 0.2};
  ShaderProgram point_shader;

  Mesh mesh;
  Mesh point;
  bool light{false};

  double a{0};

  void recompute() {
    mesh.reset();
    mesh.primitive(Mesh::LINE_STRIP);
    mesh.vertex(p[2], p[3], p[4]);
    for (int i = 0; i < (int)p[0]; i++) {
      Vec3f _(mesh.vertices().back());
      // This is the T system, not Lorenz
      // https://www.sciencedirect.com/science/article/abs/pii/S0960077906007958
      // _Analysis of a 3D chaotic system_
      float a(p[5]), b(p[6]), c(p[7]), h(p[1]);
      Vec3f f(a * (_.y - _.x),                //
              (c - a) * _.x - a * _.x * _.z,  //
              -b * _.z + _.x * _.y);
      mesh.vertex(_ + h * f);
      // the line above is Euler's method!
    }

    mesh.ribbonize(width, true);
    mesh.primitive(Mesh::TRIANGLE_STRIP);
    mesh.generateNormals();
  }

  void onCreate() override {
    point_shader.compile(slurp("../v4/point-vertex.glsl"),
                         slurp("../v4/point-fragment.glsl"),
                         slurp("../v4/point-geometry.glsl"));

    recompute();
    point.primitive(Mesh::POINTS);
    for (int i = 0; i < 1000; i++) {
      point.vertex(rnd::uniformS(2), rnd::uniformS(2), rnd::uniformS(2));
      point.color(HSV(rnd::uniform()));
      point.texCoord(1.0, 0.0);
    }
    nav().pos(0, 0, 10);
  }

  void onDraw(Graphics& g) override {
    g.clear(0);

    g.rotate(a, Vec3f(0, 1, 0));

    // draw mesh
    g.depthTesting(light);
    g.lighting(light);
    g.blendTrans();
    g.color(1);  // change the shader that is in use
    g.scale(0.1);
    g.draw(mesh);

    g.shader(point_shader);
    g.shader().uniform("pointSize", 0.1 / 100);
    g.blending(true);
    g.blendTrans();
    g.depthTesting(true);
    g.draw(point);

    // draw red ends
    g.meshColor();
    g.pointSize(8);
    Mesh m{Mesh::POINTS};
    m.vertex(mesh.vertices()[0]);
    g.color(0, 0, 1);
    m.vertex(mesh.vertices().back());
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

  double t{0};
  void onAnimate(double dt) override {
    a += 0.1;
    t += dt;
    // if (t > 10) {
    if (false) {
      t -= 10;

      for (int i = 0; i < point.vertices().size(); i++) {
        point.vertices()[i] =
            Vec3f(rnd::uniformS(2), rnd::uniformS(2), rnd::uniformS(2));
      }
    }

    for (int i = 0; i < point.vertices().size(); i++) {
      float a(p[5]), b(p[6]), c(p[7]), h(p[1]);
      Vec3f& _(point.vertices()[i]);
      Vec3f f(a * (_.y - _.x),                //
              (c - a) * _.x - a * _.x * _.z,  //
              -b * _.z + _.x * _.y);
      _ += h * f;
    }
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
