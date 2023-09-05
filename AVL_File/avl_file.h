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
    fstream file; //para manejar heap_file sin abrir y cerrar tantas veces el archivo

    public:
    AVLFile();
    AVLFile(string file_name, string atributo);
    vector<long> search(T key); //devuelve posiciones de key en filename
    vector<long> rangeSearch(T begin_key, T end_key); //devuelve posiciones de key en filename
    bool insert(T key);  //inserta key en el AVL y guarda nodo en heap_file
    bool remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)

    ~AVLFile();

    private: //funciones recursivas
    bool remove(long pos, T key){ //recibe posicion y key a eliminar, tambien fstream file tiene heap_file abierto (tenemos info de los nodos)
        if (pos==-1) return false; //no se puede eliminar
        
        //se puede eliminar
        file.seekg(pos, ios::beg); //se posiciona para leer el nodo
        NodeAVL<T> nodo;
        file.read((char*)&nodo, nodo.size()); //se lee info del nodo

        //se compara key con el value de los nodos hijos 
        if (key<nodo.value){
            this->remove(nodo.left, key);
        } else if (key>nodo.value){
            this->remove(nodo.right, key);
        } else{ //se llegó al valor del nodo  //se debe eliminar nodo y sus next
        
            if (nodo.left==-1 && nodo.right==-1){ //caso en el que es hoja

            } else if (nodo.left==-1){ //caso en el que tiene hijo derecho

            } else { //caso en el que tiene hijo izquierdo

            }
        }
    }

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
    file.open(heap_file,ios::binary|ios::in|ios::out);
    return this->remove(root, key);
    file.close();
}

template <typename T>
AVLFile<T>::~AVLFile(){}

#endif