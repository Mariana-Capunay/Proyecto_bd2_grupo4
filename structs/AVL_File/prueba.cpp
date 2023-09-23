#include "avl_file.h"
#include <vector>
using namespace std;

int main(){

    AVLFile<int>* avl = new AVLFile<int>("filename.bin", "atributo");
    vector<int> a = {23,5,7,3,546,13,56};
    for (auto x:a) avl->insert(x);
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