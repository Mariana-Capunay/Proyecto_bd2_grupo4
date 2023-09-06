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
    int remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)

    ~AVLFile();

    private: //funciones recursivas
    int remove(long& pos, T key){  //retorna posicion en la que se elimina
        //recibe posicion y key a eliminar, tambien fstream file tiene heap_file abierto (tenemos info de los nodos)
        if (pos==-1) return -1; //no se puede eliminar
        
        //se puede eliminar
        file.seekg(pos, ios::beg); //se posiciona para leer el nodo
        NodeAVL<T> nodo;
        file.read((char*)&nodo, nodo.size()); //se lee info del nodo

        long pos_delete;

        //se compara key con el value de los nodos hijos 
        if (key<nodo.value){
            pos_delete = this->remove(nodo.left, key);
            if (pos_delete==node.left){ //si se elimina en posicion del hijo izquierdo
                nodo.left = -1; //se descuelga del arbol
                file.seekp(pos,ios::beg); //se posiciona para escribir
                file.write((char*)& nodo, nodo.size()); //escribe
            } else{
                nodo.left = pos_delete;
            }

        } else if (key>nodo.value){
            pos_delete = this->remove(nodo.right, key);
            if (pos_delete==node.right){ //si se elimina en posicion del hijo derecho
                nodo.right = -1; //se descuelga del arbol
                file.seekp(pos,ios::beg); //se posiciona para escribir
                file.write((char*)& nodo, nodo.size()); //escribe
            } else{ //retorna nueva posicion a la que se debe apuntar
                nodo.right = pos_delete;
            }

        } else{ //se llegó al valor del nodo  //se debe eliminar nodo y sus next
        
            if (nodo.left==-1 && nodo.right==-1){ //caso en el que es hoja
                return pos; //se elimina nodo en la posicion actual y ya no se necesita referencia a ese valor

            } else if (nodo.left==-1){ //caso en el que tiene hijo derecho
                return nodo.right; //retorna "nodo.right" para que no se elimine esta referencia al nodo

            } else if (nodo.right==-1){ //caso en el que tiene hijo izquierdo
                return nodo.left; //retorna "nodo.left" para que no se elimine esta referencia al nodo

            } else{ // nodo existe y tiene dos hijos
                //se debe reemplazar por su sucesor (retornar valor del sucesor)
            } 

            //actualizar altura del nodo
            //balancear
            //retornar
            return 
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
    
    int result = this->remove(root, key);
    file.close();
    if (result!=-1) return true;
    return false;
}

template <typename T>
AVLFile<T>::~AVLFile(){}

#endif