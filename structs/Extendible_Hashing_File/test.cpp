#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <utility>

using namespace std;

struct Record {
    int key= {};                 // int (dni o codigo)
    char atrib1[40] = {};        // char de 40 (nombre completo o nombre producto)
    int atrib2 = {};             // int (edad o stock)
    char atrib3[25] = {};        // char de 25 (ciudad o categoria)
    float atrib4 = {};           // float (sueldo, precio)
    bool removed   ;            // bool (removed)

    //friend ofstream& operator<<(ofstream& salida, Record r);
    void print(){
        //std::cout<<key;
        //std::cout<<", ";
        //std::cout<<atrib1;
        //std::cout<<", ";
        //std::cout<<atrib2<<", ";
        //std::cout<<atrib3<<", ";
        //std::cout<<atrib4<<"\n";
    }
    int size(){
        return sizeof(int)*2 + 40 + 25 + 4 + sizeof(bool);
    }
};

Record read_record(std::string file_name, int pos){
    std::ifstream file(file_name, std::ios::binary);
    if (!file.is_open()) {throw std::runtime_error("No se pudo abrir archivo ");}
    Record record;
    file.seekg(0, std::ios::end);
    int dif = file.tellg();
    if ( dif <=pos) {throw std::runtime_error("No existe registro en esta posicion");}
    file.seekg(0,std::ios::beg);
    file.seekg(pos); // los punteros tienen posicion exacta del registro


    file.read((char*)&record.key,sizeof(record.key));
    file.read((char*)&record.atrib1,sizeof(record.atrib1));
    file.read((char*)&record.atrib2,sizeof(record.atrib2));
    file.read((char*)&record.atrib3,sizeof(record.atrib3));
    file.read((char*)&record.atrib4,sizeof(record.atrib4));
    file.read((char*)&record.removed,sizeof(record.removed));

    if (!record.removed){
        record.print();
    }

    file.close();
    return record;
}

struct BinaryNode{
    int left = -1;
    int right = -1;
    int posBucket = -1;
};

struct Bucket{
    vector<int> keys;
    vector<int> addresses; 
    Bucket(int size_bucket){
        keys = addresses = vector<int>(size_bucket,-1);
    }
};


class HashingIndex{
    public:
    string datafile;
    int max_depth = 25; // MAX DEPTH
    int max_size_bucket = 5; // MAX SIZE IN BUCKET
    
    string indexfile= "hashing_index.dat";
    string overflow_indexfile = "hashing_overflow_index.dat";
    string directoryfile = "hashing_directory.dat";
    
    int last_position = 0;
    int last_bucket = 0;
    int last_position_overflow_register = 0;
    int n_data_overflow = 0;
    BinaryNode root;

    void create_data_overflow(int key,int address){
        ofstream file;
        file.open(overflow_indexfile, ios::binary| ios::in);
        file.seekp(last_position_overflow_register,ios::beg);
        file.write((char*)&key, 4);
        file.seekp(last_position_overflow_register+4,ios::beg);
        file.write((char*)&address, 4);
        file.close();
        n_data_overflow++;
    }

    void create_node(int position, BinaryNode node){
        ofstream file;
        file.open(directoryfile, ios::binary | ios::in);
        file.seekp(position,ios::beg);
        file.write((char*)&node, sizeof(BinaryNode));
        file.close();
        if(last_position == position)
            last_position += sizeof(BinaryNode);
        
    }
    void create_bucket(int position, Bucket bucket){
        ofstream file;
        file.open(indexfile, ios::binary | ios::in);
        for(int i = 0; i < max_size_bucket; i++){
            file.seekp(position + 8*i,ios::beg);
            file.write((char*)&bucket.keys[i], 4);
            file.seekp(position + 8*i + 4,ios::beg);
            file.write((char*)&bucket.addresses[i], 4);
        }
       
        file.close();
        
        if(last_bucket == position)
            last_bucket += max_size_bucket*4*2;
    }
    
    pair<int,int> read_data_overflow(int position){
        ifstream file;
        file.open(overflow_indexfile,ios::binary | ios::in);
        int key,address;
        file.seekg(position,ios::beg);
        file.read((char*)&key, 4);
        file.seekg(position+4,ios::beg);
        file.read((char*)&address, 4);
        file.close();
        return {key,address};
    }
    BinaryNode read_node(int position){
        ifstream file;
        file.open(directoryfile,ios::binary | ios::in);
        
        BinaryNode node;
        file.seekg(position,ios::beg);
        file.read((char*)&node.left, 4);
        file.seekg(position+4,ios::beg);
        file.read((char*)&node.right, 4);
        file.seekg(position+8,ios::beg);
        file.read((char*)&node.posBucket, 4);
        file.close();
        return node;
    }
    Bucket read_bucket(int position){
        ifstream file;
        file.open(indexfile, ios::binary | ios::in);
        Bucket bucket(max_size_bucket);
        for(int i = 0; i < max_size_bucket; i++){
            int key,address = 0;
            file.seekg(position+ 8*i,ios::beg);
            file.read((char*)&key, 4);
            bucket.keys[i] = key ;
            file.seekg(position+ 8*i+4,ios::beg);
            file.read((char*)&address, 4);
            bucket.addresses[i] = address ;
        }
        file.close();    
        return bucket;
    }
    void split_leaf_node(int position, BinaryNode node,int depth){
        if(node.posBucket == -1) return;
        int position_bucket = node.posBucket;
        Bucket current_bucket = read_bucket(position_bucket);
        Bucket left_bucket(max_size_bucket); 
        Bucket right_bucket(max_size_bucket);
        int pos_left = 0, pos_right = 0;
        for(int i = 0; i < max_size_bucket; i++){
            if(current_bucket.keys[i] & (1<<depth)){
                  
                right_bucket.keys[pos_right] = current_bucket.keys[i];
                right_bucket.addresses[pos_right] = current_bucket.addresses[i];
                pos_right++;
            }
            else{
              
                left_bucket.keys[pos_left] = current_bucket.keys[i];
                left_bucket.addresses[pos_left] = current_bucket.addresses[i];
                pos_left++;
            }
        }   

       
        node.left = last_position;
        create_node(last_position,{-1,-1,position_bucket});
        create_bucket(position_bucket,left_bucket);
        
        node.right = last_position;
        create_node(last_position,{-1,-1,last_bucket});
        create_bucket(last_bucket,right_bucket);

        node.posBucket = -1;
        create_node(position,node);
        BinaryNode x = read_node(position);
  
    }

