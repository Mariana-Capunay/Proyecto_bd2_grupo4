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

#include "structs/Extendible_Hashing_File/extendible_hashing_file.cpp"
using namespace std;

int flag = -1;

std::string table_name = "";
std::string file_binary = "test.bin"; //dataset en binario
std::vector<std::string> atributos; //se lee del csv
std::vector<std::string> tipos_atributo = {"int","char","int","char","float"};

// estructuras de los atributos
HashingIndex* keys = new HashingIndex(file_binary);
//SequentialFile<string>* columna2 = new SequentialFile<string>("dataset/test.bin");
AVLFile<int>* columna3 = new AVLFile<int>(file_binary, "columna_3");
//SequentialFile<string>* columna4 = new SequentialFile<string>("test.bin");
AVLFile<float>* columna5 = new AVLFile<float>(file_binary, "columna_4");

// *"test.bin" es la ruta que se asigna en funcion conversor

bool contieneSoloDigitos(const std::string& cadena) {
    for (char caracter : cadena) {
        if (!std::isdigit(caracter)) {
            return false; // Se encontró un carácter que no es un dígito
        }
    }
    return true; // Todos los caracteres son dígitos
}


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


// Funciones de verificación de tipos de datos
bool isValidChar(const std::string& value, int size) {
    return value.size() <= size;
}

bool isValidInt(const std::string& value) {
    return std::regex_match(value, std::regex("-?\\d+"));
}

bool isValidFloat(const std::string& value) {
    return std::regex_match(value, std::regex("-?\\d+(\\.\\d+)?"));
}

/*
// Funcion para evaluar expresiones de WHERE
bool parsearExpresionRelacional(std::string& expresion, ExpresionRelacional& resultado) {
    //std::cout<<expresion<<std::endl;

    // buscamos entre todos los elementos un simbolo igual a 
    std::istringstream ss(expresion);
    
    // Extraer el atributo
    ss >> resultado.atributo;
    // verifica si atributo pertenece al vector de atributos
    
    // Extraer el operador
    ss >> resultado.operador;
    if (resultado.operador!="<" && resultado.operador!=">" && resultado.operador!="between" && resultado.operador!="="){
        return false; // verifica si son las operaciones validas
    }
    
    // Extraer el valor, teniendo en cuenta caracteres especiales
    std::getline(ss, resultado.valor);
    
    // Eliminar espacios en blanco al principio y al final del valor
    resultado.valor = resultado.valor.substr(resultado.valor.find_first_not_of(" "), resultado.valor.find_last_not_of(" ") + 1);
    if (resultado.operador!="between"){ // si no es operador _between_ entonces no debe admitir espacios en blanco
        for (auto x:resultado.valor){
            if (x==' ') return false;
        }
    }
    // Comprobar si se logró extraer los tres elementos
    return !resultado.atributo.empty() && !resultado.operador.empty() && !resultado.valor.empty();
}
*/
/*
bool parsearExpresionRelacional(std::string& expresion, ExpresionRelacional& resultado) {
    // verificamos si condicion contiene operador "=" o "between"

    // Inicializar resultado
    resultado = ExpresionRelacional(); // Asegura que resultado esté vacío
    size_t igualPos = expresion.find('='); // Buscar la posición del igual

    if (igualPos == std::string::npos) { // Si no se encuentra un igual, verificamos "between"
        // hacemos lectura respectiva para el between

        size_t betweenPos = expresion.find("between"); // Buscar la posición del between
        if (igualPos == std::string::npos) {
            return false; // caso en el que la consulta no es valida
        } else {
            // asignamos operador between
            std::istringstream ss(expresion);
    
            ss >> resultado.atributo; // Extraer el atributo
            
            ss >> resultado.operador; // Extraer el operador

            if (resultado.operador!="between"){
                return false; // consulta no valida
            }

            // Extraer el valor
            std::getline(ss, resultado.valor);

            // Eliminar espacios en blanco al principio y al final del valor
            resultado.valor = resultado.valor.substr(resultado.valor.find_first_not_of(" "), resultado.valor.find_last_not_of(" ") + 1);
            // operador igual a between, implica usar "and"
             size_t pos = resultado.valor.find(" and ");
            if (pos != std::string::npos) {  // CASO en el que se compara con texto
                std::string variable1 = resultado.valor.substr(0, pos); // 'casrlos'
                resultado.valor2  = resultado.valor.substr(pos + 5); // 'ruben'
                resultado.valor = variable1; // se da el valor de variable anterior
                return true;

            } else { 
                std::cout << "No se encontró ' and ' en la cadena." << std::endl;
                return false;
            }
        }
    }

    // consulta tipo "="
    
    // Extraer el atributo antes del igual
    resultado.atributo = expresion.substr(0, igualPos);

    // Extraer el operador "="
    resultado.operador = "=";

    // Extraer el valor después del igual
    resultado.valor = expresion.substr(igualPos + 1);

    // Comprobar si se logró extraer los elementos requeridos
    return !resultado.atributo.empty() && !resultado.valor.empty();
}
*/

