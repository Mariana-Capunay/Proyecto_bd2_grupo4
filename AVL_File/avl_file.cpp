#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

bool archivo_existe(string nombre){
    ifstream archivo(nombre.c_str());
    return archivo.good();
}

void crear_archivo(string nombre){
    ifstream archivo(nombre.c_str());
    if (archivo.good()){ //si existe
        return; //ya no es necesario crear
    }
    ofstream file(nombre, ios::binary);
//cout<<"creando archivo "<<nombre<<endl;
    file.close();
}

//template <typename T>
struct Registro{
    int criterio;       // Provisionalmente int para pruebas
    long pointer;
    long left;
    long right;
    bool removed;
};

template <typename T>
class AVLFile{
private:
    string filename;
    string index;
    long pos_root;
public:
    AVLFile(string filename){
        this->filename = filename;
        this->inicializar_root();
    }
    vector<Registro> find(int key){
        ifstream file(this->filename, ios::binary | ios::in | ios::out);
        if(!file.is_open()) throw out_of_range("No se pudo abrir el archivo");
        return find(pos_root, key, file);
    }
    void add(Registro record){
        fstream pointerFile(this->index, ios::in | ios::out | ios::binary);
        long curr_pos = pos_root;
        add(curr_pos, record, pointerFile);
    }
    vector<Registro> inorder(){ //registros de manera ordenada
        ifstream file(filename, ios::app|ios::in|ios::binary);
        file.seekg(0);
        vector<Registro> result;
        inorder_rec(pos_root, result, file);
        file.close();
        return result;
    }
private:
    Registro getRecord(int pos, ifstream& file){
        Registro record;
        file.seekg(pos, ios::beg);//fixed length record
        file.read((char *) &record, sizeof(Registro));
        return record;
    }
    vector<Registro> find(long pos_node, int key, ifstream& file) {
        //TODO: crear funciones previous y next que reciban una posición y devuelvan el anterior nodo (izquierdo más a la derecha, y siguiente (derecho más a la izquierda)
        if (pos_node == -1) {
            throw out_of_range("No se encontró");
        }
        vector<Registro> coincidenceList = {};
        Registro record = getRecord(pos_node, file);
        //Criterio es el campo buscado
        int recordKey = record.criterio;
        while (recordKey != key) {
            if (recordKey > key) {
                record = getRecord(record.left, file);
            } else {
                record = getRecord(record.right, file);
            }
            recordKey = record.criterio;
        }
        //Acabado el while, en record se tendrá a la primera coincidencia
        coincidenceList.push_back(record);
        //Ahora se deberá buscar las demás coincidencias, que pueden estar a la derecha o izquierda inmediata de la primera coincidencia

    }
    void inorder_rec(long pos_node, vector<Registro> &result, ifstream& file){
        if (pos_node==-1) {} //caso en el que se termina
        else{
            file.seekg(pos_node,ios::beg); // voy a la posicion asociada
            Registro record;
            file.read((char*)&record,sizeof(Registro)); //leo el archivo asociado
            //file.close(); //cierro el archivo
            inorder_rec(record.left, result,file);
            result.push_back(record); //añado el registro al vector
            inorder_rec(record.right, result,file);
        }
    }
    void inicializar_root(){
        bool existe = archivo_existe(this->filename);
        crear_archivo(this->filename);
        if (!existe){
            pos_root = -1;
        }
        else pos_root = 0;
    }

    void add(long& curr_pos, Registro& record, fstream& pointerFile) {
        if (curr_pos == -1) {
            pointerFile.seekg(0, ios::end);
            curr_pos = pointerFile.tellg();
            //cout << "Inserting at: " << curr_pos << endl;
            pointerFile.seekp(curr_pos, ios::beg); // Mover el puntero de escritura al nodo padre
            pointerFile.write((char*)&record, sizeof(Registro));
        }
        else {
            pointerFile.seekg(curr_pos, ios::beg);
            Registro parent;
            pointerFile.read((char*)&parent, sizeof(Registro));
            if (record.criterio < parent.criterio) {
                add(parent.left, record, pointerFile);
                // Actualizar el puntero izquierdo del nodo padre
                // pointerFile.seekp(curr_pos, ios::beg);
                // Mover el puntero de escritura al nodo padre;
                //pointerFile.write((char*)&parent, sizeof(Registro));
            }
            else if (record.criterio > parent.criterio) {
                add(parent.right, record, pointerFile);
                // Actualizar el puntero derecho del nodo padre
                // pointerFile.seekp(curr_pos, ios::beg);
                // Mover el puntero de escritura al nodo padre
                // pointerFile.write((char*)&parent, sizeof(Registro));
            }
            else {
                cout << "ID already exists" << endl;
            }
            pointerFile.seekp(curr_pos, ios::beg); // Mover el puntero de escritura al nodo padre
            //cout<<"desde aqui: "; parent.showData();
            pointerFile.write((char*)&parent, sizeof(Registro));
        }
        pointerFile.close();
    }


