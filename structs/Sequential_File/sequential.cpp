#include <iostream>
#include <math.h>
#include <fstream>
#include <cstring>
#include <string.h>
//#include "dataset_bin/binary_conversor.h"
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

struct RecordSeq{
    char key[40];
    long pointer_value;

    int next; //puntero al siguiente
    char file_next; //inicial del archivo al que apunta

    int size(){
        return 40+sizeof(long)+4+sizeof(char);
    }

    void print(){
        cout<<"key: "<<key<<" | ";
        cout<<"pointer: "<<pointer_value<<" | ";
        cout<<"next: "<<next<<" "<<file_next<<endl;
    }

};


class SequentialFile{
    string filename;
    string auxiliar;
    //string header_file; //para guardar info de la cabecera
    fstream file;
    fstream aux;

    //para saber donde se ubica la cabecera
    int header;
    char file_header;

public:
    bool add(RecordSeq& record){
        file.open(filename,ios::binary|ios::in|ios::out);
        aux.open(auxiliar, ios::binary|ios::in|ios::out);
        if (!file.good() || !aux.good()) return false; // no se puede abrir, tampoco añadir

        file.seekg(0,ios::end); aux.seekg(0,ios::end); 
        cout<<"file.tellg(): "<<file.tellg()<<"   |    aux.tellg(): "<<aux.tellg()<<endl;
        // primera insercion
        if (file.tellg()==0 && aux.tellg()==0){ //caso en el que esta vacio
            header = 0; file_header = 'a'; //se inserta en auxiliar
            record.next = -1; 
            aux.write((char*)&record, record.size());
            file.close(); aux.close();
            return true;
        }

        //rebuild(); //se llama a funcion

        //analiza record previo en ambos archivos
        int pos_File = prevFile(record.key);
        cout<<"prevFile: "<<pos_File;
        int pos_Aux = prevAux(record.key);
        cout<<" | prevAux: "<<pos_Aux<<endl;

        RecordSeq record1;

        //compara luego de cual debe ir
        if (pos_File==-1 && pos_Aux==-1){  //no hay elementos menores
            aux.seekg(0,ios::end);
            header = aux.tellg(); file_header = 'a';

            record.next = -1; aux.write((char*)&record, record.size());
        } else if(pos_File==-1){ //en archivo principal no hay elemento previo
            //se añade al final de aux y luego del ultimo registro
            aux.seekg(record.size(),ios::end);
            aux.read((char*)&record1, record1.size());
            aux.seekg(0,ios::end);
            record1.next = aux.tellg(); record1.file_next = 'a'; //apunta al final
            aux.seekg(0,ios::end); record.next = -1;
            aux.write((char*)&record,record.size());
        } else if(pos_Aux==-1){ //no se encontró registro previo en Auxiliar
            file.seekg(pos_File,ios::beg); // prev en Filename apuntará a nuevo record (en auxiliar)
            file.read((char*)&record1, record1.size()); aux.seekg(0,ios::end);
            record.next = record1.next; record.file_next = record1.file_next; //new record toma datos de su prev
            record1.next = aux.tellg(); record1.file_next = 'a';
            file.seekp(pos_File,ios::beg);
            file.write((char*)&record1, record1.size());

            //se añade record en auxiliar
            aux.seekp(0,ios::end);
            aux.write((char*)&record,record.size());
        } else{ //tanto aux como file tienen un prev para record
            //debe compararse ambos prev y evaluar cual es mas cercano al nuevo record
            file.seekg(pos_File,ios::beg);
            aux.seekg(pos_Aux,ios::beg);

            RecordSeq record2;
            file.read((char*)&record1, record2.size());
            aux.read((char*)&record2, record2.size());

            aux.seekp(0,ios::end);
            int total_bytes = aux.tellp();

            if (record1.key>record2.key){ //caso en que se añade luego de record1
                record.next = record1.next; record.file_next = record1.file_next;
                record1.next = total_bytes;
                file.seekp(pos_File,ios::beg);
                file.write((char*)&record1, record1.size());
                aux.seekp(0,ios::end); //escribe record
                aux.write((char*)&record, record.size());

            } else{ //caso en que se añade luego de record2
                record.next = record2.next; record.file_next = record2.file_next;
                record2.next = total_bytes;
                aux.seekp(pos_Aux, ios::beg);
                file.write((char*)&record2, record2.size());
                aux.seekp(0,ios::end); //escribe record
                aux.write((char*)&record, record.size());
            }
        }

        file.close(); aux.close();
    }

private:
    int searchFile(int pos, string key){ //busqueda logaritmica en filename
        //se lee registro
        RecordSeq record;
        file.read((char*)&record, record.size());

        //se posiciona en archivo
        if (key==record.key && record.next!=-2){
            return pos;
        } else if(key<record.key){ //enviarlo a la izquierda
            return searchFile(pos/2,key);
        } else if (key>record.key){ //enviarlo a la derecha
            return searchFile(pos+pos/2, key);
        } else{
            return -1; // si no se encuentra, retorna -1
        }
    }

