#include "parser.h"
#include "structs/AVL_File/avl_file.h"

using namespace std;

std::string table_name = "";
std::string file_binary = "test.bin"; //dataset en binario
std::vector<std::string> atributos; //se lee del csv
std::vector<std::string> tipos_atributo = {"int","char","int","char","float"};

// estructuras de los atributos
HashingIndex* keys = new HashingIndex(file_binary);
//SequentialFile<string>* columna2 = new SequentialFile<string>("dataset/test.bin");
AVLFile<int>* columna3 = new AVLFile<int>(file_binary, "columna_3");
//SequentialFile<string>* columna4 = new SequentialFile<string>("test.bin");
AVLFile<float>* columna5 = new AVLFile<float>(file_binary, "columna_5");

// *"test.bin" es la ruta que se asigna en funcion conversor

int flag = -1;

bool contieneSoloDigitos(const std::string& cadena) {
    for (char caracter : cadena) {
        if (!std::isdigit(caracter)) {
            return false; // Se encontró un carácter que no es un dígito
        }
    }
    return true; // Todos los caracteres son dígitos
}


bool esFloat(const std::string& str) {
    std::istringstream ss(str);
    float numero;
    return (ss >> numero) && (ss.eof() || ss.peek() == '\n');
}

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


bool parsearExpresionRelacional(const std::string& expresion, ExpresionRelacional& resultado) {
    std::istringstream ss(expresion);

    // Extraer el atributo
    ss >> resultado.atributo;
    // verifica si atributo pertenece al vector de atributos
    auto it = find(atributos.begin(), atributos.end(), resultado.atributo);
    if (it==atributos.end()) {
        cout<<"Nombre de atributo no valido"<<endl;
        return false;
    }
    // Extraer el operador
    ss >> resultado.operador;
    if (resultado.operador!="between" && resultado.operador!="="){
        return false; // verifica si son las operaciones validas
    }

    // Extraer el valor, teniendo en cuenta caracteres especiales
    std::getline(ss, resultado.valor);

    // Eliminar espacios en blanco al principio y al final del valor (tambien comillas, en caso sea string)
    resultado.valor = resultado.valor.substr(resultado.valor.find_first_not_of(" "), resultado.valor.find_last_not_of(" ") + 1);
    if (resultado.operador!="between"){ // si no es operador _between_ entonces no debe admitir espacios en blanco
        if ((resultado.valor[0]=='\'' && resultado.valor[resultado.valor.size()-1]=='\'') || (resultado.valor[0]=='\"' && resultado.valor[resultado.valor.size()-1]=='\"' )){
            resultado.valor = resultado.valor.substr(1, resultado.valor.size() - 2);
        }
        for (auto x:resultado.valor){
            if (x==' ') return false;
        }
    } else{ // verificacion para BETWEEN
        size_t pos = resultado.valor.find(" and ");
        if (pos != std::string::npos) {
            std::string variable1 = resultado.valor.substr(0, pos); // 'carlos'
            resultado.valor2  = resultado.valor.substr(pos + 5); // 'ruben'

            resultado.valor = variable1; // se da el valor de variable anterior

            // Eliminar las comillas simples (si es necesario)
            if ((variable1[0]=='\'' && variable1[variable1.size()-1]=='\'' &&
                 resultado.valor2[0]=='\'' && resultado.valor2[resultado.valor.size()-1]=='\'') ||
                (variable1[0]=='\"' && variable1[variable1.size()-1]=='\"' &&
                 resultado.valor2[0]=='\"' && resultado.valor2[resultado.valor.size()-1]=='\"')){
                variable1 = variable1.substr(1, variable1.size() - 2);
                resultado.valor2 = resultado.valor2.substr(1, resultado.valor2.size() - 2);
                std::cout << "Variable 1: " << variable1 << std::endl;
                std::cout << "Variable 2: " << resultado.valor2 << std::endl;
            }
            return true;

        } else {
            std::cout << "No se encontró ' and ' en la cadena." << std::endl;
        }
    }
    // Comprobar si se logró extraer los tres elementos
    return !resultado.atributo.empty() && !resultado.operador.empty() && !resultado.valor.empty();
}




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
        ifstream binSource(query.filePath);
        if (!binSource.is_open()) {
            flag = 2; //archivo no existe
            cout<<"Archivo "<<query.filePath<<" no existe"<<endl;
        } else{
            binSource.close();

            // generamos version binaria del archivo
            string atr_1;
            string atr_2;
            string atr_3;
            string atr_4;
            string atr_5;

            string new_file = conversor(query.filePath, atr_1, atr_2, atr_3, atr_4, atr_5);
            cout << "Archivo creado: " << new_file << endl;
            flag = 1; // archivo existe

            // añadimos valor de atributos al vector
            std::transform(atr_1.begin(), atr_1.end(), atr_1.begin(), ::tolower);
            std::transform(atr_2.begin(), atr_2.end(), atr_2.begin(), ::tolower);
            std::transform(atr_3.begin(), atr_3.end(), atr_3.begin(), ::tolower);
            std::transform(atr_4.begin(), atr_4.end(), atr_4.begin(), ::tolower);
            std::transform(atr_5.begin(), atr_5.end(), atr_5.begin(), ::tolower);
            atributos.push_back(atr_1);
            atributos.push_back(atr_2);
            atributos.push_back(atr_3);
            atributos.push_back(atr_4);
            atributos.push_back(atr_5);

            table_name = query.tableName; //se da nombre a la tabla (en caso todo haya ido bien)

            /*agregar valores a todas las estructuras*/
            binSource.open(new_file, ios::binary);
            binSource.seekg(ios::end);
            string avl_filename_1 = "AVL_file";
            avl_filename_1 += atr_2;

            auto avl1 = new AVLFile<int>(new_file, atr_2);
            cout << "Construyendo avl desde " << new_file << " de tamahnio " << binSource.tellg() << endl;
            avl1->buildFromFile(new_file, 2);

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
            flag = 11;
            std::cerr << "Tabla "<<query.tableName << " no existe" << "-> Solo cuenta con la tabla "<<table_name<<std::endl;
        }  else{

            query.whereClause = match[2].str(); // El segundo grupo capturado contiene la cláusula WHERE si está presente
            if (query.whereClause!=""){ // se hace consulta where
                //std::cout<<"select con WHERE"<<std::endl;

                /*
                    evaluar where ( <, >, =, between x and y)
                */
                ExpresionRelacional resultado;
                bool parseo = parsearExpresionRelacional(query.whereClause, resultado);
                //std::cout<<"query-operator: "<<resultado.operador;
                // evaluar que estructuras llamar para cada atributo
                if (parseo){ // se completó el parseo
                    if (resultado.operador=="="){   // selecciona busqueda puntual en cada estructura
                        flag = 4;
                        cout<<"operador = ";
                        cout<<"atributo: "<<resultado.atributo<<" | value: "<<resultado.valor<<endl;
                        vector<long> res;
                        if (resultado.atributo==atributos[0]){
                            if (contieneSoloDigitos(resultado.valor)){
                                keys->find(stoi(resultado.valor)); //retornar
                            } else{
                                flag = 5;
                            }
                        } else if (resultado.atributo==atributos[1]){

                        } else if (resultado.atributo==atributos[2]){
                            if (contieneSoloDigitos(resultado.valor)){
                                res= columna3->search(stoi(resultado.valor)); // retornar
                                for (auto x:res) read_record(file_binary,x);
                            } else {
                                flag = 5;
                            }

                        } else if (resultado.atributo==atributos[3]){

                        } else{
                            if (esFloat(resultado.valor)){
                                res = columna5->search(stof(resultado.valor)); // retornar
                                for (auto x:res) read_record(file_binary,x);
                            }
                        }

                    } else if (resultado.operador=="between"){ // selecciona busqueda por rango en cada estructura
                        flag = 4;
                        if (resultado.atributo==atributos[0]){ flag =5; cout<<"Key no soporta busqueda por rango"<<endl; }
                        else if (resultado.atributo==atributos[1]);
                        else if (resultado.atributo==atributos[2]);
                        else if (resultado.atributo==atributos[3]);
                        else;

                    } else {
                        flag = 5;
                        std::cout<<"No deberia llegar aqui"<<std::endl;
                    }
                } else{
                    flag = 5;
                }
            } else { // sin where
                //std::cout<<"select sin WHERE"<<std::endl;
                flag = 3;
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
        flag = 5;
        // Consulta SQL no válida
        std::cerr << "Consulta SQL no válida." << std::endl;
    }
    return query;
}


