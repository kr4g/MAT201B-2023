#include "al/app/al_App.hpp"
using namespace al;
using namespace std;

int main() {
    Mesh a, b;
    a.vertex(1, 2);
    a.vertex(3, 4);
    cout << "a size: " << a.vertices().size() << endl;
    cout << "a vertex 0: " << a.vertices()[0] << endl;
    cout << "a vertex 1: " << a.vertices()[1] << endl;
    cout << "b size: " << b.vertices().size() << endl;

    cout << "Copying a into b" << endl;
    b = a;

    cout << "b size: " << b.vertices().size() << endl;
    cout << "b vertex 0: " << b.vertices()[0] << endl;
    cout << "b vertex 1: " << b.vertices()[1] << endl;
}
// output:
// a size: 2
// a vertex 0: {1, 2, 0}
// a vertex 1: {3, 4, 0}
// b size: 0
// Copying a into b
// b size: 2
// b vertex 0: {1, 2, 0}
// b vertex 1: {3, 4, 0}