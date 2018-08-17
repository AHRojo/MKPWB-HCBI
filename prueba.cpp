#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <limits>
#include <ctime>

std::vector<std::vector<int> > swapNodes(int i, int j, int a, int b, std::vector<std::vector<int> > recorrido) {
    std::vector<int>::iterator it = recorrido[i].begin();
    std::vector<int>::iterator ot = recorrido[j].begin();
    int node1 = recorrido[i][a];
    int node2 = recorrido[j][b];
    it = recorrido[i].insert(it + a, node2);
    ot = recorrido[j].insert(ot + b, node1);
    it = recorrido[i].begin();
    ot = recorrido[j].begin();
    it = recorrido[i].erase(it + a + 1);
    ot = recorrido[j].erase(ot + b + 1);
    return recorrido;
}

int main() {
    std::vector<std::vector<int> > biggg;
    std::vector<int> numero1;
    // std::cout << "HOLA";
    std::vector<int> numero2;
    // std::cout << "HOLA";
    numero1.push_back(1);
    // std::cout << "HOLA";
    numero1.push_back(2);
    // std::cout << "HOLA";
    numero1.push_back(3);
    // std::cout << "HOLA";
    numero2.push_back(4);
    // std::cout << "HOLA";
    numero2.push_back(5);
    // std::cout << "HOLA";
    numero2.push_back(6);
    // std::cout << "HOLA";
    biggg.push_back(numero1);
    // std::cout << "HOLA";
    biggg.push_back(numero2);
    // std::cout << "HOLA";
    biggg = swapNodes(0, 1, 0, 2, biggg);
    // std::cout << "HOLA";
    for ( int i = 0; i < 2; i++ ) {
        int aa = biggg[i].size();
        for ( int j = 0; j < aa; j++ ) {
            std::cout << biggg[i][j] << ", ";
        }
        std::cout << "\n";
    }

}
