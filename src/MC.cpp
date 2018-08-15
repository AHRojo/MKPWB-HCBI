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
std::vector<Camion> camionesMejorSolucion;
std::vector<Nodo> planta;
std::vector<Nodo> nodos;
std::vector<Leche> leches;
std::vector<std::vector<Nodo>> mejorSolucion;
std::vector<std::vector<Nodo>> solucionInicial;
int totalCamiones;

// Se lee el archivo y se guardan los datos sobre camiones, nodos y leche en variables globales.
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
    //Se guardan los datos de los camiones;
    while ( i < n && file >> word ) {
        Camion camioncito = Camion(std::stoi(word));
        camiones.push_back(camioncito);
        i++;
    }
    i = 0;
    file >> word;
    n = std::stoi(word);
    //Se guardan los tipos de leche que hay
    while ( i < n && file >> word ) {
        Leche milk = Leche(std::stoi(word), (char)('A' + i));
        leches.push_back(milk);
        i++;
    }
    i = 0;
    //Se setea la calidad de cada leche
    while ( i < n && file >> word ) {
        leches[i].setValor(std::stof(word));
        i++;
    }
    i = 0;
    file >> word;
    n = std::stoi(word);
    //Se guardan todos los nodos que existen en el modelo
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
            nodito = Nodo( std::stoi(datos[0]), std::stof(datos[1]), std::stof(datos[2]), datos[3][0], 0);
            planta.push_back(nodito);
        }
        i++;
    }
}

//Funcion utilizada para determinar la distancia entre 2 nodos
float calcularDistancia(Nodo a, Nodo b) {
    float i = a.x - b.x;
    float j = a.y - b.y;
    i = (float) pow(i, 2);
    j = (float) pow(j, 2);
    return (float) sqrt(i+j);
}

//Se calcula el ratio Leche/Distancia entre 2 nodos para determinar cual agregar a la solucion greedy
int vecinoMasCercanoRatio(Nodo a, int zona, std::vector<std::vector<Nodo>> porTipo) {
    int i;
    int cantidadNodos = porTipo[zona].size();
    float dist = std::numeric_limits<float>::infinity();
    int index;
    for ( i = 0; i < cantidadNodos; i++ ) {
        float actual = calcularDistancia(a, porTipo[zona][i]);
        if ( actual > dist ) {
            dist = actual;
            index = i;
        }
    }

    return index;
}

//i es el camion que se esta evaluando
//Se utiliza para determinar si existe un deficit de leche en algun camion
//para cumplir la cuota, si entrega un numero negativo quiere decir que hay de sobra.
int calcularDeficit(int i, std::vector<std::vector<Nodo>> solucion) {
    int n = solucion[i].size();
    int total = 0;
    for ( int j = 0; j < n; j++ ) {
        total += solucion[i][j].cantidad;
    }
    return leches[i].cuota - total;
}

