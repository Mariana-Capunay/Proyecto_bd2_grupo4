#include "avl_file.h"
#include <vector>

int main(){

    AVLFile<int>* avl = new AVLFile<int>("filename.bin", "atributo");
    vector<int> a = {6,8,7,9,1,2,3,4};
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
    vector<long> r = avl->search(4);
    for (auto x:r) cout<<x<<" ";
    avl->deleteFiles();
    return 0;
}
