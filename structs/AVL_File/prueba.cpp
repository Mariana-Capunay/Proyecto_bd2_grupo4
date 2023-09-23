#include "avl_file.h"
#include <vector>
using namespace std;

int main(){

    AVLFile<int>* avl = new AVLFile<int>("filename.bin", "atributo");
    vector<int> a = {6,7,8,9,1,2,3,4};
    for (auto x:a) avl->insert(x);

    //prueba remove
   avl->remove(4);


    // prueba de search
   vector<long> r = avl->search(5);
   for (auto x:r) cout<<x<<" ";

    /*
    avl->insert(20);

    avl->insert(4);
    avl->insert(12);
    avl->insert(42);
    avl->insert(176);
    */
    avl->printData();
    avl->deleteFiles();
    return 0;
}