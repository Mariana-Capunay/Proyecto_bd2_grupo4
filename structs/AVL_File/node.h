#ifndef NODE_H
#define NODE_H
#include <iostream>
using namespace std;

template <typename T>
struct NodeAVL  {
    T value; //valor que guarda

    //punteros a hijos (apuntan a heap_file)
    long left; //puntero a hijo izquierdo (en heap_file)
    long right; //puntero a hijo derecho (en heap_file)

    //puntero a archivo file_name
    long pointer_value; //puntero de valor actual (en archivo filename)

    int height; //altura del nodo (para el avl)

    long next; //nodo siguiente (para gestionar repetidos)

    NodeAVL();
    NodeAVL(long puntero, T key);
    int size();
    void setValue(long puntero, T key);
    void getValue();
    ~NodeAVL();
};

template <typename T>
NodeAVL<T>::NodeAVL(){
    left = right = next = -1;
    height = 0; pointer_value = -1;
}

template <typename T> 
NodeAVL<T>::NodeAVL(long puntero, T key){
    left = right = next = -1;
    height = 0;
    value = key;
    pointer_value = puntero;
}

template <typename T>
NodeAVL<T>::~NodeAVL(){}

template <typename T>
void NodeAVL<T>::setValue(long puntero, T key){
    pointer_value = puntero;
    value = key;
}

template <typename T>
int NodeAVL<T>::size(){
    return sizeof(int) + 4*sizeof(long) + sizeof(T);
}

template <typename T>
void NodeAVL<T>::getValue(){
    cout<<"Value:  "<<this->value<<" | ";
    cout<<"Left:  "<<this->left<<" | ";
    cout<<"Right:  "<<this->right<<" | ";
    cout<<"Height:  "<<this->height<<" | ";
    cout<<"Next:  "<<this->next<<"\n";
}
#endif