    int f_hash(int key){
        unsigned hashing = 0,prime = 53, pot = 1;
        while(key){
            int d = key % 10;
            hashing =  d*pot;
            pot = pot * prime;
            key /= 10;
        }
        int result = hashing;
        if(result < 0) result *= -1;
        return result;
    }
    void insert(int key,int address,BinaryNode current_node,int position, int depth){
        int hash_key = f_hash( key);
        if(current_node.posBucket == -1){
            if(hash_key & (1<<depth)) 
                position = current_node.right,current_node = read_node(current_node.right);
            else 
                position = current_node.left, current_node = read_node(current_node.left);
    
            insert(key,address, current_node,position, depth + 1);
        }
        else{
           
            Bucket current_bucket = read_bucket(current_node.posBucket);
            int empty_pos = 0;
            while(empty_pos < max_size_bucket && current_bucket.keys[empty_pos] != -1) empty_pos++;
            if(empty_pos < max_size_bucket){
        
                current_bucket.keys[empty_pos] = key;
                current_bucket.addresses[empty_pos] = address;
              
             
                create_bucket(current_node.posBucket,current_bucket);

               
            } 
            else if(depth < max_depth){
                split_leaf_node(position,current_node, depth);
                current_node = read_node(position);
                insert(key,address,current_node, position, depth);
            }
            else{
                return ;
                create_data_overflow(key,address);
            }
        }
    }
    public:
    int find(int key){
        BinaryNode current_node = root;
        int hash_key = f_hash( key),depth  = 0;
        while(current_node.posBucket == -1){
            if(hash_key & (1<<depth)) current_node = read_node(current_node.right);
            else current_node = read_node(current_node.left);
            depth++;
        }
        Bucket bucket = read_bucket(current_node.posBucket);
        for(int i = 0; i < max_size_bucket; i++){
            if(bucket.keys[i] == key)
                return bucket.addresses[i];
        }
        if(depth == max_size_bucket && bucket.keys[max_size_bucket-1] != -1){
            int cur_position = 0;
            for(int i = 0; i < n_data_overflow; i++){
                pair<int,int> data_overflow = read_data_overflow(cur_position);
                if(data_overflow.first == key)
                    return data_overflow.second;
                cur_position += 8;
            }
        }
        return -1;
    }
    HashingIndex(string _datafile, int max_size_bucket = 5, int max_depth = 20){
        datafile = _datafile;
        root.left = last_position;
        create_node(last_position,{-1,-1,last_bucket});
        create_bucket(last_bucket,Bucket(max_size_bucket));
        root.right = last_position;
        create_node(last_position,{-1,-1,last_bucket});
        create_bucket(last_bucket,Bucket(max_size_bucket));
        root.posBucket = -1;
    };
    void insert(int key,int address){
        insert(key,address,root,0,0);
    }
    Record* search(int key){
        int address = find(key);
        if(address == -1) return nullptr;
        Record* record = new Record;
        *record = read_record(datafile,address);
        if(record->removed) return nullptr;
        return record;
    }
    void erase(int key){
        int address = find(key);
        if(address == -1) return;
        Record record = read_record(datafile,address);
        if(record.removed) return;
        record.removed = true;
        ofstream file;
        file.open(datafile,ios_base::binary);
        file.seekp(address);
        file.write((char*)&record, record.size());
        file.close();
    }

    void load_file(int limit = -1){
        last_position = 0;
        last_bucket = 0;
        last_position_overflow_register = 0;
        n_data_overflow = 0;
        ifstream file;
        file.open(datafile,ios::binary | ios::in);

        file.seekg(0,ios_base::end);
        int last_pos = file.tellg();
        file.close();
        int pos = 0;
        int cont = 0;
        while(pos < last_pos){
            if(limit != -1 && cont == limit) break;
            file.seekg(pos,ios::beg);
            Record record = read_record(datafile,pos);
            insert(record.key,pos);
            pos += record.size();
        }
    }

    void generate(int limit){
        last_position = 0;
        last_bucket = 0;
        last_position_overflow_register = 0;
        n_data_overflow = 0;
        for(int i = 0; i < limit; i++) insert(i+1,i+1);
    }
};
#include <stdlib.h> 
#include <ctime> 

int main(){
    unsigned t0, t1;
 
    double time;
    HashingIndex hashing_index("test.bin");
    int n = 1000;
    int sizes[] = {1000,5000,10000,50000,100000} ;
    for(int i = 0; i < 5; i++){
        n = sizes[i];
        cout << "TAMAÑO " << n << endl;
        hashing_index.generate(n);
        t0=clock();
        hashing_index.find( rand() % n );
        t1 = clock();
        time = (double(t1-t0)/CLOCKS_PER_SEC);
        cout << "Search Time : " << time << endl;
        
        t0=clock();
        hashing_index.insert(n + rand() % n,8174287);
        
        t1 = clock();
        time = (double(t1-t0)/CLOCKS_PER_SEC);
        cout << "Insert Time : " << time << endl;
    }
    
  
   
    return 0;
}
