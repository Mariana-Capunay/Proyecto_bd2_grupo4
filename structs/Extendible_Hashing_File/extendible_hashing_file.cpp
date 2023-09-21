#include <string>
using namespace std;

struct BinaryNode{
    int left = -1;
    int right = -1;
    int posBucket = -1;
};

template<typename TK>
class HashingIndex{
    string indexfile= "hashing_index.dat";
    string directoryfile = "hashing_directory.dat";
    string datafile;
    int D = 20; // MAX DEPTH
    int max_size_bucket = 5; // MAX SIZE IN BUCKET
    int last_position = 0;

    HashingIndex(){
        
    }
    void insert(TK key){

    }

};

