#ifndef AVLFILE_H
#define AVLFILE_H
#include "node.h"

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
    bool insert(T key);  //inserta key en el AVL y guarda nodo en heap_file
    bool remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)


};
#endif