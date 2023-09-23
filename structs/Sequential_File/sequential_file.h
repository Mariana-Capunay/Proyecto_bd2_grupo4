#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;


template <typename T>
struct Nodo{
    long pointer_value; // posicion (en file de registros) del registro correspondiente al value
    T value; // valor de la key guardada
    long next; // posicion (en archivos de nodos) del nodo siguiente (si no hay, -1)
    char nextFile; // char que indica si next está en el heap ('a' de aux.dat), o sorted ('d' de datos.dat)

    Nodo() {
        pointer_value = -1;
        next = -1;
        nextFile = ' ';
    }

    Nodo(long puntero, T key) {
        pointer_value = puntero;
        value = key;
        next = -1;
        nextFile = ' ';
    }

    Nodo(long puntero, T key, long punteroNext, char fileLoc) {
        pointer_value = puntero;
        value = key;
        next = punteroNext;
        nextFile = fileLoc;
    }

    void setValue(long puntero, T key) {
        pointer_value = puntero;
        value = key;
    }

    void setNext(long puntero) {
        next = puntero;
    }

    int size() {
        return 2*sizeof(long) + sizeof(T) + sizeof(char);
    }
};


template <typename T>
class SequentialFile {
private:
    // Punteros
    long root; //posicion en archivo file del primer registro, -1 si no hay
    int kHeapLimit;

    // File names
    string file_name; //data set en binario
    string reorg_file_name;//archivo con llave ordenadas
    string heap_file_name; // para guardar insercciones

    // Files
    fstream regFile;
    fstream sortedNodeFile;
    fstream heapNodeFile;

    long nro_registros() {
        this->regFile.seekg(0, ios::end);
        Record r;
        long nRegs = this->regFile.tellg() / r.size();

        return nRegs;
    }

    long nro_nodos(string nodeFile) {
        // Devuelve el nro de nodos del archivo aux.dat o el de datos.dat, dependiendo qué le pasen
        Nodo<T> nodo;
        long nNodos;
        if (nodeFile == "datos.dat"){
            sortedNodeFile.seekg(0, ios::end);
            nNodos = sortedNodeFile.tellg() / nodo.size();
        } else if (nodeFile == "aux.dat") {
            heapNodeFile.seekg(0, ios::end);
            nNodos = heapNodeFile.tellg() / nodo.size();
        }

        return nNodos;
    }

    void insert (long pos, T key) {
        Nodo<T> nuevoNodo(pos, key); // Creo el nuevo nodo

        // si sortedNodeFile está vacio insertar directamente al sortedNodeFile
        long nNodos = nro_nodos("datos.dat");
        if (nNodos == 0) {
            sortedNodeFile.seekg(0, ios::beg);
            sortedNodeFile.write((char*) &nuevoNodo, nuevoNodo.size());
        }

        // si a nueva key es mayor a la ultima key del archivo, insertar directamente al final
        Nodo<T> last;
        sortedNodeFile.seekg((nNodos-1)*last.size(), ios::beg); // Leer el ultimo nodo en el sortedNodeFile
        sortedNodeFile.read((char*) &last, last.size());
        if (key > last.value) {
            sortedNodeFile.write((char*) &nuevoNodo, nuevoNodo.size());
        }

        // sino, buscar el nodo que le antecede, cambiar el next, e insertar en heapNodeFile
        Nodo<T> prev = locatePlace(key);



    }

    Nodo<T> locatePlace (T key) {}
    //vendria a ser esto: (adaptar)
//    int binarySearchPosition(const Record& nuevoRecord) {
//
//        /*
//        En este código, el método binarySearchPosition utiliza la búsqueda binaria para encontrar
//        la posición de inserción del nuevo registro nuevoRecord en orden alfabético. Devolverá la
//        posición en la que debería insertarse el nuevo registro en función del nombre. Si el
//        registro ya existe en la lista, devolverá la posición actual del registro encontrado.
//        */
//        ifstream file(this->data_file, ios::binary);
//        if (!file.is_open()) throw ("No se pudo abrir el archivo");
//
//        int left = 0;
//        int right = size(this->data_file) - 1;
//
//        while (left <= right) {
//            int middle = left + (right - left) / 2;
//            Record record = readRecord(middle, this->data_file);//Obtener el registro en la mitad
//
//            // Comparar el nombre del registro con el nombre del nuevo record
//            int cmp = record.compare(nuevoRecord);
//
//            if (cmp == 0) {//Si el nombre del registro es igual al nombre del nuevo record
//
//                file.close();
//                middle += 1; //Si encuentra un nombre igual que me el nuevo registro se ponga despues
//
//                //Para la eliminacion si B se inserta despues que A, tenemos que verificar que este no apunte a un -1
//                //En caso a apunte a un menos -1, vamos a ir retrocediendo de pos en data_file hasta encontrar el mas registro
//                //superior alfabeticamente a B que no este eliminado.
//                while (getPunteroAtPosition(middle-1,this->data_file) == -1){
//                    middle -= 1;
//                    //Si esta A y quiero insertar B. Entonces el metodo me retornada left, ya que A estaba en left -1
//                }
//
//                return middle; // Encontrado, devuelve la posición actual
//            } else if (cmp < 0) {
//                left = middle + 1; // El nombre está en la mitad derecha
//            } else {
//                right = middle - 1; // El nombre está en la mitad izquierda
//            }
//        }
//
//        file.close();
//
//        // Si no se encuentra un registro igual, devolver la posición en la que debería insertarse
//
//        //Para la eliminacion si B se inserta despues que A, tenemos que verificar que este no apunte a un -1
//        //En caso a apunte a un menos -1, vamos a ir retrocediendo de pos en data_file hasta encontrar el mas registro superior alfabeticamente a B que no este eliminado.
//        while (getPunteroAtPosition(left-1,this->data_file) == -1){
//            left -= 1;
//
//            //Si esta A y quiero insertar B. Entonces el metodo me retornada left, ya que A estaba en left -1
//        }
//        return left;
//    }

public:
    SequentialFile () {
        root = -1;
    }

    SequentialFile (string file_name) {
        root = -1;

        this->file_name = file_name;
        reorg_file_name = "datos.dat";
        heap_file_name = "aux.bin";

        regFile.open(file_name, ios::binary | ios::in | ios::out);
        sortedNodeFile.open(reorg_file_name, ios::binary | ios::in | ios::out);
        heapNodeFile.open(heap_file_name, ios::binary | ios::in | ios::out);

        long nRegs = nro_registros();
        if (nRegs > 0) {
            kHeapLimit = trunc(log2(nRegs));
            Record r;
            regFile.seekg(0, ios::beg);
            for (int i = 0; i < nRegs; ++i) {
                regFile.read((char*) &r, r.size());
                insert(i, r.atrib1);
            }
        }
    }

    bool add (Record record) {

    }

    virtual ~SequentialFile() {
        this->regFile.close();
        this->sortedNodeFile.close();
        this->heapNodeFile.close();
    }

};

/*Cambios:
 * se cambió Node a Nodo para evitar conflictos con el std
 *
 * nuevo atributo a Nodo, un char que indica si el next esta en el heap (aux.dat) o sorted (datos.dat)
(ya no va lo de usar positivos y negativos, se complica innecesariamente)
 *
 * coloqué files en la clase SequentialFile y los abro en el constructor para no tener que abrirlos y cerrarlos en cada funcion
(cierro los archivos en el destructor de clase)
 *
 * se eliminó el atributo de SequentialFile que guardaba la posicion al primer elemento, ya que siempre será el primero del sorted file (datos.dat)
 *
 * */


#endif