    int searchAuxiliar(string key){ //busqueda lineal
        // (termina siendo log:  k=lgn)
        RecordSeq record;
        aux.seekg(0,ios::end);
        int total_bytes = aux.tellg();

        int pos = 0; //para iterar en registros

        while (total_bytes!=pos){
            aux.seekg(pos,ios::beg); //posiciona en registro
            aux.read((char*)&record,record.size());
            if (key==record.key && record.next!=-2) return pos;
        }
        return -1;
    }

public:
    SequentialFile(){}

    SequentialFile(string filename,string atributo){
        this->filename = atributo + filename + ".bin";
        this->auxiliar = atributo + "aux.bin";
        this->header = -1;
        this->file_header = 'a';
    
        crear_archivo(this->filename);
        crear_archivo(this->auxiliar);
    }

    void readSeq(){
        cout<<"Imprimiendo sequential: ";
        file.open(filename, ios::binary|ios::in);
        file.seekg(0,ios::end);
        int total = file.tellg();
        int pos = 0;
        while (pos<total){
            RecordSeq record;
            file.seekg(pos,ios::beg);
            file.read((char*)&record, record.size());
            record.print();
            pos+= record.size();
        }

        file.close();
    }

    void readAux(){
        cout<<"Imprimiendo auxiliar: ";
        file.open(auxiliar, ios::binary |ios::in);
        file.seekg(0,ios::end);
        int total = file.tellg();
        cout<<"total: "<<total<<endl;
        int pos = 0;
        while (pos<total){
            RecordSeq record;
            file.seekg(pos,ios::beg);
            file.read((char*)&record, record.size());
            record.print();
            pos+= record.size();
        }

        file.close();
    }

    bool search(string key){
        file.open(filename, ios::binary);
        if (!file.good()) return false;
        int expr1 = searchFile(0,key); //busqueda binaria
        if (expr1) {
            file.close();
            return expr1;
        }

        aux.open(auxiliar,ios::binary);
        if (!aux.good()) return false;
        int expr2 = searchAuxiliar(key); // O(lgn) - busqueda lineal en HeapFile
        if (expr2) {
            aux.close();
            return expr2;
        }

        return false;
    }


    void rebuild(){
        //calculamos cantidad de registros en filename
        file.seekg(0,ios::end);
        RecordSeq recordseq;
        int n = file.tellg()/recordseq.size();

        //calculamos cantidad de registros en auxiliar
        aux.seekg(0,ios::end);
        int k = file.tellg()/recordseq.size();

        //cantidad que debería haber
        int max_records = floor(log2(n));

        string new_filename = filename+"1";
        int count = 0;
        fstream new_file(new_filename, ios::binary|ios::in|ios::out);
        if (k==max_records){ //se reconstruye
            RecordSeq record; record.next = 0;
            int pos = header; char archivo = file_header;
            while (record.next!=-1){ //sale al escribir el ultimo
                if (archivo=='a'){ //en auxiliar
                    aux.seekg(pos,ios::beg);
                    aux.read((char*)&record, record.size());

                } else{ //en filename
                    file.seekg(pos,ios::beg);
                    aux.read((char*)&record, record.size());

                }
                count++;
                if (count==max_records+n) record.next==-1;
                else record.next = count*record.size();
                new_file.write((char*)&record, record.size());
            }
        } else return; //se puede seguir insertando en auxiliar
        header = 1; file_header = 'f';
        file.close(); filename = new_filename;
        //file.open(filename, ios::binary|ios::in|ios::out);
    }

