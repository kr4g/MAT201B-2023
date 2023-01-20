#include "al/app/al_App.hpp"  // al::App

struct MyApp : al::App {
  void onDraw(al::Graphics& g) override {
    g.clear(0, 1, 0);  // sets the background color
    //
  }
};

int main() {
  MyApp app;
  app.start();  // blocks; there is a loop in there
  return 0;
}