// Funcion para evaluar expresiones de WHERE
bool parsearExpresionRelacional(std::string& expresion, ExpresionRelacional& resultado) {
    //std::cout<<expresion<<std::endl;

    // buscamos entre todos los elementos un simbolo igual a 
    std::istringstream ss(expresion);
    
    // Extraer el atributo
    ss >> resultado.atributo;
    // verifica si atributo pertenece al vector de atributos
    
    // Extraer el operador
    ss >> resultado.operador;
    if (resultado.operador!="<" && resultado.operador!=">" && resultado.operador!="between" && resultado.operador!="="){
        return false; // verifica si son las operaciones validas
    }
    
    // Extraer el valor, teniendo en cuenta caracteres especiales
    std::getline(ss, resultado.valor);
    
    // Eliminar espacios en blanco al principio y al final del valor
    resultado.valor = resultado.valor.substr(resultado.valor.find_first_not_of(" "), resultado.valor.find_last_not_of(" ") + 1);
    if (resultado.operador!="between"){ // si no es operador _between_ entonces no debe admitir espacios en blanco
        for (auto x:resultado.valor){
            if (x==' ') return false;
        }
    }
    // Comprobar si se logró extraer los tres elementos
    return !resultado.atributo.empty() && !resultado.operador.empty() && !resultado.valor.empty();
}

// CREATE TABLE <nombre> from file "dfdfdfdf"

