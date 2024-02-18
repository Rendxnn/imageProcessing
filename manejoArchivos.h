#ifndef MANEJOARCHIVOS_H
#define MANEJOARCHIVOS_H

#include <vector>
using namespace std;

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

std::vector<std::vector<Pixel>> leerArchivoBMP(const char* nombreArchivo);

void guardarMatrizEnBMP(const char* nombreArchivo, const vector<vector<Pixel>>& matriz);

#endif