// MAT201B 2022
// Assignment 1 "solution"
// based on the `textureImage.cpp` AlloLib example by
// Karl Yerkes and Matt Wright (2011/10/10)
//
//

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

  void onCreate() override {
    // const char *filename = "rainbow.png";
    const char *filename = "hubble.jpg";
    auto image = Image(filename);
    if (image.array().size() == 0) {
      cout << "failed to load image" << endl;
      exit(1);
    }
    cout << "loaded image size: " << image.width() << ", " << image.height()
         << endl;

    int W = image.width();
    int H = image.height();

    pic.primitive(Mesh::POINTS);
    rgb.primitive(Mesh::POINTS);
    hsv.primitive(Mesh::POINTS);
    actual.primitive(Mesh::POINTS);
    displacement.primitive(Mesh::POINTS);

    // iterate through all the pixel, scanning each row
    for (int row = 0; row < H; row++) {
      for (int column = 0; column < W; column++) {
        auto pixel = image.at(column, H - row - 1);
        pic.vertex((float)column / W - 0.5, (float)row / H - 0.5, 0.0);
        pic.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);

        float f = (pixel.r / 255.0 + pixel.g / 255.0 + pixel.b / 255.0) / 3;
        f *= 0.8;
        displacement.vertex((float)column / W - 0.5, (float)row / H - 0.5, f);
        displacement.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);

        RGB color_rgb(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
        rgb.vertex(color_rgb.r - 0.5, color_rgb.g - 0.5, color_rgb.b - 0.5);
        rgb.color(color_rgb);

        HSV color_hsv(color_rgb);
        hsv.vertex(color_hsv.s * sin(M_2PI * color_hsv.h), color_hsv.v - 0.5,
                   color_hsv.s * cos(M_2PI * color_hsv.h));
        hsv.color(color_rgb);
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
        previous = next;
        next = pic;
        t = 0;
        break;

      case '2':
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
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
