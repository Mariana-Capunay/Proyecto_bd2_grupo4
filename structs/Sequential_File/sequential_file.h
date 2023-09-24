#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <tuple>
#include "record.h"

using namespace std;

template <typename T>
struct Nodo{
    int regPos; // posicion (en file de registros) del registro correspondiente al value
    T value; // valor de la key guardada
    int next; // posicion (en archivos de nodos) del nodo siguiente (si no hay, -1)
    char nextFile; // char que indica si next está en el heap ('a' de aux.dat), o sorted ('d' de datos.dat)

    Nodo() {
        regPos = -1;
        next = -1;
        nextFile = ' ';
    }

    Nodo(int puntero, T key) {
        regPos = puntero;
        value = key;
        next = -1;
        nextFile = ' ';
    }

    Nodo(int puntero, T key, int nodoNext, char nodoNextFile) {
        regPos = puntero;
        value = key;
        next = nodoNext;
        nextFile = nodoNextFile;
    }

    void setValue(int puntero, T key) {
        regPos = puntero;
        value = key;
    }

    void setNext(int nodoNext, char nodoNextFile) {
        next = nodoNext;
        nextFile = nodoNextFile;
    }

    void print(){
        cout << regPos;
        cout << " ";
        cout << value;
        cout << " ";
        cout << next;
        cout << " ";
        cout << nextFile;
        cout << endl;
    }

    int size() {
        return 2*sizeof(int) + sizeof(T) + sizeof(char);
    }
};


template <typename T>
class SequentialFile {
private:
    // Primer nodo
    int primerNodoPos = -1; // posicion (en archivos de nodos) del primer nodo (si no hay, -1)
    char primerNodoFile = ' ';

    int kHeapLimit; // Limite k de nodos en el heap antes de ser reconstruido

    // Files
    fstream* regFile;
    fstream* sortedNodeFile;
    fstream* heapNodeFile;

    // Devuelve el nro de registros en file.bin
    int nro_registros() {
        this->regFile->seekg(0, ios::end);
        Record r;
        return (this->regFile->tellg() / r.size());
    }

    // Devuelve el nro de nodos en aux.dat o datos.dat, dependiendo qué le pasen
    int nro_nodos(string nodeFile) {
        Nodo<T> nodo;
        if (nodeFile == "datos.dat"){
            sortedNodeFile->seekg(0, ios::end);
            return (sortedNodeFile->tellg() / nodo.size());
        } else if (nodeFile == "aux.dat") {
            heapNodeFile->seekg(0, ios::end);
            return (heapNodeFile->tellg() / nodo.size());
        }
    }

