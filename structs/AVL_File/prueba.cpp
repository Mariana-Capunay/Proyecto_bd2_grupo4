#include "avl_file.h"
#include <vector>

int main(){

    AVLFile<int>* avl = new AVLFile<int>("filename.bin", "atributo");
    vector<int> a = {1,2,4,4,4,1,1,2,3};
    for (auto x:a) avl->insert(x);

    //prueba remove
    //avl->remove(4);
    //avl->remove(3);
   


    // prueba de search
   
   //for (auto x:r) cout<<x<<" ";

    /*
    avl->insert(20);

    avl->insert(4);
    avl->insert(12);
    avl->insert(42);
    avl->insert(176);
    */
    avl->printData();

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
    return 0;
}
