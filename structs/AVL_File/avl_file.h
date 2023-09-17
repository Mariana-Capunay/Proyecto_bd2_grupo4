#ifndef AVLFILE_H
#define AVLFILE_H
#include "node.h"
#include <vector>
#include <fstream>
#include "../../dataset_bin/record.h" //importando record
template <typename T>

class AVLFile{
    long root;

    //archivos
    string filename; //data con todos los registros
    string heap_file; //archivo en el que se guardan los nodos  (value, left, hight, pointer_value, next, height)
    fstream file; //para manejar heap_file sin abrir y cerrar tantas veces el archivo

    public:
    AVLFile();
    AVLFile(string file_name, string atributo, int atributo_col);
    void buildFromFile(int atributo_col);
    vector<long> search(T key); //devuelve posiciones de key en filename
    vector<long> rangeSearch(T begin_key, T end_key); //devuelve posiciones de key en filename

    bool insert(T key) {  //inserta key en el AVL y guarda nodo en heap_file
        int pos = nro_registros() - 1; 
        NodeAVL<T> nodo = NodeAVL<T>(pos, key);
        return insert(root, nodo);
    }

    bool insert(NodeAVL<T> nodo){
        return insert(root,nodo);
    }

    bool remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)
    void balance(long pos, NodeAVL<T> &node); //verifica si una rotación es necesaria
    void left_Rotation(long pos, NodeAVL<T> &node); //rotación a la izquierda
    void right_Rotation(long pos, NodeAVL<T> &node); //rotación a la derecha
    void delete_equals(long pos);
    //bool remove(T key);
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
            if (pos_delete==nodo.left){ //si se elimina en posicion del hijo izquierdo
                nodo.left = -1; //se descuelga del arbol

                file.seekp(pos,ios::beg); //se posiciona para escribir
                file.write((char*)& nodo, nodo.size()); //escribe
            } else{
                nodo.left = pos_delete;
            }

        } else if (key>nodo.value){
            pos_delete = this->remove(nodo.right, key);
            if (pos_delete==nodo.right){ //si se elimina en posicion del hijo derecho
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
        return 0;
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
                //parent.left = pos_node;
                file.write((char*)&parent, parent.size());

            } else if(node.value > parent.value){
                node.height++;
                insert(parent.right, node);
                file.seekp(pos_node, ios::beg);
                //parent.right = pos_node;
                file.write((char*)&parent, parent.size());

            } else{
                insert(parent.next, node);
                file.seekp(pos_node, ios::beg);
                //parent.next = pos_node;
                file.write((char*)&parent, parent.size());
            }
            file.seekg(0, ios::beg);
            NodeAVL<T> first;
            file.read((char*)&first, first.size());
            balance(root, first);
        }
    }

    int nro_registros(){
        ifstream file(this->file_name, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        file.seekg(0, ios::end);
        long total_bytes = file.tellg();
        file.close();
        return total_bytes / sizeof(Record);   // Registro en data.dat
    }

    long balanceFactor(NodeAVL<T> &node){//balance factor
        long FacB=0;
        NodeAVL<T> hijoIzq;
        NodeAVL<T> hijoDer;
        if(node.left!=-1){ //si tiene hijo izquierdo
            file.seekg(node.left,ios::beg);//se posiciona en el hijo izquierdo
            file.read((char*)&hijoIzq,hijoIzq.size());//se lee el hijo izquierdo
            FacB+=hijoIzq.height; //se suma la altura del hijo izquierdo
        }
        if(node.right!=-1){ //si tiene hijo derecho
            file.seekg(node.right,ios::beg);//se posiciona en el hijo derecho
            file.read((char*)&hijoDer,hijoDer.size());//se lee el hijo derecho
            FacB-=hijoDer.height; //se resta la altura del hijo derecho
        }
        return FacB;
    }

    long altura(long node){//retorna la altura del nodo
        if(node==-1) return -1;
        file.seekg(node,ios::beg);//se posiciona en el nodo
        NodeAVL<T> nodo;
        file.read((char*)&nodo,nodo.size());//se lee el nodo
        return nodo.height; //retorna la altura del nodo
    
    }
    
    void alturaActulizada(long pos,NodeAVL<T> &node){//actualiza la altura del nodo
        long alturaIzq=altura(node.left); //altura del hijo izquierdo
        long alturaDer=altura(node.right); //altura del hijo derecho

        //recalculamos la latura del nodo
        if(alturaIzq>alturaDer) node.height=alturaIzq+1;
        else node.height=alturaDer+1;

        //escribimos el nodo
        file.seekp(pos,ios::beg);//se posiciona en el nodo
        file.write((char*)&node,node.size());//se escribe el nodo
    }

};



template <typename T>
void AVLFile<T>::balance(long pos, NodeAVL<T> &node){//verifica si una rotación es necesaria
    //factor de balanceo
        long FacB=balanceFactor(node); //balanceFactor es la diferencia de alturas entre el hijo izquierdo y el derecho
        if(FacB>1){ //si el factor de balanceo es mayor a 1, se debe rotar a la derecha
            NodeAVL<T> hijoIzq; //nodo auxiliar para el hijo izquierdo
            file.seekg(node.left,ios::beg);//se posiciona en el hijo izquierdo
            file.read((char*)&hijoIzq,hijoIzq.size());//se lee el hijo izquierdo

            if(balanceFactor(hijoIzq)<=-1){ //si el factor de balanceo del hijo izquierdo es menor o igual a -1, se debe hacer una rotación doble a la derecha
                left_Rotation(pos,node);
            }
            right_Rotation(pos,node);
        }

        else if(FacB<=-2){ //si el factor de balanceo es menor o igual a -2, se debe rotar a la izquierda
            NodeAVL<T> hijoDer; //nodo auxiliar para el hijo derecho
            file.seekg(node.right,ios::beg);//se posiciona en el hijo derecho
            file.read((char*)&hijoDer,hijoDer.size());//se lee el hijo derecho

            if(balanceFactor(hijoDer)>=1){ //si el factor de balanceo del hijo derecho es mayor o igual a 1, se debe hacer una rotación doble a la izquierda
                right_Rotation(pos,node);
            }
            left_Rotation(pos,node); 
        }
}

