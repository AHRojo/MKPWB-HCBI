#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <limits>
#include <ctime>
#include <iomanip>

#include "nodo.h"
#include "camion.h"
#include "leche.h"

/*
Factores para penalización de capacidad y cuota y magnitud de la distancia
*/
float PDISTANCIA = 1.0;
float PCAPACIDAD = 1.0;
float PCUOTA = 1.0;

std::vector<Camion> camiones;
std::vector<Camion> camionesMejorSolucion;
std::vector<Nodo> planta;
std::vector<Nodo> nodos;
std::vector<Leche> leches;
std::vector<std::vector<Nodo>> mejorSolucion;
std::vector<std::vector<Nodo>> solucionInicial;
int totalCamiones;
int sol;
std::vector<Camion> iniciales;

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
    iniciales = camiones;
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

//Se calcula el ratio Leche/Distancia entre 2 nodos para determinar cual agregar a la solucion greedy,
//Se compara ademas si los nodos son factibles para ingresar a la ruta de un camion, si cumple con la calidad de leche
int vecinoMasCercanoRatio(Nodo a, int zona, std::vector<Nodo> nodes) {
    int i;
    int cantidadNodos = (int) nodes.size();
    float dist = -1;
    int index = 0;
    for ( i = 0; i < cantidadNodos; i++ ) {
        float actual = -1;
        if ( nodes[i].tipo <= leches[zona].tipo ) {
            actual = calcularDistancia(a, nodes[i]);
        }
        if ( actual > dist ) {
            dist = actual;
            index = i;
        }
    }
    if ( dist < 0 ) {
        return -1;
    }
    return index;
}

void solucionRandom() {
    int cantidadNodos = (int) nodos.size();
    solucionInicial.clear();
    for ( int i = 0; i < totalCamiones; i++ ) {
        std::vector<Nodo> lista;
        solucionInicial.push_back(lista);
    }
    std::random_shuffle (nodos.begin(), nodos.end()); //Se barajan los nodos
    int module;
    for ( int i = 0; i < cantidadNodos; i++ ) { //Se agregan los nodos a distintos camiones, si es que se permite
        module = i%totalCamiones;
        int flag = 1;
        while ( flag == 1 ) {
            flag = 1;
            if ( nodos[i].tipo <= leches[module].tipo ) { //Se compara si la calidad de leche es igual o supera la que lleva
                solucionInicial[module].push_back(nodos[i]);
                flag = 0;
            }
            module += 1;
        }
    }
}

