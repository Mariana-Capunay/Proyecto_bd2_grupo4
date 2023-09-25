#include "binary_conversor.h"

std::string conversor(std::string file_route, std::string& atr_1, std::string& atr_2, std::string& atr_3, std::string& atr_4, std::string& atr_5){ //retorna nombre del archivo creado
    std::ifstream file(file_route);
    std::string bin_file = "test.bin";
    std::ofstream metadata(bin_file, std::ios::binary);

    if (!file.is_open() || !metadata.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo.");
    }
    std::string linea;

    getline(file, linea); // lee primera linea
    std::string termino;
    std::istringstream streamLinea(linea);

    // Divide la línea en términos separados por comas
    int i = 0;
    while (getline(streamLinea, termino,',')){
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
    i = 0; //int c= 0;
    while (std::getline(file, linea)){ //&& c<10) {
        std::istringstream streamLinea(linea);
        Record record; //c++;
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
                    std::cout << "end" << std::endl;
                    break;
            }
            record.removed = false;
        }
        //cout << "Record: "<< record.atrib4 << "\t" << "sz: " << sizeof(record.atrib4);
        //cout << endl;
        metadata.write((char*)&record.key,sizeof(record.key));
        metadata.write((char*)&record.atrib1,sizeof(record.atrib1));
        metadata.write((char*)&record.atrib2,sizeof(record.atrib2));
        metadata.write((char*)&record.atrib3,sizeof(record.atrib3));
        metadata.write((char*)&record.atrib4,sizeof(record.atrib4));
        metadata.write((char*)&record.removed,sizeof(record.removed));
        //metadata.write((char*)&record, sizeof(Record));
        //record.print();
        //cout<<record.key<<" - "<<record.atrib1<<" - "<<record.atrib2<<endl;

    }

    file.close();
    metadata.close();
    return bin_file;
}

Record read_record(std::string file_name, int pos){
    std::ifstream file(file_name, std::ios::binary);
    if (!file.is_open()) {throw std::runtime_error("No se pudo abrir archivo ");}
    Record record;
    file.seekg(0, std::ios::end);
    int dif = file.tellg();
    if ( dif <=pos) {throw std::runtime_error("No existe registro en esta posicion");}
    file.seekg(0,std::ios::beg);
    file.seekg(pos); // los punteros tienen posicion exacta del registro


    file.read((char*)&record.key,sizeof(record.key));
    file.read((char*)&record.atrib1,sizeof(record.atrib1));
    file.read((char*)&record.atrib2,sizeof(record.atrib2));
    file.read((char*)&record.atrib3,sizeof(record.atrib3));
    file.read((char*)&record.atrib4,sizeof(record.atrib4));
    file.read((char*)&record.removed,sizeof(record.removed));

    if (!record.removed){
        record.print();
    }

    file.close();
    return record;
}

bool print_record(std::string file_name, int pos){
    std::ifstream file(file_name, std::ios::binary);
    if (!file.is_open()) {throw std::runtime_error("No se pudo abrir archivo ");}
    Record record;
    file.seekg(0, std::ios::end);
    int dif = file.tellg();
    if ( dif <=pos) {throw std::runtime_error("No existe registro en esta posicion");}
    file.seekg(0,std::ios::beg);
    file.seekg(pos); // los punteros tienen posicion exacta del registro


    file.read((char*)&record.key,sizeof(record.key));
    file.read((char*)&record.atrib1,sizeof(record.atrib1));
    file.read((char*)&record.atrib2,sizeof(record.atrib2));
    file.read((char*)&record.atrib3,sizeof(record.atrib3));
    file.read((char*)&record.atrib4,sizeof(record.atrib4));
    file.read((char*)&record.removed,sizeof(record.removed));

    if (!record.removed){ // si no fue eliminado, imprime y retorna true
        record.print();
        file.close();
        return true;
    }

    file.close();
    return false;
}
