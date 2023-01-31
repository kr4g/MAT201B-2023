#include "al/app/al_App.hpp"

using namespace al;
using namespace std;

void show(const Color& c) {
  cout << "color: " << c.r << ", " << c.g << ", " << c.b << endl;
}

int main() {
  RGB rgb(1.0, 0.5, 0.25);
  HSV hsv(0.5, 0.8, 1.0);
  float t = 0.7; // a parameter on the interval (0, 1)
  Color color = rgb * (1 - t) + hsv * t; // interpolation
  // when t == 0, the output is exactly the value of rgb
  // when t == 1, the output is exactly the value of hsv
  // then t == 0.5, we have the mean of the two
  show(rgb);
  show(hsv);
  show(color);
  
  // do science!
  rgb = RGB(1.0, 0.0, 0.0); // red
  hsv = HSV(0.66666, 1.0, 1.0); // what color?........................blue!
  t = 0.5;
  show(rgb * (1 - t) + hsv * t); // what color?.......................purple!
}
// output:
// color: 1, 0.5, 0.25
// color: 0.2, 1, 1
// color: 0.44, 0.85, 0.775
// color: 0.5, 2.00272e-05, 0.5

// 2.00272e-05 ????