// Funcion para analizar consulta CREATE
CreateTableQuery parseCreateTableQuery(const std::string& sqlQuery) {
    CreateTableQuery query;

    // Expresión regular para buscar "create table" independientemente de los espacios en blanco
    std::regex regexPattern(R"(create\s+table\s+(\w+)\s+from\s+file\s+['"]([^'"]+)['"])");
    //std::regex regexPattern(R"((create\s+table)[\s\S]*(\w+)[\s\S]*(from\s+file\s+['"]([^'"]+)['"]))"); //ejemplo de lo que no se debe hacer
    std::smatch match;

    if (std::regex_search(sqlQuery, match, regexPattern)) {
        query.tableName = match[1].str(); // El primer grupo capturado es el nombre de la tabla
        query.filePath = match[2].str(); // El segundo grupo capturado es la ruta del archivo

        //table_name = query.tableName;  // valores generales

        //TODO crear dataset en binario y pasar a todas las estructuras
        /*  
            1. abre el archivo, lo lleva a binario 
            2. llena todas las estructuras (de acuerdo a sus columnas asignadas)
        */
        ifstream file(query.filePath);
        if (!file.is_open()) {
            flag = 2; //archivo no existe
            cout<<"Archivo "<<query.filePath<<" no existe"<<endl;
        } else{
            file.close();

            // generamos version binaria del archivo
            string atr_1;
            string atr_2;
            string atr_3;
            string atr_4;
            string atr_5;

            string new_file = conversor(query.filePath, atr_1, atr_2, atr_3, atr_4, atr_5);
            flag = 1; // archivo existe

            // añadimos valor de atributos al vector
            atributos.push_back(atr_1);
            atributos.push_back(atr_2);
            atributos.push_back(atr_3);
            atributos.push_back(atr_4);
            atributos.push_back(atr_5);

            table_name = query.tableName; //se da nombre a la tabla (en caso todo haya ido bien)
        }
        
    } else {
        std::cerr << "Consulta CREATE TABLE no válida." << std::endl; // Consulta CREATE TABLE no válida
    }
    //std::cout<<"Table name: "<<query.tableName<<" - filePath: "<<query.filePath<<std::endl;
    return query;
}


// Función para analizar una consulta SELECT 
SelectQuery parseSelectQuery(const std::string& sqlQuery) {
    SelectQuery query;

    // Utiliza una expresión regular para extraer el nombre de la tabla y la cláusula WHERE
    std::regex regexPattern(R"(select\s+(?:\*\s+)?from\s+(\w+)(?:\s+where\s+(.*))?)", std::regex::icase);
    std::smatch match;
    

    if (std::regex_search(sqlQuery, match, regexPattern)) {
        query.tableName = match[1].str(); // El primer grupo capturado contiene el nombre de la tabla
        if (match[1].str()!=table_name){ // si es distinto al nombre de la tabla o no fue creada
            std::cerr << "Tabla "<<query.tableName << " no existe" << "-> Solo cuenta con la tabla "<<table_name<<std::endl;
        }  else{

            query.whereClause = match[2].str(); // El segundo grupo capturado contiene la cláusula WHERE si está presente    
            if (query.whereClause!=""){ // se hace consulta where
                //std::cout<<"select con WHERE"<<std::endl;
                
                /*
                    evaluar where ( <, >, =, between x and y)
                */
                ExpresionRelacional resultado;
                parsearExpresionRelacional(query.whereClause, resultado); 
                std::cout<<"query-operator: "<<resultado.operador;
                // evaluar que estructuras llamar para cada atributo
                if (resultado.operador=="<"){
                    

                } else if (resultado.operador==">"){

                } else if (resultado.operador=="="){

                } else if (resultado.operador=="between"){

                } else {
                    std::cout<<"No deberia llegar aqui"<<std::endl;
                }

            } else { // sin where
                //std::cout<<"select sin WHERE"<<std::endl;

                /*
                    mostrar todo el contenido
                */
                int cont = 0; int pos = 0;
                Record record;
                while (cont<20){
                    bool res = print_record(file_binary,pos);
                    pos+=record.size();
                    if (res) cont++;
                }
            }
            

            //TODO evaluar where (where simple-> >25, <50, between x and y, =x)
        } //std::cout<<"Tablename: "<<query.tableName<<" - Where: "<<query.whereClause<<std::endl;
    } else {
        // Consulta SQL no válida
        std::cerr << "Consulta SQL no válida." << std::endl;
    }
    return query;
}


// Función para analizar una consulta INSERT INTO con tipos de datos específicos
InsertQuery parseInsertQuery(const std::string& sqlQuery) {
    InsertQuery query;

    // Busca "insert into" independientemente de los espacios en blanco
    std::regex regexPattern(R"(insert\s+into\s+(\w+)\s+values\s*\(\s*["']([^"']*)["']\s*,\s*["']([^"']*)["']\s*,\s*(\d+)\s*,\s*["']([^"']*)["']\s*,\s*([+-]?\d+(\.\d+)?)\s*\))");
    //std::regex regexPattern(R"(insert\s+into\s+(\w+)\s+values\s*\(\s*\"([^\"]*)\"\s*,\s*\"([^\"]*)\"\s*,\s*(\d+)\s*,\s*\"([^\"]*)\"\s*,\s*([+-]?\d+(\.\d+)?)\s*\))");
    //std::regex regexPattern(R"(insert\s+into\s+([^\s]+)\s+values\s*\(\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*\))"); //ejemplo
    std::smatch match;

    if (std::regex_search(sqlQuery, match, regexPattern)) {
        // El primer grupo capturado es el nombre de la tabla
        query.tableName = match[1].str();

        if (query.tableName!=table_name){
            std::cerr << "Tabla "<<query.tableName << " no existe" << "-> Solo cuenta con la tabla "<<table_name<<std::endl;
        } else{
            // Los grupos capturados 2 a 6 son los valores de diferentes tipos de datos
            std::string char8Value = match[2].str();
            std::string char40Value = match[3].str();
            std::string intValue = match[4].str();
            std::string char25Value = match[5].str();
            std::string floatValue = match[6].str();

            // Verifica si los tipos de datos coinciden con lo esperado
            if (!isValidInt(char8Value) || !isValidChar(char8Value,8)||  // verifica que key sea numero y con 8 cifras
                !isValidChar(char40Value,40) ||
                !isValidInt(intValue) || 
                !isValidChar(char25Value,25) ||  
                !isValidFloat(floatValue)) {
                std::cerr << "Tipos de datos no válidos en la consulta INSERT INTO." << std::endl;
                return query; // Puedes manejar este error de la manera que desees
            }

            // Si los tipos de datos son válidos, agrega los valores a la estructura InsertQuery
            query.values.push_back(intValue);
            query.values.push_back(char40Value);
            query.values.push_back(intValue);
            query.values.push_back(char25Value);
            query.values.push_back(floatValue);

            //for (auto value:query.values) std::cout<<value<<" - ";

            /*
                TODO insert values (en archivo bin, dataset y en todas las estructuras)
            */
        }
    } else {
        // Consulta INSERT INTO no válida
        std::cerr << "Consulta INSERT INTO no válida." << std::endl;
    }
    return query;
}

