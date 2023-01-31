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

  std::vector<Vec3f> position;

  void onCreate() {
    position.resize(100);
    // for (auto t : position) {
    for (auto& t : position) {
      t = rnd::ball<Vec3f>() * 3;
    }

    int Nv = addCube(mesh);
    for (int i = 0; i < Nv; ++i) {
      float f = float(i) / Nv;
      mesh.color(HSV(f * 0.1 + 0.2, 1, 1));
    }

    // Create face normals
    for (int i = 0; i < 5; ++i) {
      mesh.decompress();
      mesh.generateNormals();
    }

    // nav().pullBack(16);
    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) {}

  void onDraw(Graphics& g) {
    g.clear(0.3);

    g.depthTesting(true);
    g.lighting(true);

    for (auto t : position) {
      g.pushMatrix();

      g.translate(t);
      g.scale(0.1);

      g.meshColor();
      g.polygonFill();
      g.draw(mesh);  // draw the mesh

      // g.scale(1.01);
      // g.color(0);  //
      // g.polygonLine();
      // g.draw(mesh); //draw the mesh

      g.popMatrix();
    }
  }
};

int main() { MyApp().start(); }
