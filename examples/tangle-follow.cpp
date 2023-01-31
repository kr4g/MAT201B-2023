// Karl Yerkes
// MAT201B
// 2022-01-04
// in-class example
// inspired by Graham Wakefields "Makeshift" (2010?)
//

#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"  // al::rnd::uniform(1, 2) , rnd::normal(..)

using namespace al;

const int N = 10;

Vec3f random_vector() {
  return Vec3f(rnd::uniformS(N), rnd::uniformS(N), rnd::uniformS(N));
}

void random_vector_change(Vec3f& v) {
  v.x += rnd::uniformS(0.01);
  v.y += rnd::uniformS(0.01);
  v.z += rnd::uniformS(0.01);
}

struct MyApp : App {
  Mesh mesh;

  void onCreate() override {
    // mesh.primitive(Mesh::LINES);
    mesh.primitive(Mesh::TRIANGLES);

    for (int i = 0; i < 1000; i++) {
      mesh.vertex(random_vector());
      mesh.color(rnd::uniform(), rnd::uniform(), rnd::uniform());
    }

    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    // random walk of vertices...
    // for (Vec3f& v : mesh.vertices()) {
    //   random_vector_change(v);
    // }

    // this is what we had at the end of class
    // mesh.vertices()[0].lerp(mesh.vertices()[mesh.vertices().size() - 1],
    // 0.01); for (int i = 1; i < mesh.vertices().size(); i++) {
    //   mesh.vertices()[i].lerp(mesh.vertices()[i - 1], 0.01);
    // }

    // this might be a little easier to read
    // lerp each vertex toward its neighbor
    // reminds me of ribbon dancing
    // inspired by Makeshift by Graham Wakefield
    auto& vertex =
        mesh.vertices();  // 'vertex' becomes an alias for 'mesh.vertices()'
    vertex[0].lerp(vertex.back(), 0.01);
    for (int i = 1; i < vertex.size(); i++) {
      vertex[i].lerp(vertex[i - 1], 0.01);
    }
    // 'vertex.back()' is the same as 'vertex[size - 1]'

    if (rnd::prob(0.01)) {
      // randomly swap vertices
      auto& a = vertex[rnd::uniform(vertex.size())];
      auto& b = vertex[rnd::uniform(vertex.size())];
      Vec3f t = a;
      a = b;
      b = t;
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    g.meshColor();
    g.draw(mesh);

    Mesh m;
    m.copy(mesh);
    m.ribbonize(0.01, true);
    m.generateNormals();
    m.primitive(Mesh::TRIANGLE_STRIP);
    g.color(0.8);
    g.draw(m);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
