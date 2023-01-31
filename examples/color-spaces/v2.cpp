// MAT201B 2022
// Assignment 1 starter code
// based on the `textureImage.cpp` AlloLib example by
// Karl Yerkes and Matt Wright (2011/10/10)
//
// added displacement map surface

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

#include "al/app/al_App.hpp"
#include "al/graphics/al_Image.hpp"

using namespace al;
using namespace std;

class MyApp : public App {
 public:
  Mesh actual, pic, rgb, hsv, displacement;
  Mesh next, previous;
  Mesh surface;

  bool draw_surface{false};

  void onCreate() override {
    // const char *filename = "rainbow.png";
    const char *filename = "hubble.jpg";
    auto image = Image(filename);
    if (image.array().size() == 0) {
      cout << "Failed to load image" << endl;
      exit(1);
    }

    pic.primitive(Mesh::POINTS);
    rgb.primitive(Mesh::POINTS);
    hsv.primitive(Mesh::POINTS);
    actual.primitive(Mesh::POINTS);
    displacement.primitive(Mesh::POINTS);
    surface.primitive(Mesh::TRIANGLES);

    auto get_color = [&](int column, int row) {
      auto p = image.at(column, row);
      return RGB(p.r / 255.0, p.g / 255.0, p.b / 255.0);
    };

    int W = image.width();
    int H = image.height();
    for (int row = 0; row < H; row++) {
      for (int column = 0; column < W; column++) {
        auto color = get_color(column, row);

        // placed in the position of the original image
        pic.color(color);
        pic.vertex((float)column / W - 0.5, (float)row / H - 0.5, 0.0);

        // placed in 3D in an "RGB cube"
        rgb.vertex(color.r - 0.5, color.g - 0.5, color.b - 0.5);
        rgb.color(color);

        // placed in an HSV cylinder
        auto c = HSV(color);
        hsv.vertex(c.s * sin(M_2PI * c.h),  //
                   c.v - 0.5,               //
                   c.s * cos(M_2PI * c.h));
        hsv.color(color);

        // xy of the original image; z based on pixel brightness or value
        displacement.vertex((float)column / W - 0.5,  //
                            (float)row / H - 0.5,     //
                            (color.r + color.g + color.b) / 5);
        displacement.color(color);
      }
    }

    /////////////////////////////
    // displacement map
    /////////

    // vertices
    for (int row = 0; row < H; row++) {
      for (int column = 0; column < W; column++) {
        auto color = get_color(column, row);
        surface.vertex((float)column / W - 0.5,  //
                       (float)row / H - 0.5,     //
                       (color.r + color.g + color.b) / 5);
        surface.color(color);
      }
    }

    // indices
    int index = 0;
    for (int row = 0; row < H - 1; row++) {
      for (int column = 0; column < W - 1; column++) {
        surface.index(index);
        surface.index(index + W + 1);
        surface.index(index + W);

        surface.index(index);
        surface.index(index + 1);
        surface.index(index + W + 1);

        index++;
      }
    }

    // initialize previous and next
    previous = pic;
    next = pic;
    actual = pic;

    // set the camera position back some (z=3) and center on (x=0.5, y=0.5)
    nav().pos(0, 0, 3);
  }

  double t{1.0};
  double angle = 0;
  void onAnimate(double dt) override {
    // XXX you put more code here!
    // hint modify `actual`
    // make an animation parameter (float) `t`
    if (t < 1.0) {
      for (int i = 0; i < actual.vertices().size(); i++) {
        Mesh::Vertex &a(previous.vertices()[i]);
        Mesh::Vertex &b(next.vertices()[i]);
        // Mesh::Vertex temporary = a;
        // temporary.lerp(b, (float)t);
        // actual.vertices()[i].set(temporary);
        actual.vertices()[i].set(a * (1 - t) + b * t);
      }
      t += dt;
    } else {
      t = 1.0;
    }
    angle += dt;
  }

  bool onKeyDown(const Keyboard &k) override {
    // Use a switch to do something when a particular key is pressed
    switch (k.key()) {
      // For printable keys, we just use its character symbol:
      case '1':
        // XXX you put more code here!
        previous = next;
        next = pic;
        t = 0;
        break;

      case '2':
        // case '?':
        //  XXX you put more code here!
        previous = next;
        next = rgb;
        t = 0;
        break;

      case '3':
        previous = next;
        next = hsv;
        t = 0;
        break;

      case '4':
        previous = next;
        next = displacement;
        t = 0;
        break;
        // XXX you put more code here!
        // hint reset animation parameter

      case '5':
        draw_surface = !draw_surface;
        break;

      default:
        break;
    }
    return true;
  }

  void onDraw(Graphics &g) override {
    g.clear(0.2f);
    g.meshColor();
    g.depthTesting(true);
    g.rotate(angle * 5, 0.0, 1.0, 0.0);
    g.draw(actual);
    if (draw_surface) {
      g.draw(surface);
    }
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
