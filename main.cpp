#include <iostream>
#include "dataset_bin/binary_conversor.h"

using namespace std;

int main(){
    string atr_1;
    string atr_2;
    string atr_3;
    string atr_4;
    string atr_5;
    string new_file = conversor("dataset/10k/dataset_1.csv", atr_1, atr_2, atr_3, atr_4, atr_5);
    cout << new_file << endl;
    cout<<atr_1<<endl;
    cout<<atr_2<<endl;
    cout<<atr_5<<endl;
    return 0;
}
