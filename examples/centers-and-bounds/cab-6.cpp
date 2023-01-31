/*
  Boxes and centers?
  adapted from Lance Putnam, 12/8/2010 (putnam.lance at gmail dot com)
*/

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Functions.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

const int N = 100;

// defined later
void addBox(Mesh& mesh, const Vec3f& minimum, const Vec3f& maximum);

class MyApp : public App {
 public:
  // ParameterColor color{"Color"};
  Parameter radius{"Radius", 1, 0.001, 3};
  // ParameterInt index{"Index", 0, 0, N};
  ParameterInt index{"Index", "", 0, 0, N};

  Mesh mesh;  // vertices (positions), color, texcoords, normal

  std::vector<Vec3f> position;
  std::vector<bool> local;
  Vec3f center;            // center of the currently selected "flock"
  Vec3f minimum, maximum;  // establishes bounds

  void onCreate() override {
    position.resize(N);
    local.resize(N);
    // for (auto t : position) {
    for (auto& t : position) {
      t = rnd::ball<Vec3f>() * 3;
    }

    // add cube and create face normals
    addCube(mesh);
    mesh.decompress();
    mesh.generateNormals();

    // nav().pullBack(16); // don't use this unless you know what it does
    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    const Vec3f& point = position[index];

    int count = 0;
    Vec3f sum(0, 0, 0);
    minimum = Vec3f(3e8);
    maximum = Vec3f(-3e8);

    for (int i = 0; i < N; i++) {
      local[i] = false;

      if (i == index) continue;

      if ((point - position[i]).mag() < radius) {
        local[i] = true;     // mark this as "in the local flock"
        sum += position[i];  // find the center
        count++;

        // find the bounds of these points
        if (position[i].x > maximum.x) maximum.x = position[i].x;
        if (position[i].y > maximum.y) maximum.y = position[i].y;
        if (position[i].z > maximum.z) maximum.z = position[i].z;
        if (position[i].x < minimum.x) minimum.x = position[i].x;
        if (position[i].y < minimum.y) minimum.y = position[i].y;
        if (position[i].z < minimum.z) minimum.z = position[i].z;
      }
    }
    if (count > 0) {
      center = sum / count;  // what if count == 0?
    }

    //
    // Camera/Nav automation ~ We use the details we just computed (center and
    // bounds) to point the camera and move the camera to that we can look at
    // particular agents and their local groups. This code is not carefully
    // explained; It also is far from perfect.
    //

    nav().smooth(0.5);
    Vec3f middle = (minimum + maximum) / 2;
    nav().faceToward(middle, Vec3f(0, 1, 0), 0.07);
    nav().moveR(0.007);

    {
      Vec3f v(nav().pos());
      float distance = (v - middle).mag();
      float hueristic = al::clip((maximum - minimum).mag(), 3.0f, 0.3f);
      if (distance > hueristic * 2.7) {
        nav().nudgeToward(middle, 0.02);
      } else if (distance < hueristic * 1.2) {
        nav().nudgeToward(middle, -0.37);
      }
      v.y = middle.y;
      distance = (v - nav().pos()).mag();
      hueristic = 0.7;
      if (distance > hueristic) {
        nav().nudgeToward(v, 0.01);
      }
    }
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
        g.color(HSV(0.6666667, 1, 1));
      }
      g.polygonFill();
      g.draw(mesh);  // draw the mesh

      // g.scale(1.01);
      // g.color(0);  //
      // g.polygonLine();
      // g.draw(mesh); //draw the mesh

      g.popMatrix();
    }

    g.pushMatrix();
    Mesh sphere(Mesh::TRIANGLES);
    addSphere(sphere);
    sphere.decompress();
    sphere.generateNormals();
    g.translate(center);
    g.scale(0.1);
    g.color(HSV(0.5, 1, 1));
    g.draw(sphere);
    g.popMatrix();

    // g.depthTesting(true);
    g.lighting(false);

    Mesh box(Mesh::LINES);
    addBox(box, minimum, maximum);
    g.color(1);
    g.draw(box);
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(radius);
    gui.add(index);
  }
};

int main() { MyApp().start(); }

void addBox(Mesh& mesh, const Vec3f& minimum, const Vec3f& maximum) {
  mesh.vertex(minimum.x, minimum.y, minimum.z);
  mesh.vertex(maximum.x, minimum.y, minimum.z);
  mesh.vertex(minimum.x, minimum.y, maximum.z);
  mesh.vertex(maximum.x, minimum.y, maximum.z);
  mesh.vertex(minimum.x, maximum.y, maximum.z);
  mesh.vertex(maximum.x, maximum.y, maximum.z);
  mesh.vertex(minimum.x, maximum.y, minimum.z);
  mesh.vertex(maximum.x, maximum.y, minimum.z);

  mesh.vertex(minimum.x, minimum.y, minimum.z);
  mesh.vertex(minimum.x, maximum.y, minimum.z);
  mesh.vertex(minimum.x, minimum.y, maximum.z);
  mesh.vertex(minimum.x, maximum.y, maximum.z);
  mesh.vertex(maximum.x, minimum.y, maximum.z);
  mesh.vertex(maximum.x, maximum.y, maximum.z);
  mesh.vertex(maximum.x, minimum.y, minimum.z);
  mesh.vertex(maximum.x, maximum.y, minimum.z);

  mesh.vertex(minimum.x, minimum.y, minimum.z);
  mesh.vertex(minimum.x, minimum.y, maximum.z);
  mesh.vertex(minimum.x, maximum.y, minimum.z);
  mesh.vertex(minimum.x, maximum.y, maximum.z);
  mesh.vertex(maximum.x, maximum.y, minimum.z);
  mesh.vertex(maximum.x, maximum.y, maximum.z);
  mesh.vertex(maximum.x, minimum.y, minimum.z);
  mesh.vertex(maximum.x, minimum.y, maximum.z);
}