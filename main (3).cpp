#include "bits/stdc++.h"

using namespace std;

struct Forest {
    int weight;
    int root;
};

string in_filename;
string temp_filename;
string out_filename;


Forest forest[256];

struct Tree {
    int left;
    int right;
    int parent;
};

Tree tree[520];

FILE *f;
FILE *g;

void getmin(int size_forest, int &pos1, int &pos2) {
    pos1 = 0;
    pos2 = 1;
    if (forest[0].weight > forest[1].weight) {
        pos1 = 1;
        pos2 = 0;
    }
    for (int i = 2; i < size_forest; i++) {
        if (forest[i].weight <= forest[pos1].weight) {
            pos2 = pos1;
            pos1 = i;
        } else if (forest[i].weight < forest[pos2].weight) {
            pos2 = i;
        }
    }
}


void encode() {
    int freq[256];
    int leaf[256];
    for(int i = 0; i < 256; i++){
        freq[i] = 0;
    }
    unsigned char ch;
    f = fopen(in_filename.c_str(), "rb");
    while (fscanf(f, "%c", &ch) != EOF) {
        freq[ch]++;
    }
    fclose(f);
    int alphabet_size = 0;
    int size_forest = 0;
    int size_tree = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i]) {
            forest[size_forest].weight = freq[i];
            forest[size_forest].root = size_forest;
            leaf[i] = size_forest;
            size_forest++;
            tree[size_tree].parent = -1;
            tree[size_tree].right = -1;
            tree[size_tree].left = -1;
            size_tree++;
            alphabet_size++;
        }
    }
    int pos1, pos2;
    while (size_forest > 1) {
        getmin(size_forest, pos1, pos2);
        tree[forest[pos1].root].parent = size_tree;
        tree[forest[pos2].root].parent = size_tree;
        forest[pos1].weight += forest[pos2].weight;
        tree[size_tree].parent = -1;
        tree[size_tree].left = forest[pos1].root;
        tree[size_tree].right = forest[pos2].root;
        forest[pos1].root = size_tree;
        for (int i = pos2; i + 1 < size_forest; i++) {
            swap(forest[i], forest[i + 1]);
        }
        size_forest--;
        size_tree++;
    }

    f = fopen(in_filename.c_str(), "rb");
    vector<bool> ans;
    while (fscanf(f, "%c", &ch) != -1) {
        vector<bool> letter_code;
        int v = leaf[ch];
        while (tree[v].parent != -1) {
            letter_code.push_back(tree[tree[v].parent].right == v);
            v = tree[v].parent;
        }
        for (int i = int(letter_code.size()) - 1; i >= 0; i--) {
            ans.push_back(letter_code[i]);
        }
    }
    fclose(f);

    g = fopen(temp_filename.c_str(), "wb");
    fprintf(g, "%d\n", alphabet_size);
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            vector<bool> letter_code;
            int v = leaf[i];
            while (tree[v].parent != -1) {
                letter_code.push_back(tree[tree[v].parent].right == v);
                v = tree[v].parent;
            }
            unsigned char t[letter_code.size() + 1];
            t[letter_code.size()] = 0;
            for (int j = 0; j < letter_code.size(); j++) {
                t[j] = (unsigned char) (letter_code[letter_code.size() - 1 - j] + '0');
            }
            fprintf(g, "%d %s %c\n", int(letter_code.size()), t, i);
        }
    }
    int sz = ans.size();
    fprintf(g, "%d\n", sz);
    fprintf(g, "*");
    unsigned char out = 0;
    for (int i = 0; i < ans.size(); i++) {
        out = (out << 1) | ans[i];
        if (i % 8 == 7) {
            fprintf(g, "%c", out);
            out = 0;
        }
    }
    if (ans.size() % 8 != 0) {
        out = out << (8 - ans.size() % 8);
        fprintf(g, "%c", out);
    }
    fclose(g);
}

struct Node {
    int left = -1;
    int right = -1;
    unsigned char symbol = 0;
};

void decode() {
    f = fopen(temp_filename.c_str(), "rb");
    int alphabet_size;
    fscanf(f, "%d\n", &alphabet_size);
    int root = 0;
    Node alphabet[520];
    int last_node = 1;
    for (int i = 0; i < alphabet_size; i++) {
        int ln;
        fscanf(f, "%d", &ln);
        unsigned char s[ln];
        unsigned char ch;
        fscanf(f, " %s%c", s, &ch);
        fscanf(f, "%c", &ch);
        int v = root;
        for (int j = 0; j < ln; j++) {
            if (alphabet[v].left == -1) {
                alphabet[v].left = last_node;
                last_node++;
                alphabet[v].right = last_node;
                last_node++;
            }
            if (s[j] == '0') {
                v = alphabet[v].left;
            } else {
                v = alphabet[v].right;
            }
        }
        alphabet[v].symbol = ch;
    }
    int sz;
    fscanf(f, "%d", &sz);
    g = fopen(out_filename.c_str(), "wb");
    unsigned char ch;
    int v = root;
    fscanf(f, "%c", &ch);
    while (ch != '*') {
        fscanf(f, "%c", &ch);
    }
    while (sz > 0) {
        fscanf(f, "%c", &ch);
        for (int i = 0; i < 8 && sz > 0; i++) {
            sz--;
            if ((ch >> (7 - i) & 1) == 0) {
                v = alphabet[v].left;
            } else {
                v = alphabet[v].right;
            }
            if (alphabet[v].left == -1 && alphabet[v].right == -1) {
                unsigned char x = alphabet[v].symbol;
                fprintf(g, "%c", x);
                v = root;
            }
        }
    }
    fclose(g);
    fclose(f);
}

int32_t main(int argc, char *argv[]) {
    if (!strcmp("encode", argv[1])) {
        in_filename = argv[3];
        temp_filename = argv[2];
        encode();
    } else {
        temp_filename = argv[2];
        out_filename = argv[3];
        decode();
    }

    return 0;
}