    void insert (int regPos, T key) {
        Nodo<T> nuevoNodo(regPos, key); // Creo el nuevo nodo

        // Si sortedNodeFile está vacio insertar directamente al sortedNodeFile
        int nNodos = nro_nodos("datos.dat");
        if (nNodos == 0) {
            // Actualizar primerNodo de la clase Sequential
            primerNodoPos = 0;
            primerNodoFile = 'd';

            // Escribir nuevo nodo
            sortedNodeFile->seekg(0, ios::beg);
            sortedNodeFile->write((char*) &nuevoNodo, nuevoNodo.size());
        } else {
            // Si la nueva key es mayor a la ultima key del sortedFile, insertar directamente al final
            Nodo<T> last;

            // Leer el ultimo nodo en el sortedNodeFile
            sortedNodeFile->seekg((nNodos-1)*last.size(), ios::beg);
            sortedNodeFile->read((char*) &last, last.size());

            if (key > last.value) {
                // Actualizar next del ultimo nodo en datos.dat
                last.setNext(nNodos, 'd');
                sortedNodeFile->seekg((nNodos-1)*last.size(), ios::beg);
                sortedNodeFile->write((char*) &last, last.size());

                // Escribir nuevo nodo (luego del ultimo)
                sortedNodeFile->write((char*) &nuevoNodo, nuevoNodo.size());
            } else {
                // Si la nueva key es menor al valor del primer nodo
                Nodo<T> first;

                // Leer el primer nodo del SequentialFile (ya sea en el datos.dat o aux.dat)
                if (primerNodoFile == 'a') {
                    heapNodeFile->seekg(primerNodoPos*first.size(), ios::beg);
                    heapNodeFile->read((char*) &first, first.size());
                } else if (primerNodoFile == 'd') {
                    sortedNodeFile->seekg(primerNodoPos*first.size(), ios::beg);
                    sortedNodeFile->read((char*) &first, first.size());
                }

                if (first.value < key) {
                    // El nuevo nodo guardará como next al primer nodo
                    nuevoNodo.setNext(primerNodoPos, primerNodoFile);

                    // Escribir el nuevo nodo en aux.dat (heapFile)
                    int nNodosHeap = nro_nodos("aux.dat");
                    heapNodeFile->seekg(nNodosHeap*nuevoNodo.size(), ios::beg);
                    heapNodeFile->write((char*) &nuevoNodo, nuevoNodo.size());

                    // Actualizar primer nodo del Sequential por el nuevo
                    primerNodoPos = nNodosHeap;
                    primerNodoFile = 'a';
                } else {
                    // Si no se cumple ninguno de los anteriores casos,
                    // buscar el nodo que le antecede, actualizar los next's, e insertar en heapNodeFile
                    auto [prevPos, prevFile] = locatePrev(key);

                    // No insertar si ya existe la llave
                    if (prevPos == -1) {
                        return;
                    }

                    Nodo<T> prev;
                    int nNodosHeap = nro_nodos("aux.dat");

                    // Guardar el next de prev en next del nuevo y
                    // actualizar el next de prev para que apunte al nuevo
                    if (prevFile == 'a') {
                        heapNodeFile->seekg(prevPos*prev.size(), ios::beg);
                        heapNodeFile->read((char*) &prev, prev.size());
                        nuevoNodo.setNext(prev.next, prev.nextFile);
                        prev.setNext(nNodosHeap, 'a');
                        heapNodeFile->seekg(prevPos*prev.size(), ios::beg);
                        heapNodeFile->write((char*) &prev, prev.size());
                    } else if (prevFile == 'd') {
                        sortedNodeFile->seekg(prevPos*prev.size(), ios::beg);
                        sortedNodeFile->read((char*) &prev, prev.size());
                        nuevoNodo.setNext(prev.next, prev.nextFile);
                        prev.setNext(nNodosHeap, 'a');
                        sortedNodeFile->seekg(prevPos*prev.size(), ios::beg);
                        sortedNodeFile->write((char*) &prev, prev.size());
                    }

                    // Escribir el nuevo nodo en aux.dat (heapFile)
                    heapNodeFile->seekg(nNodosHeap*nuevoNodo.size(), ios::beg);
                    heapNodeFile->write((char*) &nuevoNodo, nuevoNodo.size());
                }
            }
        }
    }

    tuple<int, char> locatePrev (T key) {
        int nNodos = nro_nodos("datos.dat");
        int left = 0;
        int right = nNodos - 1;
        int currNodePos;
        Nodo<T> nodo;

        for (int i = 0; i < log2(nNodos); ++i) {
            currNodePos = floor((left + right)/2);
            sortedNodeFile->seekg(currNodePos*nodo.size(), ios::beg);
            sortedNodeFile->read((char*) &nodo, nodo.size());
            if (nodo.value == key) {
                return {-1,' '};
            } else if (nodo.value > key) {
                right = currNodePos - 1;
            } else {
                left = currNodePos + 1;
            }
        }

        // Si en la búsqueda se pasó el lugar, cambiar al anterior
        if (nodo.value > key) {
            currNodePos--;
            sortedNodeFile->seekg(currNodePos*nodo.size(), ios::beg);
            sortedNodeFile->read((char*) &nodo, nodo.size());
        }

        // Buscar también en los next del nodo
        while (nodo.next != -1 && nodo.nextFile == 'a') {
            Nodo<T> nodoAux;
            heapNodeFile->seekg(nodo.next*nodoAux.size(), ios::beg);
            heapNodeFile->read((char*) &nodoAux, nodoAux.size());

            if (nodoAux.value > key) {
                break;
            } else {
                nodo = nodoAux;
            }
            // TODO: ultimo caso
        }

        return {3,'d'};
        return {1,'a'};
    }

