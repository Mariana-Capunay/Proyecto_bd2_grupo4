#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


template <typename T>
struct Nodo{
    private:
        Nodo<T> *next;
        T record;
    public:
        Nodo(T record){
            this->record = record;
            this->next = nullptr;
        }

        Nodo<T>* getNext(){
            return this->next;
        }
        void setNext(Nodo<T> *next){
            this->next = next;
        }
};

template <typename T>
class Header{
    private:
        int data_Del;
    public:
        Header(){
            this->data_Del = -1;
        }
        int getData_Del(){
            return this->data_Del;
        }
        void setData_Del(int data_Del){
            this->data_Del = data_Del;
        }

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

    bool insert( T data);

    vector<T> search(const T& key);

    vector<T> rangeSearch(const T& begin_key, const T& end_key);

    bool add(const T& data);

    bool remove(const T& key);

    void reorganize();

    vector<T> load();


    private: 
    void nro_registros(){
        int nro_registros=0;
        file.open(file_name, ios::in | ios::out | ios::binary);
        if (!file.is_open()) {throw ("Error al abrir el archivo");}
        file.seekg(0, ios::end);//nos posicionamos al final del archivo
        nro_registros = file.tellg()/sizeof(T); //cantidad de registros
        file.close();
        return nro_registros;
    }


};

template <typename T>
SequentialFile<T>::SequentialFile() {}// Constructor por defecto

template <typename T>
SequentialFile<T>::SequentialFile(string file_name, string heap_file_name, string reorg_file_name) {// Constructor por asignacion
    this->file_name = file_name;
    this->heap_file_name = heap_file_name;
    this->reorg_file_name = reorg_file_name;
}


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