#include "avl_file.h"
#include <vector>

AVLFile<int>* avl = new AVLFile<int>("filename.bin", "atributo");
vector<int> a = {1,2,4,4,4,1,1,2,3,7,3};

void test_insert(){
    for (auto x:a) avl->insert(x,x);
}

void test_search(){
     //busqueda por rango retorna posicion en la que se insertó el elemento
    cout<<"elemento 4: (posiciones) ";
    vector<long> r = avl->search(4);
    for (auto x:r) cout<<x<<" ";
    cout<<endl;
    //avl->deleteFiles();

    cout<<"elemento 2: (posiciones) ";
    r = avl->search(2);
    for (auto x:r) cout<<x<<" ";
    cout<<endl;

    //

    cout<<"elemento 1: (posiciones) ";
    r = avl->search(1);
    for (auto x:r) cout<<x<<" ";
    cout<<endl;
}

void test_search_range(){
    cout<<"posiciones de elementos [1-3]: ";
    vector <long> r = avl->rangeSearch(1,3);
    for (auto x:r) cout<<x<<" -> elemento: "<<a[x]<<" \n";
    cout<<endl;
}

void test_remove(){
    cout<<"eliminando elemento 4"<<endl;
    avl->remove(4);
    cout<<"eliminando elemento 1"<<endl;
    avl->remove(1);
    cout<<"eliminando elemento 2"<<endl;
    avl->remove(2);
    cout<<"eliminando elemento 3"<<endl;
    avl->remove(3);
}

int main(){



    //prueba remove

  
    test_insert();
    avl->printData();
    test_search();
    test_search_range();
    test_remove();
    avl->printData();

    return 0;
}
