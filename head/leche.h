#ifndef LECHE_H
#define LECHE_H

class Leche {
    public:
        int cuota;
        float valor;
        char tipo;
        int recolectado = 0;

        Leche(int cuota, char tipo);
        void setValor(float valor);
        void recolectar(int i);
};

#endif