// Función para analizar una consulta DELETE FROM
DeleteQuery parseDeleteQuery(const std::string& sqlQuery) {
    DeleteQuery query;

    // Expresión regular para buscar "delete from" seguido del nombre de la tabla y opcionalmente la cláusula WHERE
    std::regex regexPattern(R"(delete\s+from\s+(\w+)\s*(?:where\s+(.+))?)");
    std::smatch match;

    if (std::regex_search(sqlQuery, match, regexPattern)) {
        if (match.size() >= 2) {
            // El segundo grupo capturado es el nombre de la tabla
            query.tableName = match[1].str();
            if (query.tableName!=table_name){
                std::cerr << "Tabla "<<query.tableName << " no existe" << "-> Solo cuenta con la tabla "<<table_name<<std::endl;
            }
            
            query.whereClause = match[2].str(); // El tercer grupo capturado es la cláusula WHERE
            if (query.whereClause!="") { 
                query.whereClause = match[2].str();
                std::cout<<query.whereClause<<std::endl;

                ExpresionRelacional resultado;
                //parsearExpresionRelacional(query.whereClause, resultado);

                // evaluar que estructuras llamar para cada atributo
                if (resultado.operador=="<"){


                } else if (resultado.operador==">"){

                } else if (resultado.operador=="="){

                } else if (resultado.operador=="between"){
                    std::cout<<"Eliminación por rango no implementada"<<std::endl;
                } else {
                    std::cout<<"No deberia llegar aqui"<<std::endl;
                }

            } else { // sin where
                //std::cout<<"select sin WHERE"<<std::endl;

                /*
                    mostrar todo el contenido
                */
                std::cout<<"Especifique busqueda"<<std::endl;
            }
        } else{
            std::cout<<"Debe indicar condicion para eliminar"<<std::endl;
        }
        
    } else {
        // Consulta DELETE FROM no válida
        std::cerr << "Consulta DELETE FROM no válida." << std::endl;
    }

    return query;
}

int main() {
    /*
    std::cout << "Ingresa la consulta SQL y presiona Enter:" << std::endl;
    std::string sqlQuery;
    std::getline(std::cin, sqlQuery);
      // Convierte la consulta a minúsculas (o mayúsculas) antes de analizarla
    //std::string queryLower = sqlQuery;
    std::transform(sqlQuery.begin(), sqlQuery.end(), sqlQuery.begin(), ::tolower);
    */
    

    while (table_name==""){ // debe crear la tabla para hacer otras consultas
        std::string sqlQuery;
        std::getline(std::cin, sqlQuery);
        std::transform(sqlQuery.begin(), sqlQuery.end(), sqlQuery.begin(), ::tolower); //se ingresa consulta (en este caso, se espera CREATE)

        std::istringstream iss(sqlQuery);
        std::string firstKeyword;
        iss >> firstKeyword;

        if (firstKeyword == "create") { // Analiza una consulta CREATE TABLE
            CreateTableQuery query = parseCreateTableQuery(sqlQuery); //acciones en función de la estructura de la consulta CREATE TABLE
        } else {
            std::cout<<"Debe crear la tabla"<<std::endl;
        }
    }


    while (true){
        std::string sqlQuery;
        std::getline(std::cin, sqlQuery);
        std::transform(sqlQuery.begin(), sqlQuery.end(), sqlQuery.begin(), ::tolower); //se ingresa consulta (en este caso, se espera CREATE)

        // Verifica la primera palabra clave para determinar el tipo de consulta
        std::istringstream iss(sqlQuery);
        std::string firstKeyword;
        iss >> firstKeyword;

        if (firstKeyword == "select"){
            SelectQuery query = parseSelectQuery(sqlQuery); // Analiza una consulta SELECT
            // Implementa acciones en función de la estructura de la consulta SELECT
        } else if (firstKeyword == "insert") {
            // Analiza una consulta INSERT INTO
            InsertQuery query = parseInsertQuery(sqlQuery);
            // Implementa acciones en función de la estructura de la consulta INSERT INTO
        } else if (firstKeyword == "delete") {
            // Analiza una consulta DELETE FROM
            DeleteQuery query = parseDeleteQuery(sqlQuery);
            // Implementa acciones en función de la estructura de la consulta DELETE FROM
        } else if (firstKeyword == "create"){
            std::cout<<"Solo se permite crear una tabla."<<std::endl;
        } else{
            std::cout<<"Consulta no valida"<<std::endl;
        }
    }
    return 0;
}
