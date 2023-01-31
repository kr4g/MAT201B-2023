// Karl Yerkes
// MAT201B
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Functions.hpp"

using namespace al;

struct State {
  float a, b, y, z;
  float n[3];
};

// https://en.wikipedia.org/wiki/Superformula
float super(const float psi, const State& state) {
  return pow(pow(abs(cos(state.y * psi / 4) / state.a), state.n[1]) +
                 pow(abs(sin(state.z * psi / 4) / state.b), state.n[2]),
             -1 / state.n[0]);
}

// https://en.wikipedia.org/wiki/Superformula#Extension_to_higher_dimensions
Vec3f spherical_product(const float latitude, const float longitude,
                        const float r1, const float r2) {
  return Vec3f(r1 * cos(longitude) * r2 * cos(latitude),
               r1 * sin(longitude) * r2 * cos(latitude),  //
               r2 * sin(latitude));
}

const int P = 7;
struct MyApp : App {
  Parameter r1[P]{
      {"a", "r1", 1, 0, 2},    //
      {"b", "r1", 1, 0, 2},    //
      {"y", "r1", 1, 0, 20},   //
      {"z", "r1", 1, 0, 20},   //
      {"n1", "r1", 1, 0, 30},  //
      {"n2", "r1", 1, 0, 20},  //
      {"n3", "r1", 1, 0, 20},
  };
  Parameter r2[P]{
      {"a", "r2", 1, 0, 2},    //
      {"b", "r2", 1, 0, 2},    //
      {"y", "r2", 1, 0, 20},   //
      {"z", "r2", 1, 0, 20},   //
      {"n1", "r2", 1, 0, 30},  //
      {"n2", "r2", 1, 0, 20},  //
      {"n3", "r2", 1, 0, 20},
  };
  ParameterInt primitive{"Primitive", "", 2, 0, 10};
  ParameterInt N{"N", "", 100, 0, 200};
  ParameterInt M{"M", "", 100, 0, 200};
  Parameter spin[2]{
      {"Spin1", 0.01, -0.5, 0.5},  //
      {"Spin2", 0.01, -0.5, 0.5},  //
  };
  ParameterColor color{"Shape", "", Color(0)};
  ParameterColor clear{"Background"};

  bool mesh_color{false};
  bool light{false};
  float a{0};
  float b{0};

  Mesh mesh;

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(N);
    gui.add(M);
    gui.add(primitive);
    gui.add(spin[0]).add(spin[1]);
    for (int i = 0; i < P; i++) {
      gui.add(r1[i]);
    }
    for (int i = 0; i < P; i++) {
      gui.add(r2[i]);
    }
    gui.add(color);
    gui.add(clear);
  }

  void rebuild() {
    mesh.reset();

    mesh.primitive((Mesh::Primitive)primitive.get());
    if (mesh.primitive() == Mesh::TRIANGLE_FAN) {
      mesh.vertex(0, 0, 0);
    }

    float h = 0;
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        float latitude = M_PI * i / N - M_PI / 2;
        float longitude = 2 * M_PI * j / M - M_PI;
        mesh.vertex(spherical_product(
            latitude, longitude,
            super(latitude,
                  {r1[0], r1[1], r1[2], r1[3], {r1[4], r1[5], r1[6]}}),
            super(longitude,
                  {r2[0], r2[1], r2[2], r2[3], {r2[4], r2[5], r2[6]}})));
        mesh.color(HSV(al::wrap(h)));
        h += 0.01;
      }
    }

    // https://en.wikipedia.org/wiki/Triangle_strip
    if (primitive.get() == Mesh::TRIANGLES) {
      int index = M;
      for (int i = 1; i < N; i++) {
        for (int j = 0; j < M; j++) {
          mesh.index(index);
          mesh.index(index - M);
          mesh.index(index + 1);
          mesh.index(index + 1);
          mesh.index(index - M);
          mesh.index(index - M + 1);
          index++;
          if (index >= M * N) {
            goto DONE;
          }
        }
      }
    DONE:;
    }

    if (primitive.get() == Mesh::TRIANGLES ||
        primitive.get() == Mesh::TRIANGLE_STRIP ||
        primitive.get() == Mesh::TRIANGLE_FAN) {
      mesh.generateNormals();
    }
  }

  void onCreate() override { rebuild(); }
  void onAnimate(double dt) override {
    a += spin[0];
    b += spin[1];
    rebuild();
  }

  void onDraw(Graphics& g) override {
    g.clear(clear);
    // g.cullFaceBack();
    if (mesh_color) {
      g.meshColor();
    } else {
      g.color(color);
    }
    g.rotate(a, Vec3f(1, 1, 1));
    g.rotate(b, Vec3f(0, 1, 0));
    g.depthTesting(light);
    g.lighting(light);
    g.draw(mesh);
  }

  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == 'l') {
      light = !light;
    }
    if (k.key() == 'k') {
      mesh_color = !mesh_color;
    }
    return false;
  }

  void onSound(AudioIOData& io) override {}
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
