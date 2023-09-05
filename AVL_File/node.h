#ifndef NODE_H
#define NODE_H
#include <iostream>
using namespace std;

template <typename T>
struct NodeAVL
{
    T value; //valor que guarda

    //punteros a hijos (apuntan a heap_file)
    long left; //puntero a hijo izquierdo (en heap_file)
    long right; //puntero a hijo derecho (en heap_file)

    //puntero a archivo file_name
    long pointer_value; //puntero de valor actual (en archivo filename)

    int height; //altura del nodo (para el avl)

    int next; //nodo siguiente (para gestionar repetidos)
};

#endif
