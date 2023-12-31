#ifndef AVLFILE_H
#define AVLFILE_H

#include "node.h"
#include <vector>
#include <fstream>
#include "../../dataset_bin/binary_conversor.h" //importando record
//#include "../../dataset_bin/record.h" //importando record
//#include <windows.h>

using namespace std;

bool archivo_existe(const string& nombre);

void crear_archivo(const string& nombre);

template <typename T>
class AVLFile{
    long root;

    //archivos
    string filename; //data con todos los registros
    string heap_file; //archivo en el que se guardan los nodos  (value, left, hight, pointer_value, next, height)
    fstream file; //para manejar heap_file sin abrir y cerrar tantas veces el archivo
    int size = 0;
    void inicializar_root(){
        bool existe = archivo_existe(this->heap_file);
        crear_archivo(this->heap_file);
        if (!existe){
            root = -1;
        }
        else root = 0;
    }

    public:
    AVLFile();
    AVLFile(string file_name, string atributo);//, int atributo_col);
    void buildFromFile(string sourceName, int atributo_col);
    vector<long> search(T key); //devuelve posiciones de key en filename
    vector<long> rangeSearch(T begin_key, T end_key); //devuelve posiciones de key en filename

    bool insert(T key, long pos) {  //inserta key en el AVL y guarda nodo en heap_file
        cout << "Insertando elemento..." << endl;
        //int pos = nro_registros();

        //cout<<"posicion: "<<pos<<endl;
        file.open(heap_file, ios::binary|ios::in|ios::out);
        if (!file.is_open()) throw runtime_error("no se pudo abrir archivo en funcion insert(T key)");

        NodeAVL<T> nodo(pos,key);// = NodeAVL<T>(pos, key);

        bool result = insert(root, nodo);

        // Llamada al autobalanceo
        file.seekg(0, ios::beg);
        NodeAVL<T> first;
        file.read((char*)&first, first.size());
        //balance(root, first);

        //cout<<"root -> "<<root<<endl;
        printData();
        file.close();
        size++;
        return result;
    }

    void insert(NodeAVL<T>& nodo){
        file.open(heap_file, ios::binary|ios::in|ios::out);
        bool res = this->insert(root,nodo);
        if (res){
            size++;
            file.close();
        }
        file.close();
    }

    void printData(){ // method for debug
        cout<<"nro de registros: "<<this->size<<endl;
        
        file.open(this->heap_file, ios::binary|ios::in);
        
        int cont = 0;
        
        ifstream record_file(this->filename,ios::binary);
        while (file.peek() != EOF){//nodo.read(file)){
            NodeAVL<T> nodo;
            file.seekg(cont);
            file.read((char*)&nodo,nodo.size()); // obtiene nodo del heap_file

            //lee nodo del dataset en binario
            record_file.seekg(nodo.pointer_value);
            Record record;
            record.read(record_file);
            record.print();
            cont += nodo.size();
            while (nodo.next!=-1){
                file.seekg(nodo.next);
                file.read((char*)&nodo,nodo.size()); // obtiene nodo del heap_file
                record_file.seekg(nodo.pointer_value);
                Record record;
                record_file.read((char*)&record, record.size());
                record.print();
                cont+= nodo.size();
            }
            //nodo.getValue();
        }
        record_file.close();
        file.close();
    }

/*
    void deleteFiles(){
        const char* archivo_ansi = this->heap_file.c_str();
        if (DeleteFileA(archivo_ansi)) {
            std::cout << "El archivo se ha eliminado correctamente." << std::endl;
        } else {
            DWORD error = GetLastError();
            std::cerr << "No se pudo eliminar el archivo. Código de error: " << error << std::endl;
        }
    }
*/
    bool remove(T key);  //elimina key del avl, cambia removed = true en filename, cambia el valor del nodo en heap_file (evaluar casos en que es hoja o no)
    void balance(long pos, NodeAVL<T> &node); //verifica si una rotación es necesaria
    void left_Rotation(long pos, NodeAVL<T> &node); //rotación a la izquierda
    void right_Rotation(long pos, NodeAVL<T> &node); //rotación a la derecha
    void delete_equals(long pos);
    //bool remove(T key);
    ~AVLFile();


