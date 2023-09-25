#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <cmath>
#include "../../dataset_bin/binary_conversor.h"

using namespace std;

void crear_archivo(const string& nombre) {
    ifstream archivo(nombre.c_str());
    if (archivo.good()) { //si existe
        archivo.close(); //cerrarlo
        return; //ya no es necesario crear
    }
    ofstream file(nombre, ios::binary);
    file.close();
}

class SeqRecord {
private:

public:
    char atr[40];
    //string atr;
    float local_pointer;
    long pointer_value;

    int size(){
        return 40+4+sizeof(long);
    }
    void setData(string atributo, float _local_pointer, long _pointer_value){
        local_pointer = _local_pointer;
        pointer_value = _pointer_value;
        atributo.copy(atr, sizeof(atr)-1);
        atr[atributo.length()-1] = '\0';
    }
    void showData(){
        //solo un atributo
        //cout << atr << " - " << Atrib2 << " - " << local_pointer << endl;
        cout << atr << " - " << local_pointer << endl;
    }

    // Agregar un método para comparar el nombre de dos records
    int compare(const SeqRecord& otro) const { // const para que no se modifique el objeto
        return strcmp(atr, otro.atr);//strcmp devuelve 0 si son iguales, 1 si el primero es mayor al segundo, -1 si el primero es menor al segundo
    }

};

class SequentialFile{
private:
    string document,atr;

    string data_file;//guara los registros
    string aux_file;//guarda los punteros

    int data_size;//cantidad de registros en data.bin
    int aux_size;//cantidad de registros en auxiliar.bin

public:
    SequentialFile(string _document, string _atr){
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
        crear_archivo(this->aux_file);
        crear_archivo(this->data_file);

        data_size = size(this->data_file);
        aux_size = size(this->aux_file);

    };

    //Metodo que imprime todos los archivos de data y auxiliar
    void print(){

        cout << "size data y aux: " << data_size << " " << aux_size <<endl;

        SequentialFile file2(document,atr);
        vector<SeqRecord> records = file2.scanAll(atr+"data.bin");
        for(SeqRecord r : records){
            r.showData();
        }

        cout << endl << "-----------------------" <<endl;

        //Lectura
        SequentialFile file5(document,atr);
        vector<SeqRecord> records2 = file5.scanAll(atr+"auxiliar.bin");
        for(SeqRecord r : records2){
            r.showData();
        }

    }

    void insert(string atrib, long pointer){
        SeqRecord record;
        //record.atr = atrib.c_str();
        atrib.copy(record.atr, record.size()-1);
        record.atr[atrib.length()-1] = '\0';
        record.pointer_value = pointer;
        record.local_pointer = 1.0;
        add(record);
    }

