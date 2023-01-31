#include "al/app/al_App.hpp"
using namespace al;

int main() {
    Color color(1.0, 0.5, 0.1); // default is RGB
    HSV hsv(color); // this has converted RGB to HSV
    std::cout << "r: " << color.r << std::endl;
    std::cout << "g: " << color.g << std::endl;
    std::cout << "b: " << color.b << std::endl;
    std::cout << "h: " << hsv.h << std::endl;
    std::cout << "s: " << hsv.s << std::endl;
    std::cout << "v: " << hsv.v << std::endl;
}
// output:
// r: 1
// g: 0.5
// b: 0.1
// h: 0.0740741
// s: 0.9
// v: 1