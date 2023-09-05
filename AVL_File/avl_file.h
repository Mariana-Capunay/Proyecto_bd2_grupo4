#ifndef AVLFILE_H
#define AVLFILE_H
#include "node.h"
#include <vector>
template <typename T>
class AVLFile
{
    long root;

    //archivos
    string filename; //data con todos los registros
    string heap_file; //archivo en el que se guardan los nodos 

    public:
    AVLFile();
    AVLFile(string file_name, string atributo);
    vector<long> search(T key); //devuelve posiciones de key en filename
    vector<long> rangeSearch(T begin_key, T end_key); //devuelve posiciones de key en filename
    bool insert(T key);  //inserta key en el AVL y guarda nodo en heap_file
    bool remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)

    ~AVLFile();

    private: //funciones recursivas
    
};

template <typename T>
AVLFile<T>::AVLFile(){
    root = -1;
}

template <typename T>
AVLFile<T>::AVLFile(string file_name, string atributo){
    this->filename = file_name;
    this->heap_file = "avl."+atributo+".bin";
}

template <typename T>
vector <long> AVLFile<T>::search(T key){

}

template <typename T>
vector <long> AVLFile<T>::rangeSearch(T begin_key, T end_key){

}

template <typename T>
bool insert(T key){

}

template <typename T>
bool remove(T key){

}

template <typename T>
AVLFile<T>::~AVLFile(){}

#endif