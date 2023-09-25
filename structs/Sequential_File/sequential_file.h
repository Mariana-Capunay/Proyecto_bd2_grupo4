/**
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include "../../dataset_bin/binary_conversor.h"
using namespace std;

class Record {
private:

public:
    char data[40];
    long puntero;

    void setData(){
        cout<<"data: "; cin>>data;
    }
    void showData(){
        cout<<"data: "; cout<<data;
    }

    // Agregar un método para comparar el nombre de dos records
    int compare(const Record& otro) const { // const para que no se modifique el objeto
        return strcmp(Nombre, otro.Nombre);//strcmp devuelve 0 si son iguales, 1 si el primero es mayor al segundo, -1 si el primero es menor al segundo
    }

};

class SequentialFile
{
private:
    string document,atr;
    string data_file;//guara los registros
    string aux_file;//guarda los punteros

    int data_size;//cantidad de registros en data.bin
    int aux_size;//cantidad de registros en auxiliar.bin

public:


    SequentialFile(string _document,string _atr){
        document = _document;
        atr = _atr;
        /*
        En el constructor:
        -   Creamos data.bin y auxiliar.bin de ser necesario
        -   Insertamos "la cabecera" que siempre va a ir antes alfabeticamente que cualquier otro elemento.
        -   Almacenamos el data_size y el aux_size, asi ahorramos accesos a la memoria
        */


        this->data_file = _atr + "data.bin";
        this->aux_file = _atr + "auxiliar.bin";

        // Verificar si el archivo está vacío antes de escribir el registro predeterminado
        ifstream checkFile(data_file, ios::binary);
        if (!checkFile.is_open()) {
            // El archivo no existe, entonces lo creamos y escribimos el registro predeterminado
            Record defaultRecord;
            strcpy(defaultRecord.Nombre, "\0\0\0\0\0\0\0\0\0\0\0\0");
            defaultRecord.Puntero = 0.1;
            writeRecordPOS(defaultRecord, 0, this->data_file);
        } else {
            // El archivo existe, no necesitamos escribir el registro predeterminado nuevamente
            checkFile.close();
        }

        //Crear aux_file tomando en cuenta que puede tener cosas dentro
        // Verificar si el archivo auxiliar ya existe y contiene datos
        ifstream auxCheckFile(this->aux_file, ios::binary);
        if (!auxCheckFile.is_open())
        {
            // El archivo auxiliar no existe o está vacío, entonces lo creamos y escribimos el registro predeterminado
            ofstream auxFile(this->aux_file, ios::binary);
            if (!auxFile.is_open())
            {
                throw ("No se pudo abrir el archivo auxiliar");
            }
            auxFile.close();
        }
        else
        {
            // El archivo auxiliar ya existe y contiene datos, no necesitamos crearlo nuevamente
            auxCheckFile.close();
        }

        data_size = size(this->data_file);
        aux_size = size(this->aux_file);

    };

    //Metodo que imprime todos los archivos de data y auxiliar
    void print(){

        cout << "size data y aux :" << data_size <<" "<<aux_size<<endl;

        SequentialFile file2(document,atr);
        vector<Record> records = file2.scanAll("data.bin");
        for(Record r : records){
            r.showData();
        }

        cout << endl << "-----------------------" <<endl;

        //Lectura
        SequentialFile file5(document,atr);
        vector<Record> records2 = file5.scanAll("auxiliar.bin");
        for(Record r : records2){
            r.showData();
        }

    }

    void add(Record record){
        //Ubicamos el elemento anterior a record
        int X_pos = binarySearchPosition(record) - 1; //binarySearchPOS me devuelve la pos donde debo insertar el elemento, por eso el -1
        Record X = readRecord(X_pos,this->data_file);

        //Si X apunta a un elemento de la misma Data
        /*
            add(Manuel)
            ======================
            Data:
            "     " -> 0.1
            Asucena -> 0.2  <- X
            Ortega  -> 0
            ======================
            Pedro   -> 0.3
        */
        if(analyzeFloat(X.Puntero)=="Data"){
            //Agregamos el record al aux y hacemos el cambiaso de punteros entre X y record
            record.Puntero = X.Puntero; //record -> X.Puntero
            writeRecordEND(record, this->aux_file);
            updatePunteroAtPosition(X_pos, aux_size, this->data_file); // X -> pos(record)

            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
            aux_size += 1;
        }

        //Si X apunta a un elemento perteneciente a Auxiliar
        /*
            add(Manuel)
            ======================
            Data:
            "     " -> 0.1
            Asucena -> 0   <- X
            Ortega  -> 0.3
            ======================
            Auxiliar:
            Belen   ->  0.2 <- Y
        */
        if(analyzeFloat(X.Puntero)=="Auxiliar"){
            //Si apunta a un auxiliar primero me tengo que ubicar en auxiliarfile[X.Puntero]

            //Y_pos = X.Puntero;
            Record Y = readRecord(X.Puntero,this->aux_file);

            //Caso particular (Todavia no nos hemos adentrado al 100% en aux_file, estamos en el limbo)
            /*
                add(Baldor)
                ======================
                Data:
                "     " -> 0.1
                Asucena -> 0   <- X      (X todavia esta en data file, mientras que Y ya esta exclusivamente en el auxiliar file)
                Ortega  -> 0.3
                ======================
                Auxiliar:
                Belen   ->  0.2 <- Y    (Baldor va antes que que Belen)
             */
            int cmp = record.compare(Y);
            if (cmp <= 0){ //Si record va antes que Y o es igual a Y
                //Agregamos el record al aux y hacemos el cambiaso de punteros entre X y record
                record.Puntero = X.Puntero; //record -> X.Puntero
                writeRecordEND(record, this->aux_file);
                updatePunteroAtPosition(X_pos, aux_size, this->data_file); // X -> pos(record)

                //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                aux_size += 1;
            }

            //Si record va despues que Y
            if (cmp > 0){

                //Caso particular (Todavia no nos hemos adentrado al 100% en aux_file, estamos en el limbo)
                /*
                    add(Boris)
                    ======================
                    Data:
                    "     " -> 0.1
                    Asucena -> 0   <- X      (X todavia esta en data file, mientras que Y ya esta exclusivamente en el auxiliar file)
                    Ortega  -> 0.3
                    ======================
                    Auxiliar:
                    Belen   ->  0.2 <- Y    (Boris va despues que que Belen, pero despues de Belen no hay nada mas)
                 */
                if (analyzeFloat(Y.Puntero) == "Data"){
                    record.Puntero = Y.Puntero;
                    writeRecordEND(record, this->aux_file);
                    updatePunteroAtPosition(X.Puntero, aux_size, this->aux_file); // X -> pos(record)
                    //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                    aux_size += 1;
                }

                //Ahora si tanto X como Y pertenecen a auxiliar file
                else {
                    bool posicionado = false;
                    int cmp2;
                    while(!posicionado){

                        X_pos = X.Puntero; //X_pos = Y_pos
                        X = readRecord(X_pos,this->aux_file);

                        //Y_pos = X.Puntero
                        Y = readRecord(X.Puntero,this->aux_file);

                        /*
                            add(Conchita)
                            ======================
                            Data:
                            "     " -> 0.1
                            Asucena -> 0   <- X
                            Ortega  -> 0.3
                            ======================
                            Auxiliar:
                            Belen   ->  1   <- Y
                            Boris   ->  0.2

                            >>>>>>>>>>>>>>>>>>>>>>

                            add(Conchita)
                            ======================
                            Data:
                            "     " -> 0.1
                            Asucena -> 0
                            Ortega  -> 0.3
                            ======================
                            Auxiliar:
                            Belen   ->  1   <- X
                            Boris   ->  0.2 <- Y

                         */
                        cmp2 = record.compare(Y);

                        //Si record va antes que Y
                        if (cmp2 <= 0){
                            //Agregamos el record al aux y hacemos el cambiaso de punteros entre X y record
                            record.Puntero = X.Puntero; //record -> X.Puntero
                            writeRecordEND(record, this->aux_file);
                            updatePunteroAtPosition(X_pos, aux_size, this->aux_file); // X -> pos(record)

                            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                            aux_size += 1;

                            posicionado = true;
                        }

                        //Si record va despues que Y, y este Y apunta a un elemento perteneciente a data.file (osea que ya no apunta a ningun otro elemento)
                        if (cmp2 > 0 && analyzeFloat(Y.Puntero)=="Data"){

                            record.Puntero = Y.Puntero;
                            writeRecordEND(record, this->aux_file);
                            updatePunteroAtPosition(X.Puntero/*Y_pos*/, aux_size, this->aux_file);

                            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                            aux_size += 1;

                            posicionado = true;
                        }

                        /*

                         En caso record vaya despues que Y pero Y apunta a otro elemento perteneciente a auxixiliar.bin
                         (osea no se ha llegao al final)

                         Se repite el while y con ello
                         -  X pasa a ser Y(1)
                         -  Y pasa a ser el Y(1).Puntero
                         */
                    }
                }
            }
        }

        /*
        Hacemos rebuild en caso que auxiliar.size() supere el log2(data.size())
        */

        if (aux_size > log2(data_size)){
            rebuild();
        }

    }

    

    bool remove_(const string& key){

        //DATA FILE
        rebuild();

        bool modified = false;

        //Busco con busqueda binaria la posicion del elemento
        Record encontrar;
        strcpy(encontrar.Nombre, key.c_str());

        // Busco con búsqueda binaria el key_pos
        int key_pos = binarySearchPosition(encontrar);//key_pos es la posicion donde se encuentra el key
        if (key_pos == 0){
            throw runtime_error("No se encontró la key");
        }
        else{
            key_pos -= 1; //Pos real del key encontrado
            Record current = readRecord(key_pos, data_file);
            if (current.Nombre != key){
                throw runtime_error("No se encontró la key");
            }

            //Actualizamos el primer elemento encontrado con binary search
            updatePunteroAtPosition(key_pos, -1, this->data_file);

            // Recorrer los elementos antes del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos - 1; i >= 0; i--) {
                current = readRecord(i, data_file);
                Record prev = readRecord(i + 1, data_file);
                if (current.compare(prev) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        updatePunteroAtPosition(i, -1, this->data_file);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }

            // Recorrer los elementos después del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos + 1; i < size(data_file); i++) {
                current = readRecord(i, data_file);
                Record next = readRecord(i - 1, data_file);
                if (current.compare(next) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        updatePunteroAtPosition(i, -1, this->data_file);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }
        }

        return modified;

        /*
         *Para el eliminar
         - Se hace un rebuild ()
         - Solo buscar el elemento por nombre, y cambiar su puntero a -1

         - Tomar en cuenta que se tiene que modificar el rebuild para que si lee un -1 en un registro_datafile,
           que no lo agregue al nuevo datafile
         */
    };

    vector<Record> search(const string& key){

        //DATAFILE

        //Busco con busqueda binaria la posicion del elemento
        Record encontrar;
        strcpy(encontrar.Nombre, key.c_str());

        // Busco con búsqueda binaria el key_pos
        int key_pos = binarySearchPosition(encontrar);
        if (key_pos == 0){
            throw runtime_error("No se encontró la key");
        }
        else {
            key_pos -= 1; //Pos real del key encontrado
            Record current = readRecord(key_pos, data_file);
            if (current.Nombre != key.c_str()){
                throw runtime_error("No se encontró la key");
            }

            vector<Record> result;

            //Agregamos el key_pos encontrado

            result.push_back(current);

            // Recorrer los elementos antes del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos - 1; i >= 0; i--) {
                current = readRecord(i, data_file);
                Record prev = readRecord(i + 1, data_file);
                if (current.compare(prev) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        result.push_back(current);
                    }
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
            return "Data";
        }
    }

    //Metodo para escribir un registro en una pos determinada
    void writeRecordPOS(Record record, int pos, string _file){
        ofstream file(_file, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.seekp(pos * sizeof(Record), ios::beg);//fixed length record
        file.write((char*) &record, sizeof(Record));
        file.close();
    }

    //Metodo para escribir un registro a final de un archivo
    void writeRecordEND(Record record, string _file){
        ofstream file(_file, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.write((char*) &record, sizeof(Record));
        file.close();
    }

    void printRegistros() {
        cout << "Archivo de registros:" << endl;
        fstream registros ("file.bin", ios::binary | ios::in | ios::out);
        Record r;
        registros.seekg(0, ios::beg);
        for (int i = 0; i < 8; ++i) {
            registros.read((char*) &r, r.size());
            r.print();
        }
    }

    void printSortedNodes() {
        cout << "Archivo ordenado:" << endl;
        fstream ordenado ("datos.dat", ios::binary | ios::in | ios::out);
        Nodo<int> nodo;
        ordenado.seekg(0, ios::beg);
        for (int i = 0; i < 8; ++i) {
            ordenado.read((char*) &nodo, nodo.size());
            nodo.print();
        }
    }

};

#endif