#ifndef HUFFMAN_ENTROPY_CODER_H
#define HUFFMAN_ENTROPY_CODER_H

#include "entropy_coder.h"

#include <string.h>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <queue>
#include <string>
#include <bitset>

using namespace std;

struct Huffman_Node {
public:
    Huffman_Node(char c, int frequency) :
        c(c), frequency(frequency), lch(NULL), rch(NULL){}
    char c;
    int frequency;

    Huffman_Node *lch, *rch;
};

class Huffman_Entropy_Coder : public Entropy_Coder {
public:
    void encode(string &in, stringstream &out);
    void decode(string &in, stringstream &out);
private:
    void build_freq_table(string &in, unordered_map<char, int> &freq_table);
    void build_huffman_tree(unordered_map<char, int> &freq_table, Huffman_Node *&root);
    string build_huffman_table(Huffman_Node *root, unordered_map<char, vector<bool>> &huffman_table);
    void build_huffman_table(map<unsigned short, vector<char>> &inverted_length_table, unordered_map<char, vector<bool>> &huffman_table);
    void build_huffman_table(map<unsigned short, vector<char>> &inverted_length_table, unordered_map<vector<bool>, char> &huffman_table);
    void rebuild_huffman_table(string &in, unordered_map<vector<bool>, char> &huffman_table);
    void clear_tree(Huffman_Node *node);
    void record_length(Huffman_Node *node, int length, map<char, unsigned short> &length_table);
    void print_tree(Huffman_Node *node, string prefix, string code);
    void vector_increase(vector<bool> &v);
};

#endif
