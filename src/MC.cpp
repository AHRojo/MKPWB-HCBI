#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "nodo.h"
#include "camion.h"
#include "leche.h"

std::vector<Camion> camiones;
std::vector<Nodo> nodos;
std::vector<Leche> leches;

void readFile(std::string arch) {
    std::ifstream file(arch);
    std::string word;
    std::string::size_type sz;
    int i = 0;
    int n;
    int j;

    file >> word;
    n = std::stoi(word);
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
    while ( i < n && file >> word ) {
        std::vector<std::string> datos;
        datos.clear();
        datos.push_back(word);
        for ( j = 0; j < 4; j++ ) {
            file >> word;
            datos.push_back(word);
        }
        std::cout << datos[0] << "   " << datos[1] << "   "  << datos[2] << "   "  << datos[3] << "   "  << datos[4] << "   "  << "\n";
        Nodo nodito = Nodo( std::stoi(datos[0]), std::stof(datos[1]), std::stof(datos[2]), datos[3][0], std::stoi(datos[4]));
        nodos.push_back(nodito);
        i++;
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
        }


    readFile(argv[1]);

    return 0;
}
