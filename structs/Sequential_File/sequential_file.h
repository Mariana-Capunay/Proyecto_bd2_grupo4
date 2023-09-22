#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


template <typename T>
struct Node{
    T value; //valor de la key guardada
    long pointer_value; //posicion de este nodo
    long next; //posicion del nodo siguiente (si no hay, -1)

    Node() {
        this->next = -1;
        this->pointer_value = -1;
    }

    Node(long puntero, T key) {
        this->next = -1;
        this->value = key;
        this->pointer_value = puntero;
    }

    void setValue(long puntero, T key) {
        this->pointer_value = puntero;
        this->value = key;
    }

    int size() {
        return 2*sizeof(long) + sizeof(T);
    }
};


class SequentialFile {
private:
    // Punteros
    long root; //posicion en archivo file del primer elemento, -1 si no hay
    long del; //posicion en archivos heap o reorg del primer eliminado, 0 si no hay
    int kHeapLimit;

    string file_name; //data set en binario
    string reorg_file_name;//archivo con llave ordenadas
    string heap_file_name; // para guardar insercciones

    long nro_registros() {
        fstream file(this->file_name, ios::in | ios::out | ios::binary);
        if (!file.is_open()) {throw ("Error al abrir el archivo");}
        file.seekg(0, ios::end); //nos posicionamos al final del archivo
        Record r;
        long nRegs = file.tellg() / r.size(); //cantidad de registros
        file.close();

        return nRegs;
    }

public:
    SequentialFile () {
        this->root = -1;
        this->del = 0;
    }

    SequentialFile (string file_name, string reorg_file_name, string heap_file_name) {
        this->file_name = file_name;
        this->reorg_file_name = reorg_file_name;
        this->heap_file_name = heap_file_name;
        this->root = -1;
        this->del = 0;

        long nRegs = this->nro_registros();
        if (nRegs > 0) {
            this->kHeapLimit = trunc(log2(nRegs));
        }

    }

    bool insert( T data);

    vector<T> search(const T& key);

    vector<T> rangeSearch(const T& begin_key, const T& end_key);

    bool add(const T& data);

    bool remove(const T& key);

    void reorganize();

    vector<T> load();


};

template <typename T>
bool SequentialFile<T>::insert(T data) {
        int pos=nro_registros()-1;

        //abrimos el archivo file_name, donde se guardan los registros
        file.open(file_name, ios::in | ios::out | ios::binary);
        if (!file.is_open()) {throw ("Error al abrir el archivo");}

        //abrimos el archivo heap_file_name, donde se guardan las insercciones fuera de la cantidqd de registros
        fstream heap_file(heap_file_name, ios::in | ios::out | ios::binary);
        if (!heap_file.is_open()) {throw ("Error al abrir el archivo");}

        //leemos el registro al cual queremos insertar
            Nodo<T> newrecord;
            newrecord = new Nodo<T>(data);
            Nodo<T> *aux = new Nodo<T>(data);

        //verificamos si hay espacio en el archivo file_name
        if (pos < 0) {
            //si no hay espacio, insertamos en el archivo heap_file_name
            heap_file.seekg(0, ios::end);
            heap_file.write((char *)&newrecord, sizeof(T));
            heap_file.close();
            return true;
        } else {
            //si hay espacio, insertamos en el archivo file_name
            file.seekg(pos * sizeof(T), ios::beg);
            file.write((char *)&newrecord, sizeof(T));
            file.close();
            return true;
        }




        


}


#endif