    private: //funciones recursivas
    int remove(long& pos, T key) {  //retorna posicion en la que se elimina
        //recibe posicion y key a eliminar, tambien fstream file tiene heap_file abierto (tenemos info de los nodos)
        if (pos == -1) return -1; //no se puede eliminar

        //se puede eliminar
        file.seekg(pos, ios::beg); //se posiciona para leer el nodo
        NodeAVL<T> nodo;
        file.read((char *) &nodo, nodo.size()); //se lee info del nodo

        long pos_delete;

        //se compara key con el value de los nodos hijos 
        if (key < nodo.value) {
            pos_delete = this->remove(nodo.left, key);
            if (pos_delete == nodo.left) { //si se elimina en posicion del hijo izquierdo
                balance(pos, nodo);
                nodo.left = -1; //se descuelga del arbol

                //file.seekp(pos,ios::beg); //se posiciona para escribir
                //file.write((char*)& nodo, nodo.size()); //escribe
            } else if (pos_delete == -2) {
                //no se debe modifica nodo
            } else {
                nodo.left = pos_delete;
            }

            file.seekp(pos, ios::beg); //se posiciona para escribir
            file.write((char *) &nodo, nodo.size()); //escribe
            return -2; //retorna posicion del padre actual
        } else if (key > nodo.value) {
            pos_delete = this->remove(nodo.right, key);
            if (pos_delete == nodo.right) { //si se elimina en posicion del hijo derecho
                balance(pos, nodo);
                nodo.right = -1; //se descuelga del arbol
            } else if (pos_delete == -2) {
                // no se debe modificar nodo
            } else { //retorna nueva posicion a la que se debe apuntar
                nodo.right = pos_delete;
            }

            file.seekp(pos, ios::beg); //se posiciona para escribir
            file.write((char *) &nodo, nodo.size()); //escribe
            return -2; //retorna posicion del padre actual

        } else { //se llegó al valor del nodo  //se debe eliminar nodo y sus next

            //delete_equals(nodo.pointer_value); //se elimina next's del nodo

            if (nodo.left == -1 && nodo.right == -1) { //caso en el que es hoja
                return pos; //se elimina nodo en la posicion actual y ya no se necesita referencia a ese valor

            } else if (nodo.left == -1) { //caso en el que tiene hijo derecho
                return nodo.right; //retorna "nodo.right" para que no se elimine esta referencia al nodo

            } else if (nodo.right == -1) { //caso en el que tiene hijo izquierdo
                return nodo.left; //retorna "nodo.left" para que no se elimine esta referencia al nodo

            } else { //caso en el que tiene ambos hijos
                //se busca el sucesor del nodo
                NodeAVL<T> sucesor = find_sucesor(nodo);
                //se elimina el sucesor, 
                this->remove(nodo.right, sucesor.value);
                balance(pos, nodo);
                //se cambia el valor del nodo por el valor del sucesor
                nodo.value = sucesor.value;
                //se cambia el pointer_value del nodo por el pointer_value del sucesor
                nodo.pointer_value = sucesor.pointer_value;
                //se cambia el next del nodo por el next del sucesor
                nodo.next = sucesor.next;
                //se cambia el next del sucesor por -1
                sucesor.next = -1;
                //se escribe el nodo
                file.seekp(pos, ios::beg); //se posiciona para escribir
                file.write((char *) &nodo, nodo.size()); //escribe
                //se escribe el sucesor
                file.seekp(nodo.right, ios::beg); //se posiciona para escribir
                file.write((char *) &sucesor, sucesor.size()); //escribe
                return -2; //retorna posicion del padre actual

            }

        }
//ELBALANCE VA EN LA LINEA 200
    }

