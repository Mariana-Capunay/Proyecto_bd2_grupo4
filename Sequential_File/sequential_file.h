#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
using namespace std;

template <typename T>
struct Record { // Estructura de un registro
        T data;
        long next_del;

        Record() : next_del(-1) {}//el next_del se inicializa en -1
    };

template <typename T>
struct Header {// Estructura del encabezado
        int HEADER;
    };


template <typename T>
class SequentialFile {
private:
    string file_name; //data set en binario
    string heap_file_name; // para guardar insercciones
    string reorg_file_name;//archivo con llave ordenadas
    fstream file;

public:
    SequentialFile();

    SequentialFile(string file_name, string heap_file_name, string reorg_file_name);

    void insert(const T& data);

    vector<T> search(const T& key);

    vector<T> rangeSearch(const T& begin_key, const T& end_key);

    bool add(const T& data);

    bool remove(const T& key);

    void reorganize();

    vector<T> load();


    private: 


};

template <typename T>
SequentialFile<T>::SequentialFile() {}// Constructor por defecto

template <typename T>
SequentialFile<T>::SequentialFile(string file_name, string heap_file_name, string reorg_file_name) {// Constructor por asignacion
    this->file_name = file_name;
    this->heap_file_name = heap_file_name;
    this->reorg_file_name = reorg_file_name;
}

    /*
Inserciones enlazadas:
Localizar la posición en donde será insertado el nuevo registro.
Si el espacio está libre, insertar.
Sino, insertar el registro en un espacio auxiliar (heap file)
En este caso, los punteros deberían ser actualizados.
Se requiere reorganizar el archivo original cada cierto tiempo mezclando ordenadamente con el espacio auxiliar. 

    */

template <typename T>
void SequentialFile<T>::insert(const T& data) {
    file.open(file_name,ios::in|ios::out|ios::binary);
    if(!file.is_open()){
        throw runtime_error("No se pudo abrir el archivo");
    }

    

}



#endif