#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <limits>

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
            Nodo nodito = Nodo( std::stoi(datos[0]), std::stof(datos[1]), std::stof(datos[2]), datos[3][0], 0);
            planta.push_back(nodito);
        }
        i++;
    }
}

float calcularDistancia(Nodo a, Nodo b) {
    float i = a.x - b.x;
    float j = a.y - b.y;
    i = pow(i, 2);
    j = pow(j, 2);
    return sqrt(i+j);
}

int vecinoMasCercanoRatio(Nodo a, int zona, std::vector<std::vector<Nodo>> porTipo) {
    int i;
    int cantidadNodos = porTipo[zona].size();
    float dist = std::numeric_limits<float>::infinity();;
    int index;
    for ( i = 0; i < cantidadNodos; i++ ) {
        float actual = calcularDistancia(a, porTipo[zona][i]);
        if ( actual > dist ) {
            dist = actual;
            index = i;
        }
        std::cout << "MAS CERCANO " << actual << "  \n\n";
    }

    return index;
}

void solucionRandom() {
    std::vector<std::vector<Nodo>> porTipo;
    int i;
    int cantidadNodos = nodos.size();

    for ( i = 0; i < totalCamiones + 1; i++ ) {
        std::vector<Nodo> lista;
        porTipo.push_back(lista);
        if ( i != totalCamiones + 1 ) {
            solucionInicial.push_back(lista);
        }
    }

    for ( i = 0; i < cantidadNodos; i++ ) {
        if ( nodos[i].tipo == 'A' ) {
            porTipo[0].push_back(nodos[i]);
        }
        else if ( nodos[i].tipo == 'B' ) {
            porTipo[1].push_back(nodos[i]);
        }
        else {
            porTipo[2].push_back(nodos[i]);
        }
    }

    // int agregados = 0;
    for ( i = 0; i < totalCamiones; i++ ) {
        int capacidadActual = camiones[i].capacidad;
        int indexMasCercano;
        Nodo actual = planta[0];
        while ( porTipo[i].size() != 0 && capacidadActual > 0 ) {
            indexMasCercano = vecinoMasCercanoRatio(actual, i, porTipo);
            if ( capacidadActual > porTipo[i][indexMasCercano].cantidad ) {
                solucionInicial[i].push_back(porTipo[i][indexMasCercano]);
                capacidadActual -= porTipo[i][indexMasCercano].cantidad;
                // agregados += 1;
            }
            else {
                porTipo[3].push_back(porTipo[i][indexMasCercano]);
            }
            porTipo[i].erase(porTipo[i].begin() + indexMasCercano);
        }
    }
    int nodosSobrantes = porTipo[3].size();
    for ( i = 0; i < nodosSobrantes; i++ ) {

    }

    // for ( i = 0; i < 3; i++ ) {
    //     int alo = solucionInicial[i].size();
    //     std::cout << "Camion" << i << "\n";
    //     for( int j = 0; j < alo; j++ ) {
    //
    //         solucionInicial[i][j].toString();
    //     }
    //     std::cout << "\n\n";
    // }
    // std::cout << agregados << "\n";


}

float calidadSolucion() {
    float total = 0;
    for ( int i = 0; i < totalCamiones; i++ ) {
        std::cout << " PARA CAMION " << i << "\n\n\n";
        int index = 0;
        int cantidadNodos = mejorSolucion[i].size();
        total -= calcularDistancia(planta[0], mejorSolucion[i][index]);
        while ( index != cantidadNodos - 1 ) {
            total += mejorSolucion[i][index].cantidad * leches[i].valor - calcularDistancia(mejorSolucion[i][index], mejorSolucion[i][index + 1]);
            leches[i].recolectar(mejorSolucion[i][index].cantidad);
            std::cout << "DESDE NODO " << mejorSolucion[i][index].id << " HASTA " << mejorSolucion[i][index + 1].id << "\n";
            std::cout << mejorSolucion[i][index].cantidad * leches[i].valor << "     " << calcularDistancia(mejorSolucion[i][index], mejorSolucion[i][index + 1]) << "\n\n";
            index++;
        }
        total += mejorSolucion[i][index].cantidad * leches[i].valor - calcularDistancia(mejorSolucion[i][index], planta[0]);
        leches[i].recolectar(mejorSolucion[i][index].cantidad);
    }
    std::cout << "CALIDAD " << total << "\n";
    std::cout << "cuotas " << "\n";
    std::cout << "CALIDAD A " << "  " << leches[0].cuota << "  " << leches[0].recolectado << "\n\n";
    std::cout << "CALIDAD B " <<"  " <<  leches[1].cuota <<"  " <<  leches[1].recolectado << "\n\n";
    std::cout << "CALIDAD C " <<"  " <<  leches[2].cuota <<"  " <<  leches[2].recolectado << "\n\n";
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
        }

    readFile(argv[1]);
    solucionRandom();
    mejorSolucion = solucionInicial;
    calidadSolucion();

    return 0;
}
