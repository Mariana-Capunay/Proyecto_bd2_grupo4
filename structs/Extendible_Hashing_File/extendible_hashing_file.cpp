#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <utility>
#include "dataset_bin/binary_conversor.h"

using namespace std;
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
    private:
    string datafile;
    int max_depth = 20; // MAX DEPTH
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
        file.open(overflow_indexfile,ios::binary);
        file.seekp(last_position_overflow_register);
        file.write((char*)&key, 4);
        file.seekp(last_position_overflow_register+4);
        file.write((char*)&address, 4);
        file.close();
        n_data_overflow++;
    }

    void create_node(int position, BinaryNode node){
        ofstream file;
        file.open(directoryfile,ios::binary);
        file.seekp(position);
        file.write((char*)&node, sizeof(BinaryNode));
        file.close();
        if(last_position == position)
            last_position += sizeof(BinaryNode);
        
    }
    void create_bucket(int position, Bucket bucket){
        ofstream file;
        file.open(indexfile,ios::binary);
        for(int i = 0; i < max_size_bucket; i++){
            file.seekp(position + 4*i);
            file.write((char*)&bucket.keys[i], 4);
            file.seekp(position + 4*i + 4);
            file.write((char*)&bucket.addresses[i], 4);
        }
        
        file.close();
        if(last_bucket == position)
            last_bucket += sizeof(bucket);
    }
    
    pair<int,int> read_data_overflow(int position){
        ifstream file;
        file.open(directoryfile,ios::binary);
        int key,address;
        file.seekg(position);
        file.read((char*)&key, 4);
        file.seekg(position+4);
        file.read((char*)&address, 4);
        file.close();
        return {key,address};
    }
    BinaryNode read_node(int position){
        ifstream file;
        file.open(directoryfile,ios::binary);
        
        BinaryNode node;
        file.seekg(position);
        file.read((char*)&node.left, 4);
        file.seekg(position+4);
        file.read((char*)&node.right, 4);
        file.seekg(position+8);
        file.read((char*)&node.posBucket, 4);
        file.close();
        return node;
    }
    Bucket read_bucket(int position){
        ifstream file;
        file.open(indexfile,ios::binary);
        vector<int> keys,addresses;
        for(int i = 0; i < max_size_bucket; i++){
            int value;
            file.seekg(position+ 4*i);
            file.read((char*)&value, sizeof(int));
            keys.push_back(value);
            file.seekg(position+ 4*i+4);
            file.read((char*)&value, sizeof(int));
            addresses.push_back(value);
        }
        file.close();
        Bucket bucket(max_size_bucket);
        bucket.keys = keys;
        bucket.addresses = addresses;
        return bucket;
    }
    void split_leaf_node(int position, BinaryNode node,int depth){
        if(node.posBucket != -1) return;

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
                left_bucket.keys[pos_left] = left_bucket.keys[i];
                left_bucket.addresses[pos_left] = left_bucket.addresses[i];
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
    }

    int f_hash(int key){
        unsigned hashing = 0,prime = 53, pot = 1;
        while(key){
            int d = key % 10;
            hashing =  d*pot;
            pot = pot * prime;
            key /= 10;
        }
        return key;
    }
    void insert(int key,int address,BinaryNode current_node,int position, int depth){
        int hash_key = f_hash(key);
        if(current_node.posBucket == -1){
            if(hash_key & (1<<depth)) current_node = read_node(current_node.right);
            else current_node = read_node(current_node.left);
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
                insert(key,address,current_node, position, depth);
            }
            else{
                create_data_overflow(key,address);
            }
        }
    }
    int find(int key){
        BinaryNode current_node = root;
        int hash_key = f_hash(key),depth  = 0;
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
    public:
    HashingIndex(string datafile, int max_size_bucket = 5, int max_depth = 20){
        root.left = last_position;
        create_node(last_position,{-1,-1,last_bucket});
        create_bucket(last_bucket,Bucket(max_size_bucket));
        root.right = last_position;
        create_node(last_position,{-1,-1,last_bucket});
        create_bucket(last_bucket,Bucket(max_size_bucket));
        root.posBucket = -1;
    }
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
};
/*
int main(){

    return 0;
}*/