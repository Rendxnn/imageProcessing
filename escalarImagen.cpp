#include "manejoArchivos.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <thread>

using namespace std;


vector<int> calcularPosicion(vector<int> coordenadas, int escalamiento) {

    vector<vector<int>> matrizTransformacion = {{escalamiento, 0}, {0, escalamiento}};
    vector<int> nuevasCoordenadas = {0, 0};
    
    nuevasCoordenadas[0] = static_cast<int>(round(coordenadas[0] * matrizTransformacion[0][0] + coordenadas[1] * matrizTransformacion[0][1]));
    nuevasCoordenadas[1] = static_cast<int>(round(coordenadas[0] * matrizTransformacion[1][0] + coordenadas[1] * matrizTransformacion[1][1]));

    return nuevasCoordenadas;
}


void escalarImagen(vector<vector<Pixel>>& imagen, vector<vector<Pixel>>& imagenEscalada, int escalamiento, int filaInicial, int filaFinal) {
    int lado = max(imagenEscalada.size(), imagenEscalada[0].size());
    int largo = imagenEscalada.size();
    int ancho = imagenEscalada[0].size();

    int iNuevo = 0;
    int jNuevo = 0;

    for (int i = filaInicial; i < filaFinal; i++) {
        for (int j = 0; j < imagen[i].size(); j++) {
            vector<int> coordenadas = {i, j};
            vector<int> nuevasCoordenadas = calcularPosicion(coordenadas, escalamiento);

            iNuevo = nuevasCoordenadas[0];
            jNuevo = nuevasCoordenadas[1];

            
            if (iNuevo >= 0 && iNuevo <= lado && jNuevo >= 0 && jNuevo < lado) {
            // Asigna directamente los valores de los píxeles
            imagenEscalada[iNuevo][jNuevo].red = imagen[i][j].red;
            imagenEscalada[iNuevo][jNuevo].green = imagen[i][j].green;
            imagenEscalada[iNuevo][jNuevo].blue = imagen[i][j].blue;
            }

            for (int k = 0; k < escalamiento; k++) {
                for (int h = 0; h < escalamiento; h++) {
                    int nuevaFila = iNuevo + k;
                    int nuevaColumna = jNuevo + h;
                    if (nuevaFila >= 0 && nuevaFila < largo && nuevaColumna >= 0 && nuevaColumna < ancho) {
                        imagenEscalada[nuevaFila][nuevaColumna].red = imagen[i][j].red;
                        imagenEscalada[nuevaFila][nuevaColumna].green = imagen[i][j].green;
                        imagenEscalada[nuevaFila][nuevaColumna].blue = imagen[i][j].blue;
                    }
                }
            }

            
        }
    }
    return;
}


void escalarImagenHilos(vector<vector<Pixel>>& imagen, vector<vector<Pixel>>& nuevaImagen, int escalamiento) {
    int numHilos = omp_get_max_threads();
    vector<thread> hilos;
    int filasPorHilo = imagen.size() / numHilos;
    for (int i = 0; i < numHilos; i++) {
        int inicioFila = i * filasPorHilo;
        int finFila = (i == numHilos - 1) ? imagen.size() : (i + 1) * filasPorHilo;
        hilos.emplace_back([&imagen, &nuevaImagen, inicioFila, finFila, escalamiento]() {
            escalarImagen(imagen, nuevaImagen, escalamiento, inicioFila, finFila);
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
    int escalamiento = atoi(argv[3]);


    vector<vector<Pixel>> imagen = leerArchivoBMP(nombreArchivoLecturaBMP);

    int lado = max(imagen.size(), imagen[0].size());
    vector<vector<Pixel>> imagenEscalada(imagen.size() * escalamiento, vector<Pixel>(imagen[0].size() * escalamiento));

    auto inicio_escalar = chrono::high_resolution_clock::now();
    escalarImagen(imagen, imagenEscalada, escalamiento, 0, imagen.size());
    auto final_escalar = chrono::high_resolution_clock::now();
    auto duracion_escalar = chrono::duration_cast<chrono::microseconds>(final_escalar - inicio_escalar);
    cout << "Tiempo de ejecucion secuencial: " << duracion_escalar.count() << " micosegundos" << endl;


    vector<vector<Pixel>> imagenEscaladaHilos(imagen.size() * escalamiento, vector<Pixel>(imagen[0].size() * escalamiento));

    auto inicio_escalar_hilos = chrono::high_resolution_clock::now();
    escalarImagenHilos(imagen, imagenEscaladaHilos, escalamiento);
    auto final_escalar_hilos = chrono::high_resolution_clock::now();
    auto duracion_escalar_hilos = chrono::duration_cast<chrono::microseconds>(final_escalar_hilos - inicio_escalar_hilos);
    cout << "Tiempo de ejecucion hilos: " << duracion_escalar_hilos.count() << " micosegundos" << endl;


    //auto inicio_rotar_paralelo = chrono::high_resolution_clock::now();
    //vector<vector<Pixel>> imagenRotadaOMP = rotarImagenOMP(imagen, gradosRotar);
    //auto final_rotar_paralelo = chrono::high_resolution_clock::now();
    //auto duracion_rotar_paralelo = chrono::duration_cast<chrono::microseconds>(final_rotar_paralelo - inicio_rotar_paralelo);
    //cout << "Tiempo de ejecucion OMP: " << duracion_rotar_paralelo.count() << " micosegundos" << endl << endl;

    guardarMatrizEnBMP(nombreArchivoEscrituraBMP, imagenEscaladaHilos);


    cout << "hilos superó a secuencial por: " << duracion_escalar.count() - duracion_escalar_hilos.count() << " microsegundos" << endl;
    

    return 0;
}