#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>

#include "nodo.h"
#include "camion.h"
#include "leche.h"

std::vector<Camion> camiones;
std::vector<Nodo> planta;
std::vector<Nodo> nodos;
std::vector<Leche> leches;
std::vector<std::vector<Nodo>> mejorSolucion;
std::vector<std::vector<Nodo>> solucionInicial;
int totalCamiones;

void readFile(std::string arch) {
    std::ifstream file(arch);
    std::string word;
    std::string::size_type sz;
    int i = 0;
    int n;
    int j;

    file >> word;
    n = std::stoi(word);
    totalCamiones = n;
    while ( i < n && file >> word ) {
        Camion camioncito = Camion(std::stoi(word));
        camiones.push_back(camioncito);
        i++;
    }
    i = 0;
    file >> word;
    n = std::stoi(word);
    while ( i < n && file >> word ) {
        Leche milk = Leche(std::stoi(word), 'A' + i);
        leches.push_back(milk);
        i++;
    }
    i = 0;
    while ( i < n && file >> word ) {
        leches[i].setValor(std::stof(word));
        i++;
    }
    i = 0;
    file >> word;
    n = std::stoi(word);
    while (i < n && file >> word ) {
        std::vector<std::string> datos;
        datos.clear();
        datos.push_back(word);
        for ( j = 0; j < 4; j++ ) {
            file >> word;
            datos.push_back(word);
        }
        Nodo nodito = Nodo( std::stoi(datos[0]), std::stof(datos[1]), std::stof(datos[2]), datos[3][0], std::stoi(datos[4]));
        if ( i != 0 ) {
            nodos.push_back(nodito);
        }
        else {
            planta.push_back(nodito);
        }
        i++;
    }
}

void solucionRandom() {
    std::vector<Nodo> borrable = nodos;
    std::random_shuffle ( borrable.begin(), borrable.end() );
    int n = borrable.size();
    for ( int i = 0; i < totalCamiones; i++ ) {
        std::vector<Nodo> lista;
        solucionInicial.push_back(lista);
    }
    for ( int i = 0; i < n; i++ ) {
        solucionInicial[i%totalCamiones].push_back(borrable[i]);
    }
}

float calidadSolucion() {

}

float calcularDistancia(Nodo a, Nodo b) {
    float i = a.x - b.x;
    float j = a.y - b.y;
    i = i*i;
    j = j*j;
    return sqrt(i+j);

}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
        }

    readFile(argv[1]);
    solucionRandom();
    
    return 0;
}
