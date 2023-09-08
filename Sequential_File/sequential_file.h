#ifndef SEQUENTIAL_FILE_H
#define SEQUENTIAL_FILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
using namespace std;

template <typename T>
struct Record { // Estructura de un registro
        T data;
        long next_del;

        Record() : next_del(-1) {}
    };

template <typename T>
struct Header {// Estructura del encabezado
        int header;
    };


template <typename T>
class SequentialFile {
private:
    string file_name;
    string heap_file_name;
    string reorg_file_name;
    fstream file;

public:
    SequentialFile();

    SequentialFile(string file_name, string heap_file_name, string reorg_file_name);

    void insert(const T& data);

    vector<T> search(const T& key);

    vector<T> rangeSearch(const T& begin_key, const T& end_key);

    bool add(const T& data);

    bool remove(const T& key);

    void reorganize();

    vector<T> load();


    private: 
    
};



#endif