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
float calidad_mejor_solucion;
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
        // std::cout << "MAS CERCANO " << actual << "  \n\n";
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
        int indexMasCercano;
        Nodo actual = planta[0];
        while ( porTipo[i].size() != 0 && camiones[i].disponible > 0 ) {
            indexMasCercano = vecinoMasCercanoRatio(actual, i, porTipo);
            if ( camiones[i].disponible > porTipo[i][indexMasCercano].cantidad ) {
                solucionInicial[i].push_back(porTipo[i][indexMasCercano]);
                camiones[i].disponible -= porTipo[i][indexMasCercano].cantidad;
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
        if ( porTipo[3][i].tipo == 'A' ) {
            if ( camiones[0].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[0].push_back(porTipo[3][i]);
            }
            else if ( camiones[1].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[1].push_back(porTipo[3][i]);
            }
            else {
                solucionInicial[2].push_back(porTipo[3][i]);
            }
        }
        else if ( porTipo[3][i].tipo == 'B' ) {
            if ( camiones[1].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[1].push_back(porTipo[3][i]);
            }
            else {
                solucionInicial[2].push_back(porTipo[3][i]);
            }
        }
        else {
            solucionInicial[2].push_back(porTipo[3][i]);
        }
    }


}

void verCuotas() {
    for ( int i = 0; i < totalCamiones; i++ ) {
        int cantidadNodos = mejorSolucion[i].size();
        for ( int j = 0; j < cantidadNodos; j++ ) {
            leches[i].recolectar(mejorSolucion[i][j].cantidad);
        }
    }
    // std::cout << "cuotas " << "\n";
    // std::cout << "CALIDAD A " << "  " << leches[0].cuota << "  " << leches[0].recolectado << "\n\n";
    // std::cout << "CALIDAD B " <<"  " <<  leches[1].cuota <<"  " <<  leches[1].recolectado << "\n\n";
    // std::cout << "CALIDAD C " <<"  " <<  leches[2].cuota <<"  " <<  leches[2].recolectado << "\n\n";
}

float calidadSolucion(std::vector<std::vector<Nodo>> solucion) {
    int calidad_solucion = 0;
    for ( int i = 0; i < totalCamiones; i++ ) {
        //std::cout << " PARA CAMION " << i << "\n\n\n";
        int index = 0;
        int cantidadNodos = solucion[i].size();
        calidad_solucion -= calcularDistancia(planta[0], solucion[i][index]);
        while ( index != cantidadNodos - 1 ) {
            calidad_solucion += solucion[i][index].cantidad * leches[i].valor - calcularDistancia(solucion[i][index],solucion[i][index + 1]);
            // std::cout << "DESDE NODO " << solucion[i][index].id << " HASTA " << solucion[i][index + 1].id << "\n";
            // std::cout << solucion[i][index].cantidad * leches[i].valor << "     " << calcularDistancia(solucion[i][index], solucion[i][index + 1]) << "\n\n";
            index++;
        }
        calidad_solucion += solucion[i][index].cantidad * leches[i].valor - calcularDistancia(solucion[i][index], planta[0]);
    }
    //std::cout << "CALIDAD " << calidad_solucion << "\n";
    return calidad_solucion;
}

std::vector<std::vector<Nodo>> dosOpt(int i, int a, int b, std::vector<std::vector<Nodo>> recorrido) {
    //std::reverse(recorrido[i][a], recorrido[i][b]);
    std::vector<Nodo> cambio;
    int c;
    int cantidadNodos = recorrido[i].size();
    for ( c = 0; c < a; c++ ) {
        cambio.push_back(recorrido[i][c]);
    }
    for ( c = b; c >= a; c-- ) {
        cambio.push_back(recorrido[i][c]);
    }
    for ( c = b + 1; c < cantidadNodos; c++ ) {
        cambio.push_back(recorrido[i][c]);
    }
    recorrido[i] = cambio;
    return recorrido;
}

std::vector<std::vector<Nodo>> HCBI(std::vector<std::vector<Nodo>> solucion) {
    std::vector<std::vector<Nodo>> candidato;
    float solcandidata;
    float solActual = calidad_mejor_solucion;
    float sol2opt;
    int contador = 0;
    for ( int i = 0; i < 3; i++ ) {
        int flag = 1;
        int cantidadNodos = solucion[i].size();
        while ( flag == 1 ) {
            int change = 0;
            for ( int j = 0; j < cantidadNodos; j++ ) {
                for ( int k = j; k < cantidadNodos; k++ ) {
                    std::vector<std::vector<Nodo>> vecino = dosOpt(i, j, k, solucion);
                    solcandidata = calidadSolucion(vecino);
                    if ( solcandidata > solActual ) {
                        solActual = solcandidata;
                        candidato = vecino;
                        change = 1;
                        contador += 1;
                    }
                }
            }
            std::cout << solActual << "\n";
            solucion = candidato;
            if ( change == 0 ) {
                flag = 0;
            }
        }
    }
    std::cout << contador << "\n";
    if ( solActual > calidad_mejor_solucion ) {
        calidad_mejor_solucion = solActual;
    }
    return solucion;
}

std::vector<std::vector<Nodo>> restart(std::vector<std::vector<Nodo>> solucion) {
    return solucion;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
        }

    readFile(argv[1]);
    solucionRandom();
    mejorSolucion = solucionInicial;
    calidad_mejor_solucion = calidadSolucion(solucionInicial);
    std::vector<std::vector<Nodo>> nueva_sol = solucionInicial;

    std::cout << calidad_mejor_solucion << "\n";
    mejorSolucion = HCBI(nueva_sol);
    std::cout << calidad_mejor_solucion << "\n";


    // for ( int i = 0; i < 1000; i++) {
    //     nueva_sol = restart(nueva_sol);
    // }
    // for ( i = 0; i < 3; i++ ) {
    //     int alo = solucionInicial[i].size();
    //     std::cout << "Camion" << i << "\n";
    //     for( int j = 0; j < alo; j++ ) {
    //
    //         solucionInicial[i][j].toString();
    //     }
    //     std::cout << "\n\n";
    // }
    //std::cout << agregados << "\n";

    return 0;
}
