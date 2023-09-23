#include "avl_file.h"
using namespace std;

int main(){

    AVLFile<int>* avl = new AVLFile<int>("filename.bin", "atributo");
    //avl->insert(4);
    //avl->insert(12);
    avl->insert(176);

    avl->printData();
}