    bool insert(long& pos_node, NodeAVL<T>& node){
        if(pos_node == -1){
            // Inserción
            file.seekg(0, ios::end);
            pos_node = file.tellg();                    // Obtener la posición de inserción
            file.seekp(pos_node, ios::beg);
            file.write((char*)&node, node.size());

            //prueba de longitud de archivo
            file.seekg(0,ios::end);
            return true;

        } else{
            // Lee el nodo en el que se encuentra actualmente a través de parent
            file.seekg(pos_node, ios::beg);
            NodeAVL<T> parent;
            file.read((char*)&parent,parent.size()); //parent.size());

            // Compara el valor del nodo actual con el ingresado
            if (node.value < parent.value){
                insert(parent.left, node);              // Llama a la función insert para el hijo izquierdo de parent

            } else if(node.value > parent.value){
                insert(parent.right, node);             // Llama a la función insert para el hijo izquierdo de parent
                //cout<<"new parent.right: "<<parent.right<<endl;


            } else{
                // Cuando ya existe un nodo con dicho valor     
                int new_pos = parent.next; 
                while (parent.next!=-1){ //lee para insertar en next
                    new_pos = parent.next;
                    file.seekg(parent.next,ios::beg);
                    file.read((char*)&parent, parent.size());
                }

                if (new_pos==-1) new_pos = pos_node;
                
                //cout<<"insertando repetido";
                //coloca nodo al final
                file.seekg(0, ios::end); 
                parent.next = file.tellg();
                file.seekp(0, ios::end); 
                node.next = -1;
                file.write((char*)&node, node.size());

                // guarda cambios por "next"
                file.seekp(new_pos, ios::beg);
                file.write((char*)&parent, parent.size());
                file.close();
                return true;
                    
                
            }
            // Actualización del nodo padre
            //cout << "Actualizando padre" <<"(posicion "<<pos_node<<")"<< endl;
            NodeAVL<T> l;
            NodeAVL<T> r;
            long acc = 0;
            if(parent.left != -1){
                //cout << "Hijo izquierdo existe" << endl;
                file.seekg(parent.left);
                file.read((char*)& l, l.size());
                acc = l.height;
            }
            if(parent.right != -1){
                //cout << "Hijo derecho existe" << endl;
                file.seekg(parent.right);
                file.read((char*)& r, r.size());
                if(r.height > acc){
                    acc = r.height;
                }
            }
            if(acc+1 > parent.height) {
                parent.height++;
                //cout << "Altura del padre actualizada" << endl;
            }
            file.seekp(pos_node, ios::beg);
            file.seekg(pos_node, ios::beg);
            file.write((char*)&parent, parent.size());

            // Balanceo
            balance(pos_node, parent);
            return true;
        }
    }

int nro_registros(){
        file.open(this->filename, ios::binary|ios::in); //abre modo lectura
        if(!file.is_open()) throw runtime_error("No se pudo abrir el archivo");
        file.clear();
        file.seekg(0, ios::end);
        long total_bytes = file.tellg();
        //cout << "Total bytes: " << total_bytes << endl;
        file.close();
        Record record;
        return (total_bytes/record.size());   // Registro en data.dat
    }

    long balanceFactor(NodeAVL<T> &node){//balance factor
        long alturaIzq=altura(node.left); //altura del hijo izquierdo
        //cout << "Altura del hijo izquierdo: " << alturaIzq << endl;
        long alturaDer=altura(node.right); //altura del hijo derecho
        //cout << "Altura del hijo derecho: " << alturaDer << endl;
        return alturaIzq-alturaDer; //retorna la diferencia de alturas entre el hijo izquierdo y el derecho
    }

    long altura(long node){//retorna la altura del nodo
        if(node==-1) return -1;
        file.seekg(node,ios::beg);//se posiciona en el nodo
        NodeAVL<T> nodo;
        file.read((char*)&nodo,nodo.size());//se lee el nodo
        return nodo.height; //retorna la altura del nodo
    
    }

    void alturaActualizada(long pos, NodeAVL<T> &node){//actualiza la altura del nodo
        long alturaIzq=altura(node.left); //altura del hijo izquierdo
        long alturaDer=altura(node.right); //altura del hijo derecho

        //recalculamos la latura del nodo
        if(alturaIzq>alturaDer) node.height=alturaIzq+1;
        else node.height=alturaDer+1;

        //escribimos el nodo
        file.seekp(pos,ios::beg);//se posiciona en el nodo
        file.write((char*)&node,node.size());//se escribe el nodo
    }