    void add(SeqRecord record){
        //Ubicamos el elemento anterior a record
        int insert_pos = binarySearchPosition(record) - 1; //binarySearchPOS me devuelve la pos donde debo insertar el elemento, por eso el -1
        SeqRecord prevRecord = readRecord(insert_pos, this->data_file);

        //Si prevRecord apunta a un elemento de la misma Data
        /*
            add(Manuel)
            ======================
            Data:
            "     " -> 0.1
            Asucena -> 0.2  <- prevRecord
            Ortega  -> 0
            ======================
            Pedro   -> 0.3
        */
        if(analyzeFloat(prevRecord.local_pointer) == "Data"){
            //Agregamos el record al aux y hacemos el cambiaso de punteros entre prevRecord y record
            record.local_pointer = prevRecord.local_pointer; //record -> prevRecord.local_pointer
            writeRecordEND(record, this->aux_file);
            updatePunteroAtPosition(insert_pos, aux_size, this->data_file); // prevRecord -> pos(record)

            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
            aux_size += 1;
        }

        //Si prevRecord apunta a un elemento perteneciente a Auxiliar
        /*
            add(Manuel)
            ======================
            Data:
            "     " -> 0.1
            Asucena -> 0   <- prevRecord
            Ortega  -> 0.3
            ======================
            Auxiliar:
            Belen   ->  0.2 <- Y
        */
        else if(analyzeFloat(prevRecord.local_pointer) == "Auxiliar"){
            //Si apunta a un auxiliar primero me tengo que ubicar en auxiliarfile[prevRecord.local_pointer]

            //Y_pos = prevRecord.local_pointer;
            SeqRecord auxPrevRecord = readRecord(prevRecord.local_pointer, this->aux_file);

            //Caso particular (Todavia no nos hemos adentrado al 100% en aux_file, estamos en el limbo)
            /*
                add(Baldor)
                ======================
                Data:
                "     " -> 0.1
                Asucena -> 0   <- prevRecord      (prevRecord todavia esta en data file, mientras que auxPrevRecord ya esta exclusivamente en el auxiliar file)
                Ortega  -> 0.3
                ======================
                Auxiliar:
                Belen   ->  0.2 <- auxPrevRecord    (Baldor va antes que que Belen)
             */
            int cmp = record.compare(auxPrevRecord);
            if (cmp <= 0){ //Si record va antes que auxPrevRecord o es igual a auxPrevRecord
                //Agregamos el record al aux y hacemos el cambiaso de punteros entre prevRecord y record
                record.local_pointer = prevRecord.local_pointer; //record -> prevRecord.local_pointer
                writeRecordEND(record, this->aux_file);
                updatePunteroAtPosition(insert_pos, aux_size, this->data_file); // prevRecord -> pos(record)

                //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                aux_size += 1;
            }

            //Si record va despues que auxPrevRecord
            if (cmp > 0){

                //Caso particular (Todavia no nos hemos adentrado al 100% en aux_file, estamos en el limbo)
                /*
                    add(Boris)
                    ======================
                    Data:
                    "     " -> 0.1
                    Asucena -> 0   <- prevRecord      (prevRecord todavia esta en data file, mientras que auxPrevRecord ya esta exclusivamente en el auxiliar file)
                    Ortega  -> 0.3
                    ======================
                    Auxiliar:
                    Belen   ->  0.2 <- auxPrevRecord    (Boris va despues que que Belen, pero despues de Belen no hay nada mas)
                 */
                if (analyzeFloat(auxPrevRecord.local_pointer) == "Data"){
                    record.local_pointer = auxPrevRecord.local_pointer;
                    writeRecordEND(record, this->aux_file);
                    updatePunteroAtPosition(prevRecord.local_pointer, aux_size, this->aux_file); // prevRecord -> pos(record)
                    //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                    aux_size += 1;
                }

                //Ahora si tanto prevRecord como auxPrevRecord pertenecen a auxiliar file
                else {
                    bool posicionado = false;
                    int cmp2;
                    while(!posicionado){

                        insert_pos = prevRecord.local_pointer; //insert_pos = Y_pos
                        prevRecord = readRecord(insert_pos, this->aux_file);

                        //Y_pos = prevRecord.local_pointer
                        auxPrevRecord = readRecord(prevRecord.local_pointer, this->aux_file);

                        /*
                            add(Conchita)
                            ======================
                            Data:
                            "     " -> 0.1
                            Asucena -> 0   <- prevRecord
                            Ortega  -> 0.3
                            ======================
                            Auxiliar:
                            Belen   ->  1   <- auxPrevRecord
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
                            Belen   ->  1   <- prevRecord
                            Boris   ->  0.2 <- auxPrevRecord

                         */
                        cmp2 = record.compare(auxPrevRecord);

                        //Si record va antes que auxPrevRecord
                        if (cmp2 <= 0){
                            //Agregamos el record al aux y hacemos el cambiaso de punteros entre prevRecord y record
                            record.local_pointer = prevRecord.local_pointer; //record -> prevRecord.local_pointer
                            writeRecordEND(record, this->aux_file);
                            updatePunteroAtPosition(insert_pos, aux_size, this->aux_file); // prevRecord -> pos(record)

                            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                            aux_size += 1;

                            posicionado = true;
                        }

                        //Si record va despues que auxPrevRecord, y este auxPrevRecord apunta a un elemento perteneciente a data.file (osea que ya no apunta a ningun otro elemento)
                        if (cmp2 > 0 && analyzeFloat(auxPrevRecord.local_pointer) == "Data"){

                            record.local_pointer = auxPrevRecord.local_pointer;
                            writeRecordEND(record, this->aux_file);
                            updatePunteroAtPosition(prevRecord.local_pointer/*Y_pos*/, aux_size, this->aux_file);

                            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                            aux_size += 1;

                            posicionado = true;
                        }

                        /*

                         En caso record vaya despues que auxPrevRecord pero auxPrevRecord apunta a otro elemento perteneciente a auxixiliar.bin
                         (osea no se ha llegao al final)

                         Se repite el while y con ello
                         -  prevRecord pasa a ser auxPrevRecord(1)
                         -  auxPrevRecord pasa a ser el auxPrevRecord(1).local_pointer
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
        SeqRecord encontrar;
        strcpy(encontrar.atr, key.c_str());

        // Busco con búsqueda binaria el key_pos
        int key_pos = binarySearchPosition(encontrar);//key_pos es la posicion donde se encuentra el key
        if (key_pos == 0){
            throw runtime_error("No se encontró la key");
        }
        else{
            key_pos -= 1; //Pos real del key encontrado
            SeqRecord current = readRecord(key_pos, data_file);
            if (current.atr != key){
                throw runtime_error("No se encontró la key");
            }

            //Actualizamos el primer elemento encontrado con binary search
            updatePunteroAtPosition(key_pos, -1, this->data_file);

            // Recorrer los elementos antes del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos - 1; i >= 0; i--) {
                current = readRecord(i, data_file);
                SeqRecord prev = readRecord(i + 1, data_file);
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
                SeqRecord next = readRecord(i - 1, data_file);
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

    vector<SeqRecord> search(const string& key){

        //DATAFILE

        //Busco con busqueda binaria la posicion del elemento
        SeqRecord encontrar;
        strcpy(encontrar.atr, key.c_str());

        // Busco con búsqueda binaria el key_pos
        int key_pos = binarySearchPosition(encontrar);
        if (key_pos == 0){
            throw runtime_error("No se encontró la key");
        }
        else {
            key_pos -= 1; //Pos real del key encontrado
            SeqRecord current = readRecord(key_pos, data_file);
            if (current.atr != key.c_str()){
                throw runtime_error("No se encontró la key");
            }

            vector<SeqRecord> result;

            //Agregamos el key_pos encontrado

            result.push_back(current);

            // Recorrer los elementos antes del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos - 1; i >= 0; i--) {
                current = readRecord(i, data_file);
                SeqRecord prev = readRecord(i + 1, data_file);
                if (current.compare(prev) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        result.push_back(current);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }

            // Recorrer los elementos después del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos + 1; i < size(data_file); i++) {
                current = readRecord(i, data_file);
                SeqRecord next = readRecord(i - 1, data_file);
                if (current.compare(next) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        result.push_back(current);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }


            //AUXILIAR FILE
            ifstream file2(this->aux_file, ios::binary);
            if (!file2.is_open()) {
                throw ("No se pudo abrir el archivo de datos.");
            }

            int totalRecords = size(this->aux_file);

            for (int i = 0; i < totalRecords; i++) {
                SeqRecord record = readRecord(i, this->aux_file);

                // Comparar el nombre del registro con la clave proporcionada (insensible a mayúsculas y minúsculas)
                if (_stricmp(record.atr, key.c_str()) == 0 && getPunteroAtPosition(i, this->aux_file) != -1) {
                    result.push_back(record);
                }
            }

            file2.close();

            return result;

        }


    }

    vector<SeqRecord> rangeSearch(const string& begin, const string& end) {
        rebuild();

        //Busco con busqueda binaria la posicion del begin
        SeqRecord encontrar_begin;
        strcpy(encontrar_begin.atr, begin.c_str());
        int limite_inferior = binarySearchPosition(encontrar_begin);

        if (limite_inferior == 0){
            throw runtime_error("No se encontró el límite inferior");
        }
        else {
            limite_inferior -= 1;
            SeqRecord inf = readRecord(limite_inferior, this->data_file);
            if (inf.atr != begin){
                throw runtime_error("No se encontró la key inferior");
            }
        }

        //Busco con busqueda binaria la posicion del end
        SeqRecord encontrar_end;
        strcpy(encontrar_end.atr, end.c_str());
        int limite_superior = binarySearchPosition(encontrar_end);
        if (limite_superior == 0){
            throw runtime_error("No se encontró el límite superior");
        }
        else {
            limite_superior -= 1;
            SeqRecord sup = readRecord(limite_superior, this->data_file);
            if (sup.atr != end){
                throw runtime_error("No se encontró la key superior");
            }
        }


        vector<SeqRecord> result;

        /*// Agregar los elementos que están entre el límite inferior y el superior (Si apuntan a -1 no se agregan)
        for (int i = limite_inferior; i <= limite_superior; i++) {
            SeqRecord record = readRecord(i, data_file);
            if (getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(record);
            }
        }
        */

        // Agregar los elementos que están entre el límite inferior y el superior (Si apuntan a -1 no se agregan)
        if (limite_inferior <= limite_superior);
        else swap(limite_inferior, limite_superior);// En caso de que begin sea mayor que end, intercambiamos los límites

        for (int i = limite_inferior; i <= limite_superior; i++) {
            SeqRecord record = readRecord(i, data_file);
            if (getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(record);
            }
        }


        // Recorrer los elementos antes del límite inferior para ver si se repiten y agregarlos (si apuntan a -1 no se agregan)
        for (int i = limite_inferior - 1; i >= 0; i--) {
            SeqRecord current = readRecord(i, data_file);
            SeqRecord prev = readRecord(i + 1, data_file);
            if (current.compare(prev) == 0 && getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(current);
            } else if (current.compare(prev) != 0){
                break; // Si ya no se repiten o apuntan a -1, detener la búsqueda
            }
        }

        // Recorrer los elementos después del límite superior para ver si se repiten y agregarlos (si apuntan a -1 no se agregan)
        for (int i = limite_superior + 1; i < size(data_file); i++) {
            SeqRecord current = readRecord(i, data_file);
            SeqRecord next = readRecord(i - 1, data_file);
            if (current.compare(next) == 0 && getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(current);
            } else if (current.compare(next) != 0){
                break; // Si ya no se repiten o apuntan a -1, detener la búsqueda
            }
        }

        return result;

    }

private:
    //Metodo para reconstruir
    void rebuild() {

        ifstream dataFileStream(data_file, ios::binary);
        if (!dataFileStream.is_open()) {
            cerr << "No se pudo abrir el archivo de datos: " << data_file << endl;
            return;
        }

        ofstream newFileStream("nuevo_data_file.bin", ios::binary);
        if (!newFileStream.is_open()) {
            cerr << "No se pudo crear el nuevo archivo de datos." << endl;
            return;
        }

        SeqRecord X;
        float puntero = -0.9;
        float puntero_temporal;

        int cantidad_eliminados = 0;

        // Iterar a través de los registros en el archivo de datos
        while (dataFileStream.read(reinterpret_cast<char*>(&X), X.size())) {

            if (X.local_pointer == -1){
                cantidad_eliminados += 1;
            }

            //Nos saltamos los records que apunten a -1, ya que estos estan eliminados
            if (X.local_pointer != -1){
                //Agregamos X al nuevo data_file
                puntero += 1;
                puntero_temporal = X.local_pointer;
                X.local_pointer = puntero;
                writeRecordEND(X,"nuevo_data_file.bin");

                //En caso X apunte a un registro perteneciente al espacio auxiliar
                if (analyzeFloat(puntero_temporal ) == "Auxiliar"){
                    //Sabemos que X apunta a Y
                    //Nos ubicamos en Y
                    SeqRecord Y;
                    while (analyzeFloat(puntero_temporal) != "Data" ){
                        puntero += 1;
                        Y = readRecord(puntero_temporal,this->aux_file);
                        puntero_temporal = Y.local_pointer;
                        Y.local_pointer = puntero;
                        writeRecordEND(Y,"nuevo_data_file.bin");
                    };
                }
            }
        }

        dataFileStream.close();
        newFileStream.close();

        // Reemplazar el archivo original con el nuevo archivo
        if (remove(data_file.c_str()) != 0) {
            cerr << "Error al eliminar el archivo original." << endl;
        }

        if (rename("nuevo_data_file.bin", data_file.c_str()) != 0) {
            cerr << "Error al renombrar el nuevo archivo." << endl;
        }

        clearFile(this->aux_file);

        //Actualizamos la cantidad de elementos que hay en ambos archivos
        data_size += (aux_size - cantidad_eliminados);
        aux_size = 0;

    }

    // Método para limpiar un archivo (borrar su contenido)
    void clearFile(const string& _file) {
        std::ofstream file(_file, std::ofstream::trunc);
        if (!file.is_open()) {
            throw ("No se pudo abrir el archivo para limpiar.");
        }
        file.close();
    }

    //Binary Search Position me devuelve la posicion donde tiene que ser insertado cierto registro
    //Si en data_file existe A (ubicado en pos 0), y quiero insertar B, entonces binary me devuelve pos 1 (pos 0 + 1), ya que A esta despues que B.
    int binarySearchPosition(const SeqRecord& nuevoRecord) {

        /*
        En este código, el método binarySearchPosition utiliza la búsqueda binaria para encontrar
        la posición de inserción del nuevo registro nuevoRecord en orden alfabético. Devolverá la
        posición en la que debería insertarse el nuevo registro en función del nombre. Si el
        registro ya existe en la lista, devolverá la posición actual del registro encontrado.
        */
        ifstream file(this->data_file, ios::binary);
        if (!file.is_open()) throw ("No se pudo abrir el archivo");

        int left = 0;
        int right = size(this->data_file) - 1;

        while (left <= right) {
            int middle = left + (right - left) / 2;
            SeqRecord record = readRecord(middle, this->data_file);//Obtener el registro en la mitad

            // Comparar el nombre del registro con el nombre del nuevo record
            int cmp = record.compare(nuevoRecord);

            if (cmp == 0) {//Si el nombre del registro es igual al nombre del nuevo record

                file.close();
                middle += 1; //Si encuentra un nombre igual que me el nuevo registro se ponga despues

                //Para la eliminacion si B se inserta despues que A, tenemos que verificar que este no apunte a un -1
                //En caso a apunte a un menos -1, vamos a ir retrocediendo de pos en data_file hasta encontrar el mas registro 
                //superior alfabeticamente a B que no este eliminado.
                while (getPunteroAtPosition(middle-1,this->data_file) == -1){
                    middle -= 1;
                    //Si esta A y quiero insertar B. Entonces el metodo me retornada left, ya que A estaba en left -1
                }

                return middle; // Encontrado, devuelve la posición actual
            } else if (cmp < 0) {
                left = middle + 1; // El nombre está en la mitad derecha
            } else {
                right = middle - 1; // El nombre está en la mitad izquierda
            }
        }

        file.close();

        // Si no se encuentra un registro igual, devolver la posición en la que debería insertarse

        //Para la eliminacion si B se inserta despues que A, tenemos que verificar que este no apunte a un -1
        //En caso a apunte a un menos -1, vamos a ir retrocediendo de pos en data_file hasta encontrar el mas registro superior alfabeticamente a B que no este eliminado.
        while (getPunteroAtPosition(left-1,this->data_file) == -1){
            left -= 1;

            //Si esta A y quiero insertar B. Entonces el metodo me retornada left, ya que A estaba en left -1
        }
        return left;
    }

    //Metodo para hallar la cantidad de registros en un archivo especifico
    int size(string _file){
        ifstream file(_file, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.seekg(0, ios::end);//ubicar cursos al final del archivo
        long total_bytes = file.tellg();//cantidad de bytes del archivo
        file.close();
        SeqRecord X;
        return total_bytes / X.size();
    }

    //Metodo para determinar si un registro punta otro perteneciente a data file o a aux_file
    string analyzeFloat(float number) {
        if (std::floor(number) == number) {
            return "Auxiliar";
        } else {
            return "Data";
        }
    }

    //Metodo para escribir un registro en una pos determinada
    void writeRecordPOS(SeqRecord record, int pos, string _file){
        ofstream file(_file, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.seekp(pos * record.size(), ios::beg);//fixed length record
        file.write((char*) &record, record.size());
        file.close();
    }

    //Metodo para escribir un registro a final de un archivo
    void writeRecordEND(SeqRecord record, string _file){
        ofstream file(_file, ios::app | ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.write((char*) &record, sizeof(SeqRecord));
        file.close();
    }

    //Metodo para leer un registro en una pos especifica
    SeqRecord readRecord(int pos, string _file){
        ifstream file(_file, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        SeqRecord record;
        file.seekg(pos * sizeof(SeqRecord), ios::beg);//fixed length record
        file.read((char*) &record, sizeof(SeqRecord));//leer el registro en la posicion
        file.close();
        return record;
    }

    //Metodo que te devuelve el puntero de un registro en una pos especifica
    float getPunteroAtPosition(int pos, string _file) {
        ifstream file(_file, ios::binary);
        if (!file.is_open()) throw ("No se pudo abrir el archivo auxiliar");

        SeqRecord record;
        file.seekg(pos * sizeof(SeqRecord), ios::beg); // Ir a la posición especificada
        file.read((char*)&record, sizeof(SeqRecord)); // Leer el registro en la posición
        file.close();

        return record.local_pointer; // Devolver el valor del puntero del registro
    }

    //Metodo que te modifica el puntero de un registro en una pos especifica
    void updatePunteroAtPosition(int pos, float newPunteroValue, string _file) {
        // Open the data file in read-write mode
        fstream file(_file, ios::in | ios::out | ios::binary);
        if (!file.is_open()) throw ("No se pudo abrir el archivo de datos");

        // Seek to the position specified
        file.seekg(pos * sizeof(SeqRecord), ios::beg);

        // Read the record at that position
        SeqRecord record;
        file.read((char*)&record, sizeof(SeqRecord));

        // Update the local_pointer field
        record.local_pointer = newPunteroValue;

        // Seek back to the same position
        file.seekp(pos * sizeof(SeqRecord), ios::beg);

        // Write the updated record back to the data file
        file.write((char*)&record, sizeof(SeqRecord));

        // Close the file
        file.close();
    }

    //Metodo que retorna todos los elementos de un archivo
    vector<SeqRecord> scanAll(string _file){
        ifstream file(_file, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        vector<SeqRecord> records;
        SeqRecord record;

        while(file.peek() != EOF){
            record = SeqRecord();
            file.read((char*) &record, sizeof(SeqRecord));
            records.push_back(record);
        }
        file.close();

        return records;
    }

    void load_file(){
        ifstream fileaux;
        fileaux.open(document,ios::binary | ios::in);
        fileaux.seekg(0,ios::end);
        fileaux.close();
        int x = fileaux.tellg(), pos = 0;
        while(pos < x){
            SeqRecord record = readRecord(pos,document);
            add(record);
            pos +=  sizeof(SeqRecord);
        }
    }

    public:
    void buildFromFile(string sourceName, int atributo_col){
        ifstream source(sourceName,ios::binary | ios::in);
        string msg = "No existe archivo";
        if (!source.is_open()) throw runtime_error(msg);

        long bytes = 0;
        source.seekg(0, ios::end);
        int max_bytes = source.tellg();
        source.seekg(0, ios::beg);
        while(bytes < max_bytes){

            //SeqRecord record = readRecord(bytes,document);
            //creamos un registro tipo SeqRecord
            SeqRecord record;

            add(record);
            record.showData();
            bytes +=  record.size();
        }
        source.close();
    }

};

