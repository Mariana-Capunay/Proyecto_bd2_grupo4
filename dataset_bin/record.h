#ifndef PROYECTO_1_RECORD_H
#define PROYECTO_1_RECORD_H
#include <iostream>

struct Record {
    int key= {};                 // int (dni o codigo)
    char atrib1[40] = {};        // char de 40 (nombre completo o nombre producto)
    int atrib2 = {};             // int (edad o stock)
    char atrib3[25] = {};        // char de 25 (ciudad o categoria)
    float atrib4 = {};           // float (sueldo, precio)
    bool removed   ;            // bool (removed)

    //friend ofstream& operator<<(ofstream& salida, Record r);
    void print(){
        //std::cout<<key;
        //std::cout<<", ";
        //std::cout<<atrib1;
        //std::cout<<", ";
        //std::cout<<atrib2<<", ";
        //std::cout<<atrib3<<", ";
        //std::cout<<atrib4<<"\n";
    }
    int size(){
        return sizeof(int)*2 + 40 + 25 + 4 + sizeof(bool);
    }
};

/*
ofstream& operator<<(ofstream& salida, Record r){
    salida<<r.key<<", ";
    salida<<r.atrib1<<", ";
    salida<<r.atrib2<<", ";
    salida<<r.atrib3<<", ";
    salida<<r.atrib4<<"\n";
    return salida; // Devolver el objeto ofstream
}
*/

/*
ofstream& operator<<(ofstream& salida, Record& r){
    salida << r.key << ", ";

    // Tratar r.atrib1 como una cadena de caracteres null-terminated
    salida.write(r.atrib1, sizeof(r.atrib1) - 1);
    salida << ", ";

    salida << r.atrib2 << ", ";

    // Tratar r.atrib3 como una cadena de caracteres null-terminated
    salida.write(r.atrib3, sizeof(r.atrib3) - 1);
    salida << ", ";

    salida << r.atrib4 << "\n";

    return salida; // Devolver el objeto ofstream
}
*/

#endif //PROYECTO_1_RECORD_H