    NodeAVL<T> find_sucesor(NodeAVL<T> &node){//retorna el sucesor del nodo
        NodeAVL<T> nodo;
        file.seekg(node.right,ios::beg);//se posiciona en el hijo derecho
        file.read((char*)&nodo,nodo.size());//se lee el hijo derecho
        long pos=nodo.left;//posicion del hijo izquierdo del hijo derecho

        while(pos!=-1){//mientras exista un hijo izquierdo
            file.seekg(pos,ios::beg);//se posiciona en el hijo izquierdo
            file.read((char*)&nodo,nodo.size());//se lee el hijo izquierdo
            pos=nodo.left;//posicion del hijo izquierdo del hijo derecho
        }
        return nodo;//retorna el sucesor
    }
};



template <typename T>
void AVLFile<T>::balance(long pos, NodeAVL<T> &node){//verifica si una rotación es necesaria
    //factor de balanceo
    //cout<<"ingreso de balanceo..............................................."<<endl;
        long FacB=balanceFactor(node); //balanceFactor es la diferencia de alturas entre el hijo izquierdo y el derecho
        //cout << "Factor de balanceo del nodo " << node.value << ": " << FacB << endl;
        if(FacB>1){ //si el factor de balanceo es mayor a 1, se debe rotar a la derecha
            NodeAVL<T> hijoIzq; //nodo auxiliar para el hijo izquierdo
            file.seekg(node.left,ios::beg);//se posiciona en el hijo izquierdo
            file.read((char*)&hijoIzq,hijoIzq.size());//se lee el hijo izquierdo
            if(balanceFactor(hijoIzq)<=-1){ //si el factor de balanceo del hijo izquierdo es menor o igual a -1, se debe hacer una rotación doble a la derecha
                //cout << "\tEjecutando rotacion doble hacia la izquierda..." << endl;
                left_Rotation(pos,node);
            }
            //cout << "Ejecutando rotacion a la derecha..." << endl;
            right_Rotation(pos,node);
        }

        else if(FacB<=-2){ //si el factor de balanceo es menor o igual a -2, se debe rotar a la izquierda
            NodeAVL<T> hijoDer; //nodo auxiliar para el hijo derecho
            file.seekg(node.right,ios::beg);//se posiciona en el hijo derecho
            file.read((char*)&hijoDer,hijoDer.size());//se lee el hijo derecho

            if(balanceFactor(hijoDer)>=1){ //si el factor de balanceo del hijo derecho es mayor o igual a 1, se debe hacer una rotación doble a la izquierda
                //cout << "\tEjecutando rotacion doble hacia la derecha..." << endl;
                right_Rotation(pos,node);
            }
            //cout << "Ejecutando rotacion a la izquierda..." << endl;
            left_Rotation(pos,node); 
        }
}

template <typename T>
void AVLFile<T>::left_Rotation(long node_pos, NodeAVL<T> &node){//rotación a la izquierda
    NodeAVL<T> hijoDer; //nodo auxiliar para el hijo derecho
    long r_pos = node.right;

    file.seekg(r_pos,ios::beg);//se posiciona en el hijo derecho
    file.read((char*)&hijoDer,hijoDer.size());//se lee el hijo derecho

    node.right=hijoDer.left; //el hijo derecho del nodo actual es el hijo izquierdo del hijo derecho
    hijoDer.left=r_pos; //el hijo izquierdo del hijo derecho es el nodo actual

    file.seekp(node_pos,ios::beg);//se posiciona en el nodo actual
    file.write((char*)&hijoDer,hijoDer.size());//se escribe el hijo derecho
    

    file.seekp(r_pos,ios::beg);//se posiciona en el hijo derecho
    file.write((char*)&node,node.size());//se escribe el nodo actual

    //actualizar altura
    alturaActualizada(r_pos, node); //actualiza la altura del nodo actual
    alturaActualizada(node_pos, hijoDer); //actualiza la altura del hijo derecho

    //actualizar el node
    node=hijoDer;
    //escribimos el valor del nodo para que se guarde
    file.seekp(node_pos,ios::beg);//se posiciona en el nodo actual
    file.write((char*)&node,node.size());//se escribe el nodo actual



}

