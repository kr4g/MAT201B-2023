/*
  Boxes and centers?
  adapted from Lance Putnam, 12/8/2010 (putnam.lance at gmail dot com)
*/

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

const int N = 100;

class MyApp : public App {
 public:
  // ParameterColor color{"Color"};
  Parameter radius{"Radius", 0.001, 0.001, 3};
  // ParameterInt index{"Index", 0, 0, N};
  ParameterInt index{"Index", "", 0, 0, N};

  Mesh mesh;  // vertices (positions), color, texcoords, normal

  std::vector<Vec3f> position;
  std::vector<bool> local;
  Vec3f center;

  void onCreate() override {
    position.resize(N);
    local.resize(N);
    // for (auto t : position) {
    for (auto& t : position) {
      t = rnd::ball<Vec3f>() * 3;
    }

    int Nv = addCube(mesh);
    for (int i = 0; i < Nv; ++i) {
      mesh.color(Color(0.7));
    }

    // Create face normals
    for (int i = 0; i < 5; ++i) {
      mesh.decompress();
      mesh.generateNormals();
    }

    // nav().pullBack(16);
    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    const Vec3f& point = position[index];

    int count = 0;
    Vec3f sum(0, 0, 0);
    for (int i = 0; i < N; i++) {
      local[i] = false;
      if ((point - position[i]).mag() < radius) {
        local[i] = true;     // mark this as "in the local flock"
        sum += position[i];  // find the center
        count++;
        // find the bounds of these points
      }
    }
    center = sum / count;  // what if count == 0?
  }

  void onDraw(Graphics& g) override {
    g.clear(0.3);

    g.depthTesting(true);
    g.lighting(true);

    for (int i = 0; i < N; i++) {
      g.pushMatrix();

      g.translate(position[i]);
      g.scale(0.1);

      if (i == index) {
        g.color(HSV(0.166666, 1, 1));
      } else if (local[i]) {
        g.color(HSV(0, 1, 1));
      } else {
        g.meshColor();
      }
      g.polygonFill();
      g.draw(mesh);  // draw the mesh

      // g.scale(1.01);
      // g.color(0);  //
      // g.polygonLine();
      // g.draw(mesh); //draw the mesh

      g.popMatrix();
    }

    Mesh c(Mesh::TRIANGLES);
    addSphere(c);
    c.decompress();
    c.generateNormals();
    g.translate(center);
    g.scale(0.1);
    g.draw(c);
  }

  //

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(radius);  // add parameter to GUI
    gui.add(index);   // add parameter to GUI
  }
};

int main() { MyApp().start(); }
