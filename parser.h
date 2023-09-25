#ifndef PROYECTO_1_PARSER_H
#define PROYECTO_1_PARSER_H
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm> // Necesario para transformar a minúsculas o mayúsculas
#include <regex>
#include <fstream>
#include <cctype>
#include "dataset_bin/binary_conversor.h"
#include "structs/AVL_File/avl_file.h"
#include "parser.h"
#include "structs/Extendible_Hashing_File/hashing.h"
//#include "structs/Extendible_Hashing_File/extendible_hashing_file.h"
bool contieneSoloDigitos(const std::string& cadena);

bool esFloat(const std::string& str);

// Estructura para representar una consulta CREATE TABLE
struct CreateTableQuery {
    std::string tableName;
    std::string filePath;
};

// Estructura para representar una consulta SELECT
struct SelectQuery {
    std::string tableName;
    std::string whereClause;
};

// Estructura para representar una consulta INSERT INTO
struct InsertQuery {
    std::string tableName;
    std::vector<std::string> values;
};

// Estructura para representar una consulta DELETE FROM
struct DeleteQuery {
    std::string tableName;
    std::string whereClause;
};

// Estructura para representar Expresiones regulares
struct ExpresionRelacional {
    std::string atributo;
    std::string operador;
    std::string valor;
    std::string valor2;
};

bool isValidChar(const std::string& value, int size);

bool isValidInt(const std::string& value);

bool isValidFloat(const std::string& value);

bool parsearExpresionRelacional(const std::string& expresion, ExpresionRelacional& resultado);

CreateTableQuery parseCreateTableQuery(const std::string& sqlQuery);

SelectQuery parseSelectQuery(const std::string& sqlQuery);

InsertQuery parseInsertQuery(const std::string& sqlQuery);

DeleteQuery parseDeleteQuery(const std::string& sqlQuery);

int run_sql(std::string sqlQuery);


#endif //PROYECTO_1_PARSER_H
