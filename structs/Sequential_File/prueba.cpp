#include "sequential_files.h"
#include <vector>


//auto seq = SequentialFile("input", "atributo");
/*
void test_insert(){
    seq.insert("Coconut", 0);

    seq.print();
}
*/
int main() {
    // Crear el archivo secuencial
    SequentialFile file("input", "atributo");

    SeqRecord record1;
    // Agregar algunos registros
    record1.setData("Andrea", 0.1, 33);
    file.add(record1);
    record1.setData("Carlos", 0.2, 33);

    file.add(record1);
    record1.setData("Cinthya", 0.3, 33);

    file.add(record1);
    record1.setData("Josimar", 0.4, 33);

    file.add(record1);
    record1.setData("Jorge", 0.5, 33);

    file.add(record1);
    record1.setData("Mabel", 0.6, 33);

    file.add(record1);
    record1.setData("Saulo", 0.7, 33);

    file.add(record1);

    // Imprimir todos los registros
            file.print();
/*
    // Agregar algunos registros
    file.add({"Gabriel", 0.1});
    file.add({"Diana", 0.2});
    file.add({"Maria", 0.3});
    // Imprimir todos los registros

    file.print();

    // Eliminar algunos registros
    file.remove_("Andrea");
    file.remove_("Maria");
    file.remove_("Cinthya");
    file.remove_("Gabriel");

    // Imprimir todos los registros
    file.print();

    // Buscar algunos registros
    vector<SeqRecord> result = file.search("Carlos");
    cout << "Resultados de la búsqueda: " << endl;
    for (SeqRecord record : result) {
        record.showData();
    }
*/


}
