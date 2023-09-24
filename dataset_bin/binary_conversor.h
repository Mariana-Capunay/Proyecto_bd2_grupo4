#ifndef PROYECTO_1_BINARY_CONVERSOR_H
#define PROYECTO_1_BINARY_CONVERSOR_H
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "record.h"

std::string conversor(std::string file_route, std::string& atr_1, std::string& atr_2, std::string& atr_3, std::string& atr_4, std::string& atr_5);

Record read_record(std::string file_name, int pos);

bool print_record(std::string file_name, int pos);

#endif //PROYECTO_1_BINARY_CONVERSOR_H
