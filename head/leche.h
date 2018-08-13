#ifndef LECHE_H
#define LECHE_H

class Leche {
    public:
        int cuota;
        float valor;
        char tipo;

        Leche(int cuota, char tipo);
        void setValor(float valor);
};

#endif
