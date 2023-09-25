
//#include "dataset_bin/binary_conversor.h"
//#include "structs/AVL_File/avl_file.h"
#include "structs/Extendible_Hashing_File/hashing.h"

using namespace std;
int main(){

    HashingIndex hashing_index("test.bin");
    hashing_index.load_file();
    
    return 0;
}