/*
Se distibuyen los tipos de leche entre los camiones, el camion 1 recolecta leche tipo A
el 2 B y el 3 C
*/
void solucionGreedy() {
    std::vector<Nodo> nodosBorrables;
    nodosBorrables = nodos;
    int i;
    solucionInicial.clear();
    //Se agregan tantos vectores como camiones haya a la solucion.
    for ( i = 0; i < totalCamiones; i++ ) {
        std::vector<Nodo> lista;
        solucionInicial.push_back(lista);
    }
    //Se agregan los nodos a las rutas
    for ( i = 0; i < totalCamiones; i++ ) {
        int indexMasCercano;
        int cantidadNodos = (int) nodosBorrables.size();
        Nodo actual = planta[0];
        for ( int j = 0; j < cantidadNodos; j++ ) {
            //Se determina el vecino con el mejor ratio leche/distancia
            indexMasCercano = vecinoMasCercanoRatio(actual, i, nodosBorrables);
            if ( indexMasCercano >= 0 ) {
                solucionInicial[i].push_back(nodosBorrables[indexMasCercano]);
                nodosBorrables.erase(nodosBorrables.begin() + indexMasCercano);
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
    float calidad_solucion = 0;
    for ( int i = 0; i < totalCamiones; i++ ) {
        float lecheRecolectada = 0;
        int index = 0;
        int cantidadNodos = (int) solucion[i].size();
        if ( cantidadNodos > 0) {
            //Se calcula la distancia desde la planta al primer nodo.
            calidad_solucion -= calcularDistancia(planta[0], solucion[i][index])*PDISTANCIA;
            while ( index != cantidadNodos - 1 ) {
                //Se agregan las cantidades de leche de los nodos y se restan sus distancias, excepto el ultimo
                calidad_solucion += (float) solucion[i][index].cantidad * (float) leches[i].valor - calcularDistancia(solucion[i][index],solucion[i][index + 1])*PDISTANCIA;
                lecheRecolectada += (float) solucion[i][index].cantidad;
                index++;
            }
            //aca se calcula el ultimo
            calidad_solucion += (float) solucion[i][index].cantidad * (float) leches[i].valor - calcularDistancia(solucion[i][index], planta[0])*PDISTANCIA;
            lecheRecolectada += (float) solucion[i][index].cantidad;
        }
        //Se penaliza el deficit de cuota y el exceso de capacidad
        if ( lecheRecolectada < (float) leches[i].cuota ) {
            calidad_solucion += (lecheRecolectada - (float) leches[i].cuota)*PCUOTA;
        }
        else if ( lecheRecolectada > (float) trucks[i].capacidad ) {
            calidad_solucion += ( (float) trucks[i].capacidad - lecheRecolectada)*PCAPACIDAD;
        }
    }
    return calidad_solucion;
}
// recorrido en el que estoy, a, b arcos que voy a cambiar y el recorrido.
//Movimiento realizado en el caso corresponde a 2opt.
std::vector<std::vector<Nodo>> dosOpt(int i, int a, int b, std::vector<std::vector<Nodo>> recorrido) {
    std::vector<Nodo> cambio;
    int c;
    int cantidadNodos = (int) recorrido[i].size();
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

// parametros recorrido en el que estoy, recorrido al que le voy a robar,
//posicion en la que voy a agregar, posicion a la que le voy a robar, y la
//representacion.
/*
La funcion toma un nodo de otro camion que lleve calidad mayor y la agrega en
una posicion a del vector que solicita nodos.
*/
std::vector<std::vector<Nodo>> takeNode(int i, int j, int a, int b, std::vector<std::vector<Nodo>> recorrido) {
    std::vector<Nodo>::iterator it = recorrido[i].begin();
    std::vector<Nodo>::iterator ot = recorrido[j].begin();
    Nodo node = recorrido[j][b];
    it = recorrido[i].insert(it + a, node);
    ot = recorrido[j].erase(ot + b);
    return recorrido;
}

//Se intercambian los nodos a y b de los camiones i y j.
std::vector<std::vector<Nodo>> swapNodes(int i, int j, int a, int b, std::vector<std::vector<Nodo>> recorrido) {
    std::vector<Nodo>::iterator it = recorrido[i].begin();
    std::vector<Nodo>::iterator ot = recorrido[j].begin();
    Nodo node1 = recorrido[i][a];
    Nodo node2 = recorrido[j][b];
    it = recorrido[i].insert(it + a, node2);
    ot = recorrido[j].insert(ot + b, node1);
    it = recorrido[i].begin();
    ot = recorrido[j].begin();
    it = recorrido[i].erase(it + a + 1);
    ot = recorrido[j].erase(ot + b + 1);
    return recorrido;
}

//Se compara si la leche no arruina la calidad de la leche de otro camión.
int permitirTake(Nodo a, Nodo b) {
    if ( a.tipo >= b.tipo ) {
        return 1;
    }
    return 0;
}

//Se revisa si al realizar un swap no se arruinan calidades de leche.
int permitirSwap(Nodo a, Nodo b) {
    if ( a.tipo == b.tipo ) {
        return 1;
    }
    return 0;
}

//Algoritmo principal de hill climbing
std::vector<std::vector<Nodo>> HCBI(std::vector<std::vector<Nodo>> solucion) {
    std::vector<std::vector<Nodo>> candidato;
    float solcandidata;
    float solActual = calidadSolucion(solucion, camiones);
    int flag = 1;
    //Se repite el proceso hasta que no se encuentre un mejor vecino que el actual
    while ( flag == 1 ) {
        int change = 0;
        //Se recorren todos los camiones
        for ( int i = 0; i < 3; i++ ) {
            int cantidadNodos = (int) solucion[i].size();
                for ( int j = 0; j < cantidadNodos; j++ ) {
                    for ( int k = j; k < cantidadNodos; k++ ) {
                        //Se realizan movimientos 2opt entre los nodos de un mismo camion
                        std::vector<std::vector<Nodo>> vecino = dosOpt(i, j, k, solucion);
                        solcandidata = calidadSolucion(vecino, camiones);
                        //Se revisa si el nuevo vecino es mejor que el mejor candidato actual
                        if ( solcandidata > solActual ) {
                            solActual = solcandidata;
                            candidato = vecino;
                            change = 1;
                        }
                    }
                    //Se recorren los otros camiones
                    for ( int k = 0; k < totalCamiones; k++) {
                        int cantidadNodosOtros = (int) solucion[k].size();
                        if ( i != k ) {
                            for ( int l = 0; l < cantidadNodosOtros; l++ ) {
                                //Se compara si es posible robar un nodo a otro camion
                                if ( permitirTake (solucion[i][j], solucion[k][l]) == 1 ) {
                                    std::vector<std::vector<Nodo>> vecino = takeNode(i, k, j, l, solucion);
                                    solcandidata = calidadSolucion(vecino, camiones);
                                    if ( solcandidata > solActual ) {
                                        solActual = solcandidata;
                                        candidato = vecino;
                                        change = 1;
                                    }
                                }
                                //Se revisa si es posible realizar un swap
                                if ( permitirSwap (solucion[i][j], solucion[k][l]) == 1 ) {
                                    std::vector<std::vector<Nodo>> vecino = swapNodes(i, k, j, l, solucion);
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
        //Si change = 0, significa que no existe un vecino con mejor calidad,
        //si change = 1, se mueve hacia el vecino y se repite el proceso.
        if ( change == 1 ) {
            solucion = candidato;
        }
        //Si no hubo mejor candidato se detiene el loop.
        else {
            flag = 0;
        }
    }
    return solucion;
}

//reset, se barajan los nodos y el orden de los camiones
//por mala implementación es necesario barajar los camiones e.e
void reset() {
    std::random_shuffle ( nodos.begin(), nodos.end() );
    std::random_shuffle ( camiones.begin(), camiones.end() );
    for ( int i = 0; i < totalCamiones; i++ ) {
        camiones[i].disponible = camiones[i].capacidad;
    }
}

//Escritura en archivo .out
//Nada interesante es calcular la calidad de la solución por camiones separados
//Y escribir
void output(std::string s) {
    int asd = 0;
    asd = (int) s.find(".txt", asd);
    s.replace(asd, 4, ".out");
    std::ofstream file(s, std::ofstream::out);
    if (!file.good()){
        std::cout << "No se pudo crear el archivo.";
        return;
    }
    file << std::left << round(calidadSolucion(mejorSolucion, camionesMejorSolucion)*10)/10 << "\t";
    std::vector<std::string> rutas;
    std::vector<float> costoViaje;
    std::vector<float> cantidadLeche;
    for ( int i = 0; i < totalCamiones; i++ ) {
        rutas.push_back("1-");
        costoViaje.push_back(0);
        cantidadLeche.push_back(0);
        int cantidadNodos = (int) mejorSolucion[i].size();
        if (cantidadNodos != 0) {
            costoViaje[i] += calcularDistancia(planta[0], mejorSolucion[i][0])*PDISTANCIA;
            int index = 0;
            while ( index != cantidadNodos - 1 && cantidadNodos != 0 ) {
                rutas[i] += std::to_string(mejorSolucion[i][index].id) + "-";
                cantidadLeche[i] += (float) mejorSolucion[i][index].cantidad;
                costoViaje[i] += calcularDistancia(mejorSolucion[i][index], mejorSolucion[i][index])*PDISTANCIA;
                index++;
            }
            rutas[i] += std::to_string(mejorSolucion[i][index].id) + "-1";
            cantidadLeche[i] += (float) mejorSolucion[i][index].cantidad;
            costoViaje[i] += calcularDistancia(mejorSolucion[i][index], planta[0])*PDISTANCIA;
        }
    }
    float totalLeche = 0;
    float costoViajeT = 0;
    for ( int i = 0; i < (int) cantidadLeche.size(); i++ ) {
        totalLeche += (float) cantidadLeche[i]*leches[i].valor;
        costoViajeT += costoViaje[i];
    }
    file << std::left << round(costoViajeT*10)/10 << "\t" << round(totalLeche) << "\n";
    for ( int i = 0; i < totalCamiones; i++ ) {
        camionesMejorSolucion[i].disponible = camionesMejorSolucion[i].capacidad;
        for ( int j = 0; j < totalCamiones; j++ ) {
            if ( camionesMejorSolucion[i].capacidad == iniciales[j].capacidad ) {
                iter_swap(camionesMejorSolucion.begin() + i, camionesMejorSolucion.begin() + j);
                iter_swap(rutas.begin() + i, rutas.begin() + j);
                iter_swap(costoViaje.begin() + i, costoViaje.begin() + j);
                iter_swap(cantidadLeche.begin() + i, cantidadLeche.begin() + j);
                iter_swap(leches.begin() + i, leches.begin() + j);
                break;
            }
        }
    }
    for (int i = 0; i < totalCamiones; i++ ) {
        file << std::left << std::setw(40) << rutas[i] << "\t" << round(costoViaje[i]*10)/10 << "\t" << round(cantidadLeche[i]) << leches[i].tipo << "\n";
        }
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "No file name entered. Exiting...";
        return -1;
        }
    std::vector<std::vector<Nodo>> candidato;
    int r;
    readFile(argv[1]);
    /*
    Si se descomenta esto al ejecutar el programa es posible cambiar el factor de penalizacion.
    */
    // std::cout << "Capacidad: ";
    // std::cin >> PCAPACIDAD;
    // std::cout << "Cuota: ";
    // std::cin >> PCUOTA;
    std::cout << "1 solucion greedy, cualquier otro solucion semi random: ";
    std::cin >> sol;
    if ( sol != 1 ) {
        std::cout << "Cantidad de restarts: ";
        std::cin >> r;
    }
    clock_t start_s=clock();
    if ( sol == 1 ) {
        solucionGreedy();
        r = 9; //como implemente mal los camiones tengo que hacer estos restart e.e para cambiar que camion lleva que.
    }
    else {
        solucionRandom();
    }
    mejorSolucion = solucionInicial;
    camionesMejorSolucion = camiones;
    candidato = mejorSolucion;
    //restarts
    for ( int i = 0; i < r; i++ ) {
        //se realiza HCBI
        candidato = HCBI(solucionInicial);
        //Si la calidad de la nueva solucion es mejor que la mejor solucion se reemplaza
        if ( calidadSolucion(candidato, camiones) > calidadSolucion(mejorSolucion, camionesMejorSolucion) ) {
            mejorSolucion = candidato;
            camionesMejorSolucion = camiones; //como implemente mal los camiones tambien tengo que guardarlos asi e.e
            }
        candidato.clear();
        reset();
        if ( sol == 1 ) {
            solucionGreedy();
        }
        else {
            solucionRandom();
        }
    }
    int stop_s=(int) clock();
    //se muestra la solucion por terminal
    for ( int i = 0; i < 3; i++ ) {
        int alo = (int) mejorSolucion[i].size();
        std::cout << "Camion" << i << "\n";
        for( int j = 0; j < alo; j++ ) {
            mejorSolucion[i][j].toString();
        }
        std::cout << "\n\n";
    }
    std::cout << calidadSolucion(mejorSolucion, camionesMejorSolucion) << "\n";
    std::cout << "tiempo: " << (double)(stop_s-start_s)/double(CLOCKS_PER_SEC) << "segundos\n";
    //Se escribe en output.
    output(argv[1]);

    return 0;
}
