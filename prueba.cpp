#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <limits>

int main() {
    std::vector<int> numeros;
    numeros.push_back(1);
    numeros.push_back(3);
    numeros.push_back(20);

    numeros.insert(numeros.begin(), 33);
    std::cout << numeros[0] << "\n";
}
