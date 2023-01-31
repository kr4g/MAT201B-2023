#include "al/app/al_App.hpp"

using namespace al;
using namespace std;

int main() {
  Mesh a, b, c;
  for (int i = 0; i < 10; i++) {
    a.vertex(i, i, i);              // some increasing data
    b.vertex(9 - i, 9 - i, 9 - i);  // decreasing
    c.vertex(0, 0, 0);              // zero
  }

  // sum the vertices of mesh a and b and save in c
  for (int i = 0; i < c.vertices().size(); i++) {
    c.vertices()[i] = a.vertices()[i] + b.vertices()[i];
  }

  // print out all the vertices in mesh c
  for (int i = 0; i < c.vertices().size(); i++) {
    cout << c.vertices()[i] << endl;
  }
}
// output:
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
// {9, 9, 9}