template <typename T>
void AVLFile<T>::right_Rotation(long pos, NodeAVL<T> &node){
    NodeAVL<T> hijoIzq; //nodo auxiliar para el hijo izquierdo
    long l_pos = node.left;

    file.seekg(l_pos,ios::beg);//se posiciona en el hijo izquierdo
    file.read((char*)&hijoIzq,hijoIzq.size());//se lee el hijo izquierdo

    node.left=hijoIzq.right; //el hijo izquierdo del nodo actual es el hijo derecho del hijo izquierdo
    hijoIzq.right=l_pos; //el hijo derecho del hijo izquierdo es el nodo actual

    file.seekp(pos,ios::beg);//se posiciona en el nodo actual
    file.write((char*)&hijoIzq,hijoIzq.size());//se escribe el hijo izquierdo

    file.seekp(l_pos,ios::beg);//se posiciona en el hijo izquierdo
    file.write((char*)&node,node.size());//se escribe el nodo actual

    //actualizar altura
    alturaActualizada(l_pos, node); //actualiza la altura del nodo actual
    alturaActualizada(pos, hijoIzq); //actualiza la altura del hijo izquierdo

    //actualizar el node 
    node=hijoIzq;
    //escribimos el valor del nodo para que se guarde
    file.seekp(pos,ios::beg);//se posiciona en el nodo actual
    file.write((char*)&node,node.size());//se escribe el nodo actual
}


template <typename T>
AVLFile<T>::AVLFile(){
    root = -1;
}

template <typename T>
AVLFile<T>::AVLFile(string file_name, string atributo){//, int atributo_col){
    this->filename = file_name;
    this->heap_file = "avl_"+atributo+".bin";

    root = -1;


    //crear_archivo(this->filename);
    crear_archivo(this->heap_file);
    // crear archivos (si es necesario)
    //buildFromFile(atributo_col); // se genera AVL en base a una columna especifica
}

template <typename T>
vector <long> AVLFile<T>::search(T key){
    file.open(heap_file,ios::binary|ios::in); // Abrir el archivo en binario
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
        } else if (nodo.value < key){
            pos = nodo.right; //Actualizar pos a hijo derecho
        }  else{
            //cout<<"no debería llegar aqui"<<endl;
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
        //throw runtime_error("No se encontró");
        //cout<<"No se encontró "<<key<<endl;
        file.close();
        return pointerValueList;
    }

    file.close();

    return pointerValueList;
}

template <typename T>
vector <long> AVLFile<T>::rangeSearch(T begin_key, T end_key){
    vector <long> pointerValueList = {}; // Vector a retornar (vacio al comienzo)

    for (T i = begin_key; i <= end_key ; i++) {
        vector <long> vAux = search(i); // Llamada a función search, se guarda en vAux
        pointerValueList.insert(pointerValueList.end(), vAux.begin(), vAux.end()); // Concatenar
    }

    return pointerValueList;
}

template <typename T>
bool AVLFile<T>::remove(T key){
    file.open(this->heap_file,ios::binary|ios::in|ios::out);
    
    int result = this->remove(root, key);
    file.close();

    if (result!=-1){ size--; return true;}
    return false;
}

template <typename T>
void AVLFile<T>::delete_equals(long pos){
    //file.open(this->heap_file, ios::binary|ios::out|ios::in); //para leer y escribir
    
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
            file.write((char*)&nodo, nodo.size()); //escribimos "nodo" con next=-1
        } //iteramos hasta llegar a nodo con next=-1
    }

    //file.close();
}

template <typename T>
void AVLFile<T>::buildFromFile(string sourceName, int atributo_col){
    this->filename = sourceName;
    ifstream source(sourceName, ios::binary);
    string msg = "No existe archivo" + filename;
    if (!source.is_open()) throw runtime_error(msg);

    long bytes = 0;
    source.seekg(0,ios::end);
    int max_bytes = source.tellg();
    //cargamos los datos uno a uno  //cargamos cada registro del file_name como un nodo
    source.seekg(0,ios::beg);
    while (bytes < max_bytes){
        Record record;
        source.seekg(bytes,ios::beg);
        record.read(source);

        NodeAVL<T> nodo;
        if (atributo_col == 2) {nodo.setValue(bytes, record.atrib2);} // creamos el nodo
        else {
            nodo.setValue(bytes, record.atrib4);
        }

        this->insert(nodo);
        //record.print();
        bytes+= record.size(); // se aumenta contador de bytes
    }
    source.close();
}

template <typename T>
AVLFile<T>::~AVLFile(){}

#endif