/*
Si distibuyen los tipos de leche entre los camiones, el camion 1 recolecta leche tipo A
el 2 B y el 3 C, los nodos sobrantes (cuando se supera la capacidad maxima se agregan a otros camiones que
no cambien la calidad de la leche)
Finalmente se reparan los problemas de deficit extrayendo leche de otros camiones con calidad de leche
superior.

*/
void solucionGreedy() {
    std::vector<std::vector<Nodo>> porTipo;
    int i;
    int cantidadNodos = nodos.size();
    solucionInicial.clear();

    for ( i = 0; i < totalCamiones + 1; i++ ) {
        std::vector<Nodo> lista;
        porTipo.push_back(lista);
        if ( i != totalCamiones ) {
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

    //SE agregan los nodos sobrantes
    int nodosSobrantes = porTipo[3].size();
    for ( i = 0; i < nodosSobrantes; i++ ) {
        if ( porTipo[3][i].tipo == 'A' ) {
            if ( camiones[0].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[0].insert(solucionInicial[0].begin(), porTipo[3][i]);
                camiones[0].disponible -= porTipo[3][i].cantidad;

            }
            else if ( camiones[1].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[1].insert(solucionInicial[1].begin(), porTipo[3][i]);
                camiones[1].disponible -= porTipo[3][i].cantidad;
            }
            else if ( camiones[2].disponible > porTipo[3][i].cantidad ){
                solucionInicial[2].insert(solucionInicial[2].begin(), porTipo[3][i]);
                camiones[2].disponible -= porTipo[3][i].cantidad;
            }
        }
        else if ( porTipo[3][i].tipo == 'B' ) {
            if ( camiones[1].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[1].insert(solucionInicial[1].begin(), porTipo[3][i]);
                camiones[1].disponible -= porTipo[3][i].cantidad;
            }
            else if ( camiones[2].disponible > porTipo[3][i].cantidad ){
                solucionInicial[2].insert(solucionInicial[2].begin(), porTipo[3][i]);
                camiones[2].disponible -= porTipo[3][i].cantidad;
            }
        }
        else{
            if ( camiones[2].disponible > porTipo[3][i].cantidad ) {
                solucionInicial[2].insert(solucionInicial[2].begin(), porTipo[3][i]);
                camiones[2].disponible -= porTipo[3][i].cantidad;
            }
        }
    }


    //Se solucionan los problemas de Deficit

    for ( i = 0; i < totalCamiones; i++ ) {
        if ( calcularDeficit(i, solucionInicial) > 0 ) {
            if ( i == 1 ) {
                for ( int j = solucionInicial[0].size() - 1; j >= 0; j-- ) {
                    if ( (calcularDeficit(0, solucionInicial) + solucionInicial[0][j].cantidad) < 0) {
                        if ( camiones[1].disponible > solucionInicial[0][j].cantidad ) {
                            camiones[1].disponible -= solucionInicial[0][j].cantidad;
                            camiones[0].disponible += solucionInicial[1][j].cantidad;
                            solucionInicial[i].insert(solucionInicial[i].begin(), solucionInicial[0][j]);
                            solucionInicial[0].erase(solucionInicial[0].begin() + j);
                        }
                    }
                }
            }
            else if ( i == 2 ) {
                for ( int j = solucionInicial[1].size() - 1; j >= 0; j-- ) {
                    if ( (calcularDeficit(1, solucionInicial) + solucionInicial[1][j].cantidad) < 0) {
                        if ( camiones[2].disponible > solucionInicial[1][j].cantidad ) {
                            camiones[2].disponible -= solucionInicial[1][j].cantidad;
                            camiones[1].disponible += solucionInicial[1][j].cantidad;
                            solucionInicial[i].insert(solucionInicial[i].begin(), solucionInicial[1][j]);
                            solucionInicial[1].erase(solucionInicial[1].begin() + j);
                        }

                    }
                }
                for ( int j = solucionInicial[0].size() - 1; j >= 0; j-- ) {
                    if ( (calcularDeficit(0, solucionInicial) + solucionInicial[0][j].cantidad) < 0) {
                        if ( camiones[2].disponible > solucionInicial[0][j].cantidad ) {
                            camiones[2].disponible -= solucionInicial[0][j].cantidad;
                            camiones[0].disponible += solucionInicial[0][j].cantidad;
                            solucionInicial[i].insert(solucionInicial[i].begin(), solucionInicial[0][j]);
                            solucionInicial[0].erase(solucionInicial[0].begin() + j);
                        }

                    }
                }
            }
        }
    }
}


/*
Por cada camion se determina la cantidad de leche que recogio en un nodo y
la distancia que hay hacia el proximo nodo, la funcion de calidad corresponde
a Sum(Leche*calidad) - Sum(Distancia de nodo a nodo), en caso de que haya Deficit
se entrega una calidad infinitamente negativa.
*/
float calidadSolucion(std::vector<std::vector<Nodo>> solucion, std::vector<Camion> trucks) {
    int calidad_solucion = 0;
    for ( int i = 0; i < totalCamiones; i++ ) {
        int lecheRecolectada = 0;
        int index = 0;
        int cantidadNodos = solucion[i].size();
        calidad_solucion -= calcularDistancia(planta[0], solucion[i][index]);
        while ( index != cantidadNodos - 1 ) {
            calidad_solucion += solucion[i][index].cantidad * leches[i].valor - calcularDistancia(solucion[i][index],solucion[i][index + 1]);
            lecheRecolectada += solucion[i][index].cantidad;
            index++;
        }
        calidad_solucion += solucion[i][index].cantidad * leches[i].valor - calcularDistancia(solucion[i][index], planta[0]);
        lecheRecolectada += solucion[i][index].cantidad;

        if ( lecheRecolectada < leches[i].cuota ) {
            return -std::numeric_limits<float>::infinity();
            //calidad_solucion += (lecheRecolectada - leches[i].cuota) * leches[i].valor;
        }
        else if ( lecheRecolectada > trucks[i].capacidad ) {
            return -std::numeric_limits<float>::infinity();
            //calidad_solucion += ( camiones[i].capacidad - lecheRecolectada)*camiones[i].capacidad;
        }
    }
    return calidad_solucion;
}
// recorrido en el que estoy, a, b arcos que voy a cambiar y el recorrido.
//Movimiento realizado en el caso corresponde a 2opt.
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

// parametros recorrido en el que estoy, recorrido al que le voy a robar, posicion en la que voy a agregar, posicion a la que le voy a robar, y la representacion.
/*
La funcion toma un nodo de otro camion que lleve calidad mayor y la agrega en
una posicion a del vector que solicita nodos.
*/
std::vector<std::vector<Nodo>> takeNode(int i, int j, int a, int b, std::vector<std::vector<Nodo>> recorrido) {
    recorrido[i].insert(recorrido[i].begin() + a, recorrido[j][b]);
    recorrido[j].erase(recorrido[j].begin() + b);

    return recorrido;
}

int permitirSwap(Nodo a, Nodo b) {
    if (a.tipo == 'A' ) {
        if ( b.tipo == 'A') {
            return 1;
        }
    }
    else if ( a.tipo == 'B' ) {
        if ( b.tipo == 'A') {
            return 1;
        }
        if (b.tipo == 'B') {
            return 1;
        }
    }
    else if ( a.tipo == 'C' ) {
        return 1;
    }
    return 0;
}

std::vector<std::vector<Nodo>> HCBI(std::vector<std::vector<Nodo>> solucion) {
    std::vector<std::vector<Nodo>> candidato;
    float solcandidata;
    float solActual = calidadSolucion(solucion, camiones);
    int flag = 1;
    while ( flag == 1 ) {
        int change = 0;
        for ( int i = 0; i < 3; i++ ) {
            int cantidadNodos = solucion[i].size();
                for ( int j = 0; j < cantidadNodos; j++ ) {
                    for ( int k = j; k < cantidadNodos; k++ ) {
                        std::vector<std::vector<Nodo>> vecino = dosOpt(i, j, k, solucion);
                        solcandidata = calidadSolucion(vecino, camiones);
                        if ( solcandidata > solActual ) {
                            solActual = solcandidata;
                            candidato = vecino;
                            change = 1;
                        }
                    }
                    for ( int k = 0; k < 3; k++) {
                        int cantidadNodosOtros = solucion[k].size();
                        for ( int l = 0; l < cantidadNodosOtros; l++ ) {
                            if ( i != k ) {
                                if ( permitirSwap (solucion[i][j], solucion[k][l]) == 1 ) {
                                    std::vector<std::vector<Nodo>> vecino = takeNode(i, k, j, l, solucion);
                                    solcandidata = calidadSolucion(vecino, camiones);
                                    if ( solcandidata > solActual ) {
                                        solActual = solcandidata;
                                        candidato = vecino;
                                        change = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        if ( change == 1 ) {
            solucion = candidato;
        }
        if ( change == 0 ) {
            flag = 0;
        }
    }
    return solucion;
}

void resetCamiones() {
    std::random_shuffle (camiones.begin(), camiones.end());
    for ( int i = 0; i < totalCamiones; i++ ) {
        camiones[i].disponible = camiones[i].capacidad;
    }
}

void output(std::string s) {
    s += ".out";
    std::ofstream file{s, std::ofstream::out};
    if (!file.good()){
        std::cout << "No se pudo crear el archivo.";
        return;
    }
    file << calidadSolucion(mejorSolucion, camionesMejorSolucion) << "\t";
    //Costo total
    std::vector<std::string> rutas;
    std::vector<float> costoViaje;
    std::vector<int> cantidadLeche;
    for ( int i = 0; i < totalCamiones; i++ ) {
        rutas.push_back("0-");
        costoViaje.push_back(0);
        cantidadLeche.push_back(0);
        int cantidadNodos = mejorSolucion[i].size();
        if (cantidadNodos != 0) {
            costoViaje[i] += calcularDistancia(planta[0], mejorSolucion[i][0]);
        }
        int index = 0;
        while ( index != cantidadNodos - 1 && cantidadNodos != 0 ) {
            rutas[i] += mejorSolucion[i][index].id + "-";
            cantidadLeche[i] += mejorSolucion[i][index].cantidad;
            costoViaje[i] += calcularDistancia(mejorSolucion[i][index], mejorSolucion[i][index]);
            index++;
        }
        rutas[i] += mejorSolucion[i][index].id + "-0";
        cantidadLeche[i] += mejorSolucion[i][index].cantidad;
        costoViaje[i] += calcularDistancia(mejorSolucion[i][index], planta[0]);
        if ( cantidadLeche[i] > camionesMejorSolucion[i].capacidad ) {
            std::cout << "ESTO EXPLOTO! se supera la capacidad.\n";
        }
        if ( leches[i].cuota > cantidadLeche[i] ) {
            std::cout << "ESTO EXPLOTO! no se alcanza la cuota.\n";
        }
    }
    int totalLeche = 0;
    float costoViajeT = 0;
    for ( int i = 0; i < cantidadLeche.size(); i++ ) {
        totalLeche += cantidadLeche[i]*leches[i].valor;
        costoViajeT += costoViaje[i];
    }

    file << costoViajeT << "\t" << totalLeche << "\n";
    for (int i = 0; i < totalCamiones; i++ ) {
        file << rutas[i] << "\t" << costoViaje[i] << "\t" << cantidadLeche[i] << leches[i].tipo << "\n";
        std::cout << rutas[i] << "\t" << costoViaje[i] << "\t" << cantidadLeche[i] << leches[i].tipo << "\n";
    }

    file.close();
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
        }
    std::vector<std::vector<Nodo>> candidato;

    readFile(argv[1]);

    solucionGreedy();
    mejorSolucion = solucionInicial;
    camionesMejorSolucion = camiones;
    candidato = mejorSolucion;

    resetCamiones();
    for ( int r = 0; r < 10; r++ ) {
        solucionGreedy();
        candidato = HCBI(solucionInicial);
        if ( calidadSolucion(candidato, camiones) > calidadSolucion(mejorSolucion, camionesMejorSolucion) ) {
            mejorSolucion = candidato;
            camionesMejorSolucion = camiones;
            }
        candidato.clear();
        resetCamiones();
    }

     for ( int i = 0; i < 3; i++ ) {
     int alo = mejorSolucion[i].size();
    std::cout << "Camion" << i << "\n";
   for( int j = 0; j < alo; j++ ) {
     mejorSolucion[i][j].toString();
    }
     std::cout << "\n\n"; }
     std::cout << calidadSolucion(mejorSolucion, camionesMejorSolucion);

//    output(argv[1]);



    return 0;
}
