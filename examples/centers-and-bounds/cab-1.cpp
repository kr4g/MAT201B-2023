/*
  Boxes and centers?
  adapted from Lance Putnam, 12/8/2010 (putnam.lance at gmail dot com)
*/

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

class MyApp : public App {
 public:
  Mesh mesh;  // vertices (positions), color, texcoords, normal
  double angle1, angle2;

  void onCreate() {
    angle1 = angle2 = 0;
    int Nv;

    Nv = addCube(mesh);
    for (int i = 0; i < Nv; ++i) {
      float f = float(i) / Nv;
      mesh.color(HSV(f * 0.1 + 0.2, 1, 1));
    }

    // Create face normals
    for (int i = 0; i < 5; ++i) {
      mesh.decompress();
      mesh.generateNormals();
    }

    nav().pullBack(16);
  }

  void onAnimate(double dt) {
    angle1 += 1. / 3;
    angle2 += M_PI / 3;
  }

  void onDraw(Graphics& g) {
    g.clear(0.3);

    g.depthTesting(true);
    g.lighting(true);

    g.pushMatrix();
    g.rotate(angle1, 0, 1, 0);
    g.rotate(angle2, 1, 0, 0);

    // g.color(1);
    // g.meshColor();
    g.translate(1, 1, 1);
    // g.color(0.5, 0.5, 0.5);
    g.polygonFill();
    g.draw(mesh);

    g.scale(1.01);
    g.color(0);
    g.polygonLine();
    g.draw(mesh);

    g.popMatrix();
  }
};

int main() { MyApp().start(); }
