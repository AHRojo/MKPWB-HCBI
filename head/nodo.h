#ifndef NODO_H
#define NODO_H

class Nodo {
    public:
        int id;
        float x;
        float y;
        char tipo;
        int cantidad;

        Nodo(int id, float x, float y, char tipo, int cantidad);
        void toString();
};

#endif