    void remove(T value);
    T maxValue();
private:
    void remove(long pos, T value, fstream& dataset, fstream& indices);
    T maxValue(long pos);

    // para estos metodos, el puntero ya debe tener la posicion correcta para leer o escribir en el archivo
    void leer_registro(Registro& record); //crear funcion para leer registro
    void leer_dato(Registro record, T& dato); //funcion para leer el dato, una vez ya se tiene el registro
    void escribir_registro(Registro& record);
    Registro dar_valor(long pos); //retorna registro en la posicion pos
};

template <typename T>
T AVLFile<T>::maxValue(){
    return maxValue(this->pos_root);
}

template <typename T>
T AVLFile<T>::maxValue(long pos)
{
    if (pos == -2)
        throw("The tree is empty");


    // obtenemos valor de atributo (que se quiere eliminar) de nodo actual
    dataset.seekp(pos, ios::beg); //se situa puntero donde nodo actual indica

    // se lee dato y registro
    Registro record; T dato;
    leer_registro(record);
    leer_dato(record, dato);

    if (record.right == -1)
        return dato; //retorna dato actual
    else
        return maxValue(record.right);
}


template <typename T>
void AVLFile<T>::remove(T value){
    fstream dataset(this->filename, ios::binary|ios::in|ios::out);
    fstream indices(this->index, ios::binary|ios::in|ios::out);
    remove(this->pos_root, value,fstream dataset, fstream indices);
    dataset.close();
    indices.close();
}

template <typename T>
void AVLFile<T>::remove(long pos, T value, fstream& dataset, fstream& indices){
    if (pos==-1) return; //no se encontró lo que se quiere eliminar

    // obtenemos valor de atributo (que se quiere eliminar) de nodo actual
    dataset.seekp(pos, ios::beg); //se situa puntero donde nodo actual indica

    // se lee dato y registro
    Registro record; T dato;
    leer_registro(record);
    leer_dato(record, dato);

    //se recorre nodo
    if (value<dato)remove(record.left, value); // se verifica que hijo recorrer
    else if (value>dato) remove(record.right, value);
    else
    {
        dataset.seekp(pos, ios::beg); //se situa puntero donde nodo actual indica

        if (record.left == -1 && record.right == -1) //si el registro es hoja
        {
            record.removed = true; //se actualiza flag
            escribir_registro(record); //descolgamos al nodo
        }
        else if (record.left == -1)
        {
            record = dar_valor(record.rigth); // hijo derecho pasa a suplantar al registro
            escribir_registro(record);
            //node = node->right;
            
        }
        else if (record.right == -1)
        {
            record = dar_valor(record.left);  // hijo izquierdo pasa a suplantar al registro
            escribir_registro(record);
        }
        else
        {
            T temp = maxValue(record.left);
            // = temp; dar valor de temp a atributo de record
            remove(record.left, temp);
        }
    }

    if(node){
        updateHeight(node);
        balance(node);
    }
}

void writeFile(string sourceName){      // Modelo de insercion por archivo (rango)
    AVLFile<int> file(sourceName);
    Registro record;
    int pos = 0;
    int source_regs = 4;
    for (int i = 0; i < source_regs; i++) {
        //record.setData();
        file.add(record);
        pos++;
    }
}
void readFile(string filename){
    AVLFile file(filename);
    cout<<"--------- show all sorted data -----------\n";
    vector<Registro> result = file.inorder();
//cout<<"Tamano: "<<result.size()<<endl;
    for(Registro re : result){
        re.showData(); cout<<endl;
    }
}
void findFile(string filename,int key){
    cout<<"\n\nRegistro nro "<<key<<endl;
    AVLFile file(filename);
    file.find(key).showData();
}
int main() {
    writeFile("data.dat");
    readFile("data.dat");
    findFile("data.dat", 4);
    findFile("data.dat", 11);
    return 0;
}
