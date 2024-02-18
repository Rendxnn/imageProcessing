#include "manejoArchivos.h"
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;


vector<int> calcularPosicion(vector<int> coordenadas, int anguloGrados) {
    double anguloRadianes = anguloGrados * M_PI / 180;

    double seno = sin(anguloRadianes);
    double coseno = cos(anguloRadianes);

    vector<vector<double>> matrizTransformacion = {{coseno, -seno}, {seno, coseno}};
    vector<int> nuevasCoordenadas = {0, 0};
    
    nuevasCoordenadas[0] = static_cast<int>(round(coordenadas[0] * matrizTransformacion[0][0] + coordenadas[1] * matrizTransformacion[0][1]));
    nuevasCoordenadas[1] = static_cast<int>(round(coordenadas[0] * matrizTransformacion[1][0] + coordenadas[1] * matrizTransformacion[1][1]));

    return nuevasCoordenadas;
}

vector<vector<Pixel>> rotarImagen(vector<vector<Pixel>> imagen, int anguloGrados) {
    int lado = max(imagen.size(), imagen[0].size());
    vector<vector<Pixel>> imagenRotada(lado, vector<Pixel>(lado));

    int alto = imagen.size();
    int ancho = imagen[0].size();

    int iNuevo = 0;
    int jNuevo = 0;

    
    for (int i = 0; i < imagen.size(); i++) {
        for (int j = 0; j < imagen[i].size(); j++) {

            vector<int> coordenadas = {i - alto / 2, j - alto / 2};
            vector<int> nuevasCoordenadas = calcularPosicion(coordenadas, anguloGrados);

            iNuevo = nuevasCoordenadas[0] + alto / 2;
            jNuevo = nuevasCoordenadas[1] + ancho / 2;

            if (iNuevo >= 0 && iNuevo < lado && jNuevo >= 0 && jNuevo < lado) {
                imagenRotada[iNuevo][jNuevo].red = imagen[i][j].red;
                imagenRotada[iNuevo][jNuevo].green = imagen[i][j].green;
                imagenRotada[iNuevo][jNuevo].blue = imagen[i][j].blue;
            }
        }

    }

    return imagenRotada;
}



int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <nombre_del_archivo_entrada.bmp> <nombre_del_archivo_salida.bmp> <cantidad de grados a rotar>" << endl;
        return 1;
    }
    const char* nombreArchivoLecturaBMP = argv[1];
    const char* nombreArchivoEscrituraBMP = argv[2];
    int gradosRotar = atoi(argv[3]);

    // Leer el archivo BMP y obtener la matriz de píxeles
    vector<vector<Pixel>> imagen = leerArchivoBMP(nombreArchivoLecturaBMP);


    vector<vector<Pixel>> imagenRotada = rotarImagen(imagen, gradosRotar);
    

    guardarMatrizEnBMP(nombreArchivoEscrituraBMP, imagenRotada);

    return 0;
}