#include <iostream>
#include "dataset_bin/binary_conversor.h"
//#include "structs/AVL_File/avl_file.h"
#include "structs/Extendible_Hashing_File/hashing.h"
using namespace std;
int main(){
    string atr_1;
    string atr_2;
    string atr_3;
    string atr_4;
    string atr_5;
    string new_file = conversor("dataset/10k/dataset_1.csv", atr_1, atr_2, atr_3, atr_4, atr_5);
    HashingIndex test(new_file);
    test.insert(40,123);
    /*
    cout << new_file << endl;
    cout<<atr_1<<endl;
    cout<<atr_2<<endl;
    cout<<atr_5<<endl;
    */
   Record record;
   cout<<record.size();
   //read_record(new_file,0);  // para primer registro (en posicion 0)
   //read_record(new_file,record.size()*9999); // para registro en posicion 10k
    return 0;
}
