#include "huffman_entropy_coder.h"

class Bit_Writer {
public:
    Bit_Writer(stringstream &out) : out(out), n(0){}
    void flush();
    void push(bool bit);
    void push(vector<bool> &bits);
private:
    stringstream &out;
    bitset<8> buffer;
    int n;
};

void Bit_Writer::flush(){
    if (n != 0){
        out << static_cast<unsigned char>(buffer.to_ulong());
        buffer.reset();
    }
}

void Bit_Writer::push(bool bit){
    buffer[n++] = bit;
    if (n == 8){
        flush();
        n = 0;
    }
}

void Bit_Writer::push(vector<bool> &bits){
    for (auto bit : bits){
        push(bit);
    }
}

class Bit_Reader {
public:
    Bit_Reader(string &in) : in(in), ptr(0), n(0){}
    bool read();
    bool empty();
private:
    string in;
    size_t ptr;
    int n;
};

bool Bit_Reader::read(){
    unsigned char c = in[ptr];
    bool bit = c & (unsigned char)(1 << n);
    n++;
    if (n == 8){
        ptr++;
        n = 0;
    }
    return bit;
}

bool Bit_Reader::empty(){
    return ptr >= in.size();
}

void Huffman_Entropy_Coder::encode(string &in, stringstream &out){
    unordered_map<char, int> freq_table;
    build_freq_table(in, freq_table);

    Huffman_Node *root;
    build_huffman_tree(freq_table, root);

    unordered_map<char, vector<bool>> huffman_table;
    string encoded_table;
    encoded_table = build_huffman_table(root, huffman_table);
    clear_tree(root);
    
    size_t len = in.size();
    out << string((char*)&len, sizeof(size_t));
    out << encoded_table;
    Bit_Writer writer(out);
    for (auto c : in){
        writer.push(huffman_table[c]);
    }
    writer.flush();

    return;
}

void Huffman_Entropy_Coder::decode(string &in, stringstream &out){
    size_t len;
    memcpy(&len, &in[0], sizeof(size_t));
    unordered_map<vector<bool>, char> huffman_table;

    in = in.substr(sizeof(size_t));
    rebuild_huffman_table(in, huffman_table);

    Bit_Reader reader(in);
    vector<bool> bits;
    while (!reader.empty()){
        bool bit = reader.read();
        bits.push_back(bit);
        if (huffman_table.find(bits) != huffman_table.end()){
            out << huffman_table[bits];
            bits.clear();
            if (--len == 0)
                break;
        }
    }
}

void Huffman_Entropy_Coder::build_freq_table(string &in, unordered_map<char, int> &freq_table){
    for (auto c : in){
        freq_table[c]++;
    }

    return;
}

void Huffman_Entropy_Coder::build_huffman_tree(unordered_map<char, int> &freq_table, Huffman_Node *&root){
    auto comp = [](Huffman_Node *l, Huffman_Node *r){ return l->frequency > r->frequency; };
    priority_queue<Huffman_Node*, vector<Huffman_Node*>, decltype(comp)> pq(comp);
    for (auto it = freq_table.begin(); it != freq_table.end(); it++){
        Huffman_Node *node = new Huffman_Node(it->first, it->second);
        pq.push(node);
    }

    while (pq.size() > 1){
        Huffman_Node *n1 = pq.top();
        pq.pop();
        Huffman_Node *n2 = pq.top();
        pq.pop();
        Huffman_Node *n3 = new Huffman_Node('.', n1->frequency + n2->frequency);
        n3->lch = n1;
        n3->rch = n2;
        pq.push(n3);
    }

    root = pq.top();
    //print_tree(root, "", "");
}

string Huffman_Entropy_Coder::build_huffman_table(Huffman_Node *root, unordered_map<char, vector<bool>> &huffman_table){
    map<char, unsigned short> length_table;
    record_length(root, 0, length_table);

    map<unsigned short, vector<char>> inverted_length_table;
    for (auto it = length_table.begin(); it != length_table.end(); it++){
        inverted_length_table[it->second].push_back(it->first);
    }

    build_huffman_table(inverted_length_table, huffman_table);

    string s1, s2;
    unsigned int count;
    for (int i = 1; i <= 16; i++){
        count = inverted_length_table[i].size();
        s1 += string((char*)&count, 2);
        for (char c : inverted_length_table[i])
            s2 += string(1, c);
    }
    
    return s1 + s2;
}

void Huffman_Entropy_Coder::build_huffman_table(map<unsigned short, vector<char>> &inverted_length_table, unordered_map<char, vector<bool>> &huffman_table){
    vector<bool> code;
    for (auto it = inverted_length_table.begin(); it != inverted_length_table.end(); it++){
        // grow code to correct length
        while (code.size() < it->first)
            code.push_back(false);

        for (auto c : it->second){
            huffman_table[c] = code;
            vector_increase(code);
        }
    }
}

void Huffman_Entropy_Coder::build_huffman_table(map<unsigned short, vector<char>> &inverted_length_table, unordered_map<vector<bool>, char> &huffman_table){
    vector<bool> code;
    for (auto it = inverted_length_table.begin(); it != inverted_length_table.end(); it++){
        // grow code to correct length
        while (code.size() < it->first)
            code.push_back(false);

        for (auto c : it->second){
            huffman_table[code] = c;
            vector_increase(code);
        }
    }
}

void Huffman_Entropy_Coder::rebuild_huffman_table(string &in, unordered_map<vector<bool>, char> &huffman_table){
    map<unsigned short, vector<char>> inverted_length_table;
    unsigned short count;
    int ptr = 32;
    for (int i = 0; i < 16; i++){
        memcpy(&count, &in[2 * i], 2);
        for (int j = 0; j < count; j++){
            inverted_length_table[i+1].push_back(in[ptr++]);
        }
    }
    build_huffman_table(inverted_length_table, huffman_table);

    in = in.substr(ptr);
}

void Huffman_Entropy_Coder::clear_tree(Huffman_Node *node){
    if (node == NULL)
        return;

    clear_tree(node->lch);
    clear_tree(node->rch);
    delete(node);
    node = NULL;
}

void Huffman_Entropy_Coder::record_length(Huffman_Node *node, int length, map<char, unsigned short> &length_table){
    if (node == NULL)
        return;

    if (node->lch == NULL && node->rch == NULL){
        length_table[node->c] = length;
    }
    else {
        if (node->lch)
            record_length(node->lch, length + 1, length_table);
        if (node->rch)
            record_length(node->rch, length + 1, length_table);
    }
}

void Huffman_Entropy_Coder::print_tree(Huffman_Node *node, string prefix, string code){
    if (node == NULL)
        return;
    cout << prefix << "Node(" << node->c << ") " << node->frequency << " : " << code << endl;
    print_tree(node->lch, prefix + "  ", code + "0");
    print_tree(node->rch, prefix + "  ", code + "1");
}

void Huffman_Entropy_Coder::vector_increase(vector<bool> &v){
    int i;
    for (i = v.size() - 1; i >= 0; i--){
        v[i] = !v[i];
        if (v[i])
            break;
    }
    if (i < 0){
        v.push_back(false);
        v[0] = true;
    }
}
