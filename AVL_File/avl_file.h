#ifndef AVLFILE_H
#define AVLFILE_H
#include "node.h"
#include <vector>
#include <fstream>

template <typename T>

class AVLFile{
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

    bool insert(T key) {  //inserta key en el AVL y guarda nodo en heap_file
        int pos = nro_registros() - 1;
        NodeAVL<T> nodo = NodeAVL<T>(pos, key);
        return insert(root, nodo);
    }

    int remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)
    void delete_equals(long pos);
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
            
            delete_equals(nodo.pointer_value); //se elimina next's del nodo

            if (nodo.left==-1 && nodo.right==-1){ //caso en el que es hoja
                return pos; //se elimina nodo en la posicion actual y ya no se necesita referencia a ese valor

            } else if (nodo.left==-1){ //caso en el que tiene hijo derecho
                return nodo.right; //retorna "nodo.right" para que no se elimine esta referencia al nodo

            } else if (nodo.right==-1){ //caso en el que tiene hijo izquierdo
                return nodo.left; //retorna "nodo.left" para que no se elimine esta referencia al nodo

            } else{ // nodo existe y tiene dos hijos
                //se debe reemplazar por su sucesor (retornar valor del sucesor - actualizar altura)
            } 
        }
        
        //balancear
        //retornar
        return
    }

    bool insert(long& pos_node, NodeAVL<T>& node){
        if(pos_node == -1){
            file.seekg(0, ios::end);
            pos_node = file.tellg();
            file.seekp(pos_node, ios::beg);
            file.write((char*)&node, node.size());
            return true;

        } else{
            file.seekg(pos_node, ios::beg);
            NodeAVL<T> parent;
            file.read((char*)&parent, parent.size());
            if (node.value < parent.value){
                node.height++;
                insert(parent.left, node);
                file.seekp(pos_node, ios::beg);
                parent.left = pos_node;
                file.write((char*)&parent, parent.size());

            } else if(node.value > parent.value){
                node.height++;
                insert(parent.right, node);
                file.seekp(pos_node, ios::beg);
                parent.right = pos_node;
                file.write((char*)&parent, parent.size());

            } else{
                insert(parent.next, node);
                file.seekp(pos_node, ios::beg);
                parent.next = pos_node;
                file.write((char*)&parent, parent.size());
            }
            // Autobalanceo
        }
    }

    int nro_registros(){
        ifstream file(this->file_name, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        file.seekg(0, ios::end);
        long total_bytes = file.tellg();
        file.close();
        return total_bytes / sizeof(reg);   // Registro en data.dat
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
    file.open(heap_file,ios::binary); // Abrir el archivo en binario
    vector <long> pointerValueList = {}; // Vector a retornar (vacio al comienzo)
    bool found = false; // Flag para determinar si se encontró o no el key
    NodeAVL<T> nodo; // Nodo auxiliar para lectura de nodos

    // Inicializar pos en el primer nodo (root)
    long pos = this->root;

    // Mientras la posicion del nodo exista (!= -1)
    while (pos != -1) {
        // Leer el nodo desde la posicion actual (pos)
        file.seekg(pos, ios::beg);
        file.read((char*)&nodo, nodo.size());

        if (nodo.value == key) {
            found = true; // Actualizo found y salgo
            break;
        } else if (nodo.value > key) {
            pos = nodo.left; //Actualizar pos a hijo izquierdo
        } else {
            pos = nodo.right; //Actualizar pos a hijo derecho
        }
    }

    // Validación de key encontrado
    if (found) {
        pointerValueList.pushback(nodo.pointer_value); // Guarda el pos del nodo en el vector
        pos = nodo.next; // Coloca next en pos para ver si tiene repetidos

        // Mientras exista nodo next (repetidos)
        while (pos != -1) {
            // Leer el nodo
            file.seekg(pos, ios::beg);
            file.read((char*)&nodo, nodo.size());

            pointerValueList.pushback(nodo.pointer_value); // Guarda el pos del nodo en el vector a retornar

            pos = nodo.next; // Actualizo pos para ver el siguiente next
        }
    } else {
        throw runtime_error("No se encontró");
    }

    file.close();

    return pointerValueList;
}

template <typename T>
vector <long> AVLFile<T>::rangeSearch(T begin_key, T end_key){

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
void AVLFile<T>::delete_equals(long pos){
    file(this->heap_file, ios::in|ios::out); //para leer y escribir
    
    bool iterar = true;

    while (iterar){ 
        file.seekg(pos); //vamos a posicion del registro
        NodeAVL<T> nodo;
        file.read((char*)&nodo, nodo.size()); //leemos nodo        

        if (nodo.next==-1){ //si no tiene siguiente
            iterar = false;
        } else{ //tiene siguiente
            file.seekp(pos); //nos posicionamos para escribir
            pos = nodo.next; //guardamos posicion 
            nodo.next = -1;
            file.write((char*)nodo, nodo.size()); //escribimos "nodo" con next=-1
        } //iteramos hasta llegar a nodo con next=-1
    }

    file.close();
}

template <typename T>
AVLFile<T>::~AVLFile(){}

#endif