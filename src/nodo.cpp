#include <iostream>
#include "nodo.h"

Nodo::Nodo(int id, float x, float y, char tipo, int cantidad) {
    this->id = id;
    this->x = x;
    this->y = y;
    this->tipo = tipo;
    this->cantidad = cantidad;
};

void Nodo::toString() {
    std::cout << this->id << "-" << this->x << "-" << this->y << "-" << this->tipo << "-" << this->cantidad << "\n";
}