// Función para analizar una consulta INSERT INTO con tipos de datos específicos
InsertQuery parseInsertQuery(const std::string& sqlQuery) {
    InsertQuery query;

    // Busca "insert into" independientemente de los espacios en blanco
    //std::regex regexPattern(R"(insert\s+into\s+(\w+)\s+values\s*\(\s*["']([^"']*)["']\s*,\s*["']([^"']*)["']\s*,\s*(\d+)\s*,\s*["']([^"']*)["']\s*,\s*([+-]?\d+(\.\d+)?)\s*\))");
    std::regex regexPattern(R"(insert\s+into\s+(\w+)\s+values\s*\(\s*(\d+)\s*,\s*["']([^"']*)["']\s*,\s*(\d+)\s*,\s*["']([^"']*)["']\s*,\s*([+-]?\d+(\.\d+)?)\s*\))");
    //std::regex regexPattern(R"(insert\s+into\s+(\w+)\s+values\s*\(\s*\"([^\"]*)\"\s*,\s*\"([^\"]*)\"\s*,\s*(\d+)\s*,\s*\"([^\"]*)\"\s*,\s*([+-]?\d+(\.\d+)?)\s*\))");
    //std::regex regexPattern(R"(insert\s+into\s+([^\s]+)\s+values\s*\(\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*,\s*(\S+)\s*\))"); //ejemplo
    std::smatch match;

    if (std::regex_search(sqlQuery, match, regexPattern)) {
        // El primer grupo capturado es el nombre de la tabla
        query.tableName = match[1].str();

        if (query.tableName!=table_name){
            flag = 11;
            std::cerr << "Tabla "<<query.tableName << " no existe" << "-> Solo cuenta con la tabla "<<table_name<<std::endl;
        } else{
            // Los grupos capturados 2 a 6 son los valores de diferentes tipos de datos
            std::string char8Value = match[2].str();
            std::string char40Value = match[3].str();
            std::string intValue = match[4].str();
            std::string char25Value = match[5].str();
            std::string floatValue = match[6].str();

            // Verifica si los tipos de datos coinciden con lo esperado
            if (!isValidInt(char8Value) || char8Value.size()>8 || // verifica que key sea numero y con 8 cifras
                !isValidChar(char40Value,40) ||
                !isValidInt(intValue) ||
                !isValidChar(char25Value,25) ||
                !isValidFloat(floatValue)) {
                std::cerr << "Tipos de datos no válidos en la consulta INSERT INTO." << std::endl;
                flag = 7;
                return query; // Puedes manejar este error de la manera que desees
            }

            // Si los tipos de datos son válidos, agrega los valores a la estructura InsertQuery
            query.values.push_back(intValue);
            query.values.push_back(char40Value);
            query.values.push_back(intValue);
            query.values.push_back(char25Value);
            query.values.push_back(floatValue);

            /*
                TODO insert values (en archivo bin, dataset y en todas las estructuras)
            */
            Record record;
            record.key = stoi(intValue);
            strncpy(record.atrib1, char40Value.c_str(), sizeof(record.atrib1) - 1); // -1 para dejar espacio para el carácter nulo
            record.atrib1[sizeof(record.atrib1) - 1] = '\0'; // Asegura que el último carácter sea nulo
            record.atrib2 = stoi(intValue);
            strncpy(record.atrib3, char25Value.c_str(), sizeof(record.atrib3) - 1); // -1 para dejar espacio para el carácter nulo
            record.atrib3[sizeof(record.atrib3) - 1] = '\0'; // Asegura que el último carácter sea nulo
            strncpy(record.atrib3, char25Value.c_str(), sizeof(record.atrib3) - 1); // -1 para dejar espacio para el carácter nulo
            record.atrib4 = stoi(floatValue);
            record.removed = false;

            ofstream file(file_binary, ios::binary);
            if (!file.is_open()){
                flag = 7;
                cout<<"No se pudo abrir archivo dataset en binario"<<endl;
            } else{
                file.seekp(0,ios::end);
                flag = 6;
                file.write((char*)&record,record.size()); //escribe registro al final
                file.close();
            }

            /* insertar cada valor en una estructura */

            //for (auto value:query.values) std::cout<<value<<" - ";

        }
    } else {
        // Consulta INSERT INTO no válida
        std::cerr << "Consulta INSERT INTO no válida." << std::endl;
        flag = 7;
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
                flag = 11;
            }

            query.whereClause = match[2].str(); // El tercer grupo capturado es la cláusula WHERE
            if (query.whereClause!="") {
                query.whereClause = match[2].str();
                std::cout<<query.whereClause<<std::endl;

                ExpresionRelacional resultado;
                //parsearExpresionRelacional(query.whereClause, resultado);

                // evaluar que estructuras llamar para cada atributo
                if (resultado.operador=="="){

                    //busca elemento en la estructura asociada
                    // si no lo encuentra, flag = 10. Si lo encuentra, lo elimina, flag=8
                } else if (resultado.operador=="between"){
                    std::cout<<"Eliminación por rango no implementada"<<std::endl;
                    flag = 9;
                } else {
                    flag = 11;
                    std::cout<<"Nombre de tabla no existe"<<std::endl;
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
        std::cout << "Waiting" << std::endl;
        std::string sqlQuery;
        std::getline(std::cin, sqlQuery);
        std::transform(sqlQuery.begin(), sqlQuery.end(), sqlQuery.begin(), ::tolower); //se ingresa consulta (en este caso, se espera CREATE)

        std::istringstream iss(sqlQuery);
        std::string firstKeyword;
        iss >> firstKeyword;
        //create table tabla from file "dataset/10k/dataset_1.csv"
        if (firstKeyword == "create") { // Analiza una consulta CREATE TABLE
            cout << "Creando tabla" << endl;
            CreateTableQuery query = parseCreateTableQuery(sqlQuery); //acciones en función de la estructura de la consulta CREATE TABLE
        } else {
            std::cout<<"Debe crear la tabla"<<std::endl;
        }
    }

    //for (auto x:atributos) cout<<x<<" ";  // para verificar que atributos están en minuscula

    while(true){
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