template <typename T>
void AVLFile<T>::left_Rotation(long pos, NodeAVL<T> &node){//rotación a la izquierda
    NodeAVL<T> hijoDer; //nodo auxiliar para el hijo derecho
    file.seekg(node.right,ios::beg);//se posiciona en el hijo derecho
    file.read((char*)&hijoDer,hijoDer.size());//se lee el hijo derecho

    NodeAVL<T> hijoIzq; //nodo auxiliar para el hijo izquierdo del hijo derecho
    file.seekg(hijoDer.left,ios::beg);//se posiciona en el hijo izquierdo del hijo derecho
    file.read((char*)&hijoIzq,hijoIzq.size());//se lee el hijo izquierdo del hijo derecho

    node.right=hijoIzq.left; //el hijo derecho del nodo actual es el hijo izquierdo del hijo derecho
    hijoDer.left=pos; //el hijo izquierdo del hijo derecho es el nodo actual
    file.seekp(node.right,ios::beg);//se posiciona en el hijo derecho del nodo actual
    file.write((char*)&node,node.size());//se escribe el nodo actual
    file.seekp(hijoDer.left,ios::beg);//se posiciona en el hijo izquierdo del hijo derecho
    file.write((char*)&hijoDer,hijoDer.size());//se escribe el hijo derecho

    //actualizar altura
    alturaActulizada(pos,node); //actualiza la altura del nodo actual
    alturaActulizada(hijoDer.left,hijoDer); //actualiza la altura del hijo derecho

    
}

template <typename T>
void AVLFile<T>::right_Rotation(long pos, NodeAVL<T> &node){
    NodeAVL<T> hijoIzq; //nodo auxiliar para el hijo izquierdo
    file.seekg(node.left,ios::beg);//se posiciona en el hijo izquierdo
    file.read((char*)&hijoIzq,hijoIzq.size());//se lee el hijo izquierdo
    
    NodeAVL<T> hijoDer; //nodo auxiliar para el hijo derecho del hijo izquierdo
    file.seekg(hijoIzq.right,ios::beg);//se posiciona en el hijo derecho del hijo izquierdo
    file.read((char*)&hijoDer,hijoDer.size());//se lee el hijo derecho del hijo izquierdo

    node.left=hijoDer.right; //el hijo izquierdo del nodo actual es el hijo derecho del hijo izquierdo
    hijoIzq.right=pos; //el hijo derecho del hijo izquierdo es el nodo actual
    file.seekp(node.left,ios::beg);//se posiciona en el hijo izquierdo del nodo actual
    file.write((char*)&node,node.size());//se escribe el nodo actual
    
    file.seekp(hijoIzq.right,ios::beg);//se posiciona en el hijo derecho del hijo izquierdo
    file.write((char*)&hijoIzq,hijoIzq.size());//se escribe el hijo izquierdo

    //actualizar altura
    alturaActulizada(pos,node); //actualiza la altura del nodo actual
    alturaActulizada(hijoIzq.right,hijoIzq); //actualiza la altura del hijo izquierdo
}


template <typename T>
AVLFile<T>::AVLFile(){
    root = -1;
}

template <typename T>
AVLFile<T>::AVLFile(string file_name, string atributo, int atributo_col){
    this->filename = file_name;
    this->heap_file = "avl."+atributo+".bin";

    buildFromFile(atributo_col); // se genera AVL en base a una columna especifica
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
        pointerValueList.push_back(nodo.pointer_value); // Guarda el pos del nodo en el vector
        pos = nodo.next; // Coloca next en pos para ver si tiene repetidos

        // Mientras exista nodo next (repetidos)
        while (pos != -1) {
            // Leer el nodo
            file.seekg(pos, ios::beg);
            file.read((char*)&nodo, nodo.size());

            pointerValueList.push_back(nodo.pointer_value); // Guarda el pos del nodo en el vector a retornar

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
    vector <long> result;

    return result;
}

template <typename T>
bool AVLFile<T>::remove(T key){
    file(heap_file,ios::binary|ios::in|ios::out);
    
    int result = this->remove(root, key);
    file.close();
    if (result!=-1) return true;
    return false;
}

template <typename T>
void AVLFile<T>::delete_equals(long pos){
    file(this->heap_file, ios::binary|ios::out|ios::in); //para leer y escribir
    
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
void AVLFile<T>::buildFromFile(int atributo_col){
    ifstream data(this->filename, ios::binary);
    if (!data.is_open()) throw runtime_error("No existe archivo",filename);
    
    long bytes = 0;
    //cargamos los datos uno a uno  //cargamos cada registro del file_name como un nodo
    while (data.tellg()<bytes){
        Record record;
        data.read((char*)&record, sizeof(Record)); // leemos record actual

        NodeAVL<T> nodo;
        if (atributo_col == 2) nodo.setValue(bytes, record.atrib2); // creamos el nodo
        else nodo.setValue(bytes, record.atrib4);
        this->insert(nodo);
        bytes+= sizeof(Record); // se aumenta contador de bytes
    }

    data.close();
}

template <typename T>
AVLFile<T>::~AVLFile(){}

#endif