    void rebuild () {
        vector<Nodo<T>> nodos;
        Nodo<T> nodo;
        int pos = primerNodoPos;
        fstream* nodeFile;
        if (primerNodoFile == 'a') {
            nodeFile = heapNodeFile;
        } else if (primerNodoFile == 'd') {
            nodeFile = sortedNodeFile;
        }

        // Llenar el vector nodos
        while (pos != -1) {
            nodeFile->seekg(pos*nodo.size(), ios::beg);
            nodeFile->read((char*) &nodo, nodo.size());
            nodos.push_back(nodo);

            if (nodo.nextFile == 'a') {
                nodeFile = heapNodeFile;
            } else if (nodo.nextFile == 'd') {
                nodeFile = sortedNodeFile;
            }
            pos = nodo.next;
        }

        // Actualizar primer nodo del Sequential
        primerNodoPos = 0;
        primerNodoFile = 'd';

        // Sobreescribir datos.dat
        sortedNodeFile->seekg(0, ios::beg);
        for (int i = 0; i < nodos.size(); ++i) {
            nodos[i].setNext(i+1, 'd'); // Actualizar next de cada nodo ahora ordenado
            sortedNodeFile->write((char*) &nodos[i], nodos[i].size());
        }

        // Vaciar aux.dat
        // TODO
    }


public:
    SequentialFile () {}

    SequentialFile (string file_name) {

        fstream regF(file_name, ios::binary | ios::in | ios::out);
        regFile = &regF;

        fstream sortedF ("datos.dat", ios::binary | ios::in | ios::out | ios::trunc);
        sortedNodeFile = &sortedF;

        fstream heapF ("aux.dat", ios::binary | ios::in | ios::out | ios::trunc);
        heapNodeFile = &heapF;

        int nRegs = nro_registros();
        if (nRegs > 0) {
            kHeapLimit = trunc(log2(nRegs));
            Record r;
            regFile->seekg(0, ios::beg);
            for (int i = 0; i < nRegs; ++i) {
                regFile->read((char*) &r, r.size());
                insert(i, r.key);
            }
        } else {
            kHeapLimit = 10;
        }
    }

    bool add (Record record) {

    }

    virtual ~SequentialFile() {
        this->regFile->close();
        this->sortedNodeFile->close();
        this->heapNodeFile->close();
    }

    void printRegistros(int n) {
        cout << "Archivo de registros:" << endl;
        fstream registros ("file.bin", ios::binary | ios::in | ios::out);
        Record r;
        registros.seekg(0, ios::beg);
        for (int i = 0; i < n; ++i) {
            registros.read((char*) &r, r.size());
            r.print();
        }
    }

    void printSortedNodes(int n) {
        cout << "Archivo ordenado:" << endl;
        fstream ordenado ("datos.dat", ios::binary | ios::in | ios::out);
        Nodo<int> nodo;
        ordenado.seekg(0, ios::beg);
        for (int i = 0; i < n; ++i) {
            ordenado.read((char*) &nodo, nodo.size());
            nodo.print();
        }
    }

    void printHeapNodes(int n) {
        cout << "Archivo auxiliar:" << endl;
        fstream heap ("aux.dat", ios::binary | ios::in | ios::out);
        Nodo<int> nodo;
        heap.seekg(0, ios::beg);
        for (int i = 0; i < n; ++i) {
            heap.read((char*) &nodo, nodo.size());
            nodo.print();
        }
    }

};

#endif