#include "manejoArchivos.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <thread>

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


void rotarImagen(vector<vector<Pixel>>& imagen, vector<vector<Pixel>>& imagenRotada, int anguloGrados, int filaInicial, int filaFinal) {
    int lado = max(imagen.size(), imagen[0].size());
    int largo = imagen.size();
    int ancho = imagen[0].size();

    int iNuevo = 0;
    int jNuevo = 0;

    for (int i = filaInicial; i < filaFinal; i++) {
        for (int j = 0; j < imagen[i].size(); j++) {
            vector<int> coordenadas = {i - largo / 2, j - ancho / 2};
            vector<int> nuevasCoordenadas = calcularPosicion(coordenadas, anguloGrados);

            iNuevo = nuevasCoordenadas[0] + largo / 2;
            jNuevo = nuevasCoordenadas[1] + ancho / 2;

            if (iNuevo >= 0 && iNuevo < lado && jNuevo >= 0 && jNuevo < lado) {
                // Asigna directamente los valores de los píxeles
                imagenRotada[iNuevo][jNuevo].red = imagen[i][j].red;
                imagenRotada[iNuevo][jNuevo].green = imagen[i][j].green;
                imagenRotada[iNuevo][jNuevo].blue = imagen[i][j].blue;
            }
        }
    }
    return;
}


void rotarImagenHilos(vector<vector<Pixel>>& imagen, vector<vector<Pixel>>& nuevaImagen, int anguloGrados) {
    int numHilos = omp_get_max_threads();
    vector<thread> hilos;
    int filasPorHilo = imagen.size() / numHilos;
    for (int i = 0; i < numHilos; i++) {
        int inicioFila = i * filasPorHilo;
        int finFila = (i == numHilos - 1) ? imagen.size() : (i + 1) * filasPorHilo;
        hilos.emplace_back([&imagen, &nuevaImagen, inicioFila, finFila, anguloGrados]() {
            rotarImagen(imagen, nuevaImagen, anguloGrados, inicioFila, finFila);
        });
    }
    for (std::thread& hilo : hilos) {
        hilo.join();
    }
}



int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Uso: " << argv[0] << " <nombre_del_archivo_entrada.bmp> <nombre_del_archivo_salida.bmp> <cantidad de grados a rotar>" << endl;
        return 1;
    }
    const char* nombreArchivoLecturaBMP = argv[1];
    const char* nombreArchivoEscrituraBMP = argv[2];
    int gradosRotar = atoi(argv[3]);


    vector<vector<Pixel>> imagen = leerArchivoBMP(nombreArchivoLecturaBMP);

    int lado = max(imagen.size(), imagen[0].size());
    vector<vector<Pixel>> imagenRotada(lado, vector<Pixel>(lado));

    auto inicio_rotar = chrono::high_resolution_clock::now();
    rotarImagen(imagen, imagenRotada, gradosRotar, 0, imagen.size());
    auto final_rotar = chrono::high_resolution_clock::now();
    auto duracion_rotar = chrono::duration_cast<chrono::microseconds>(final_rotar - inicio_rotar);
    cout << "Tiempo de ejecucion secuencial: " << duracion_rotar.count() << " micosegundos" << endl;


    vector<vector<Pixel>> imagenRotadaHilos(lado, vector<Pixel>(lado));

    auto inicio_rotar_hilos = chrono::high_resolution_clock::now();
    rotarImagenHilos(imagen, imagenRotadaHilos, gradosRotar);
    auto final_rotar_hilos = chrono::high_resolution_clock::now();
    auto duracion_rotar_hilos = chrono::duration_cast<chrono::microseconds>(final_rotar_hilos - inicio_rotar_hilos);
    cout << "Tiempo de ejecucion hilos: " << duracion_rotar_hilos.count() << " micosegundos" << endl;


    //auto inicio_rotar_paralelo = chrono::high_resolution_clock::now();
    //vector<vector<Pixel>> imagenRotadaOMP = rotarImagenOMP(imagen, gradosRotar);
    //auto final_rotar_paralelo = chrono::high_resolution_clock::now();
    //auto duracion_rotar_paralelo = chrono::duration_cast<chrono::microseconds>(final_rotar_paralelo - inicio_rotar_paralelo);
    //cout << "Tiempo de ejecucion OMP: " << duracion_rotar_paralelo.count() << " micosegundos" << endl << endl;

    guardarMatrizEnBMP(nombreArchivoEscrituraBMP, imagenRotadaHilos);


    cout << "hilos superó a secuancial por: " << duracion_rotar.count() - duracion_rotar_hilos.count() << " microsegundos" << endl;
    

    return 0;
}