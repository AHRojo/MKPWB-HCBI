#include <iostream>
#include "leche.h"

Leche::Leche(int cuota, char tipo) {
    this->cuota = cuota;
    this->tipo = tipo;
};

void Leche::setValor(float valor) {
    this->valor = valor;
}
