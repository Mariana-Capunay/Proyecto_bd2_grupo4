#ifndef PROYECTO_1_BINARY_CONVERSOR_H
#define PROYECTO_1_BINARY_CONVERSOR_H
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "record.h"

using namespace std;

void crear_archivo(string nombre){
    ifstream archivo(nombre.c_str());
    if (archivo.good()){ //si existe
        return; //ya no es necesario crear
    }
    ofstream file(nombre, ios::binary);
//cout<<"creando archivo "<<nombre<<endl;
    file.close();
}


string conversor(string file_route, string& atr_1, string& atr_2, string& atr_3, string& atr_4, string& atr_5){ //retorna nombre del archivo creado
    ifstream file(file_route);
    string bin_file = "test.bin";
    ofstream bin(bin_file);

    if (!file.is_open() || !bin.is_open()) {
        throw runtime_error("No se pudo abrir el archivo.");
    }
    string linea;

    getline(file, linea); // lee primera linea
    std::string termino;
    std::istringstream streamLinea(linea);

    // Divide la línea en términos separados por comas
    int i = 0;
    while (std::getline(streamLinea, termino,',')){
        switch(i){
            case 0:
                atr_1 = termino;
                i++;
                break;
            case 1:
                atr_2 = termino;
                i++;
                break;
            case 2:
                atr_3 = termino;
                i++;
                break;
            case 3:
                atr_4 = termino;
                i++;
                break;
            default:
                //cout<<termino<<endl;
                atr_5 = termino;
                break;
        }
    }
    //std::getline(streamLinea, termino); // Lee la última columna (sueldo)
    //atr_5 = termino;


    // Lee cada línea del archivo
    i = 0;
    while (std::getline(file, linea)) {
        std::istringstream streamLinea(linea);
        Record record;
        while (std::getline(streamLinea, termino, ',')){
            //cout << "Full line: " << termino << endl;
            switch (i) {
                case 0:
                    record.key = stoi(termino);
                    //cout << termino << " to " << record.key << " |" << endl;
                    //cout << "Szterm: " << sizeof(termino) << "\t" << "Szrec: " << sizeof(record.key)<< endl;
                    i++;
                    break;
                case 1:
                    //strcpy(record.atrib1, termino.c_str());
                    std::strncpy(record.atrib1, termino.c_str(), sizeof(record.atrib1)-1);
                    record.atrib1[sizeof(record.atrib1) - 1] = '\0'; // Asegúrate de agregar el carácter nulo
                    //cout << termino << " to " << record.atrib1 << " |" << endl;
                    //cout << "Szterm: " << sizeof(termino) << "\t" << "Szrec: " << sizeof(record.atrib1)<< endl;
                    i++;
                    //cout << termino << " to " << record.atrib1 << "|" << endl;
                    break;
                case 2:
                    record.atrib2 = stoi(termino);
                    //cout << termino << " to " << record.atrib2 << "|" << endl;
                    i++;
                    break;
                case 3:
                    //strcpy(record.atrib3, termino.c_str());
                    std::strncpy(record.atrib3, termino.c_str(), sizeof(record.atrib3)-1);
                    record.atrib3[sizeof(record.atrib3) - 1] = '\0'; // Asegúrate de agregar el carácter nulo
                    //cout << termino << " to " << record.atrib3 << "|" << endl;
                    i++;
                    break;
                case 4:
                    record.atrib4 = stof(termino);
                    //cout << termino << " to " << record.atrib4 << "|" << endl;
                    i = 0;
                    break;
                default:
                    cout << "end" << endl;
                    break;
            }
            record.removed = false;
            bin.write((char*)&record, sizeof(Record));
        }
        //cout << "Record: "<< record.atrib4 << "\t" << "sz: " << sizeof(record.atrib4);
        //cout << endl;
        record.print();
        //cout<<record.key<<" - "<<record.atrib1<<" - "<<record.atrib2<<endl;

    }

    file.close();
    bin.close();
    return bin_file;
};


#endif //PROYECTO_1_BINARY_CONVERSOR_H
