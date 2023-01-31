#include "al/app/al_App.hpp"

using namespace al;
using namespace std;

int main() {
  Mesh::Vertex v(1,2,3);
  v = v * 2;
  cout << "v * 2 -> " << v << endl;

  Vec3f w(1,2,3);
  w *= 2;
  cout << "w *= 2 -> " << w << endl;

  cout << "w + v -> " << w + v << endl;

  cout << "w / 2 + v * 0.5 -> "  << w / 2 + v * 0.5 << endl;
}
// output:
// v * 2 -> {2, 4, 6}
// w *= 2 -> {2, 4, 6}
// w + v -> {4, 8, 12}
// w / 2 + v * 0.5 -> {2, 4, 6}
