#ifndef UTILS_H
#define UTILS_H

template <typename T>
void mySwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

struct Coord {
    int r;
    int c;
};

#endif