    bool remove(string key){
        file.open(filename,ios::binary|ios::in|ios::out);
        aux.open(auxiliar,ios::binary|ios::in|ios::out);

        if (!file.good() && !aux.good()) return false;

        int posFile = searchFile(0,key);
        int posAux = searchAuxiliar(key);

        if (posFile==-1 && posAux == -1){
            file.close(); aux.close();
            return false;
        }

        RecordSeq record;
        //se eliminan todos (hasta repetidos)
        while (posFile!=-1){
            file.seekg(posFile,ios::beg);
            file.read((char*)&record, record.size());
            record.next = -2;

            file.seekp(posFile,ios::beg);
            file.write((char*)&record, record.size());
            posFile = searchFile(posFile, key);
        }

        while (posAux!=-1){
            aux.seekg(posAux,ios::beg);
            aux.read((char*)&record, record.size());
            record.next = -2;

            aux.seekp(posAux,ios::beg);
            aux.write((char*)&record, record.size());
            posAux = searchAuxiliar(key);
        }
        file.close(); aux.close();
    }


    int prevFile(string key){//retorna posicion previa a donde se debe incluir el key

        file.seekg(0, ios::end);
        int total_bytes = file.tellg();

        if (total_bytes==0) return -1;        
        //se necesita 2 record's para comparar
        RecordSeq record1;
        RecordSeq record2;

        int pos1 = 0; //para posicionarse
        int pos2 = 0;
        while (true){ //busqueda logaritmica
            if (pos1>=total_bytes) return -1;
            file.seekg(pos1,ios::beg);
            file.read((char*)&record1, record1.size()); //lee record1
            if (key<record1.key){ //caso en que key es menor (itera)
                pos1 /= 2;
            }
            if (record1.next==-1) return pos1; //retorna posicion actual (en caso no haya mas elementos)

            pos2 = record1.next; //buscamos posicion siguiente (en caso la haya)
            file.seekg(pos2); // leemos registros
            file.read((char*)&record2, record1.size()); //lee record2
            if (record1.key<=key && record2.key>=key) return pos1; // va entre estos dos registros y retorna posicion prev
            else if (record1.key>key){ //se debe ir por derecha 
                pos1 += (pos1/2); // busca por la derecha
            } else{ // caso en el que es menor, va por la izquierda
                pos1 -= (pos1/2); // busca por la derecha
            }
        }
    }

    int prevAux(string key){ //retorna posicion a registro previo en Auxiliar
        RecordSeq record;
        aux.seekg(0,ios::end);
        int size = aux.tellg(); //cuenta bytes
        //if (size<record.size()*10) return -1;
        int pos = 0; int result=-1;
        //char current_prev[40] = "a";

        
        while (size>pos){
            aux.seekg(pos,ios::beg); //se posiciona
            aux.read((char*)&record, record.size());
            if (record.key<=key){
                //si el record encontrado es menor= que el key que buscamos
                // y mayor que el key que tenemos (hasta el momento) como previo
                ///current_prev = record.key;
                //strcpy(current_prev, record.key);
                pos+=record.size();
                result = pos;
            }
        }
        //cout<<"en auxiliar se puede insertar en la posicion: "<<result<<endl;
        return result; //luego de recorrer todo, retorna posicion
    }

    public:
    void insert(string word, long pointer){
        RecordSeq record;
        strcpy(record.key,word.c_str());
        record.next = -1;
        record.pointer_value = pointer;
        cout<<"Añadiendo: "; record.print();
        cout<<add(record);
        cout<<"Añadido"<<endl;
    }
};

int main(){
    SequentialFile seq("filename","nombre");
    seq.insert("abcd",234);
    seq.insert("bcd",23456);
    //seq.insert("aa",23456);
    seq.readSeq();
    seq.readAux();
    return 0;
}