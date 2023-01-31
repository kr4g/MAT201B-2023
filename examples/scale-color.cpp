#include "al/app/al_App.hpp"

using namespace al;
using namespace std;

int main() {
  RGB rgb(1.0, 0.5, 0.25);
  rgb *= 0.5;
  cout << "rgb * 0.5 -> " << rgb.r << ", " << rgb.g << ", " << rgb.b << endl;
}
// output:
// rgb * 0.5 -> 0.5, 0.25, 0.125