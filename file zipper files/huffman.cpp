#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

class HuffmanCoding {
private:
    
    unordered_map<char, string> codes;

    
    void buildCodes(Node* root, const string &code) {
        if (!root) return;
        if (!root->left && !root->right) {
            codes[root->ch] = code;
        }
        buildCodes(root->left, code + "0");
        buildCodes(root->right, code + "1");
    }

public:
    // Compress the input file using Huffman coding
    void compress(const string &inputFile, const string &compressedFile, const string &codeFile) {
        // Read the entire input file into a string
        ifstream in(inputFile, ios::binary);
        if (!in) {
            cerr << "Error: Cannot open input file " << inputFile << endl;
            return;
        }
        stringstream buffer;
        buffer << in.rdbuf();
        string text = buffer.str();
        in.close();

        // Build frequency map for each character
        unordered_map<char, int> freq;
        for (char c : text) {
            freq[c]++;
        }

        // Build the Huffman Tree using a priority queue.
        priority_queue<Node*, vector<Node*>, Compare> pq;
        for (auto &p : freq) {
            pq.push(new Node(p.first, p.second));
        }
        while (pq.size() > 1) {
            Node* left = pq.top(); pq.pop();
            Node* right = pq.top(); pq.pop();
            Node* parent = new Node('\\0', left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            pq.push(parent);
        }
        Node* root = pq.top();
        buildCodes(root, "");

        // Encode the text into a string of '0's and '1's
        string encoded;
        for (char c : text) {
            encoded += codes[c];
        }

        int padding = (8 - (encoded.size() % 8)) % 8;
        for (int i = 0; i < padding; i++) {
            encoded.push_back('0');
        }
        vector<unsigned char> packedBytes;
        for (size_t i = 0; i < encoded.size(); i += 8) {
            string byteStr = encoded.substr(i, 8);
            unsigned char byte = 0;
            for (int j = 0; j < 8; j++) {
                byte = (byte << 1) | (byteStr[j] - '0');
            }
            packedBytes.push_back(byte);
        }

        // Write the compressed data in binary mode.
        // First byte indicates how many padding bits were added.
        ofstream outComp(compressedFile, ios::binary);
        if (!outComp) {
            cerr << "Error: Cannot open compressed file for writing." << endl;
            return;
        }
        unsigned char padByte = static_cast<unsigned char>(padding);
        outComp.write(reinterpret_cast<const char*>(&padByte), sizeof(padByte));
        outComp.write(reinterpret_cast<const char*>(packedBytes.data()), packedBytes.size());
        outComp.close();

        // Write the Huffman codes mapping to a text file.
        // Each line is formatted as: <ASCII value> <Huffman code>
        ofstream outCode(codeFile);
        if (!outCode) {
            cerr << "Error: Cannot open code file for writing." << endl;
            return;
        }
        for (auto &p : codes) {
            outCode << static_cast<int>(p.first) << " " << p.second << "\n";
        }
        outCode.close();

        cout << "Compression complete. Compressed data written to \"" << compressedFile 
             << "\" and codes written to \"" << codeFile << "\"." << endl;
    }

    // Decompress the file
    void decompress(const string &compressedFile, const string &codeFile, const string &outputFile) {
        // Rebuild the reverse mapping: Huffman code to character.
        ifstream inCode(codeFile);
        if (!inCode) {
            cerr << "Error: Cannot open code file " << codeFile << endl;
            return;
        }
        unordered_map<string, char> revCodes;
        int ascii;
        string code;
        while (inCode >> ascii >> code) {
            revCodes[code] = static_cast<char>(ascii);
        }
        inCode.close();

        // Read the compressed file in binary mode.
        ifstream inComp(compressedFile, ios::binary);
        if (!inComp) {
            cerr << "Error: Cannot open compressed file " << compressedFile << endl;
            return;
        }
        // read the padding information (first byte).
        unsigned char padByte;
        inComp.read(reinterpret_cast<char*>(&padByte), sizeof(padByte));
        int padding = static_cast<int>(padByte);
        // Read the rest of the bytes.
        vector<unsigned char> packedBytes((istreambuf_iterator<char>(inComp)), istreambuf_iterator<char>());
        inComp.close();

        // Unpack the bytes into a string of bits.
        string encoded;
        for (unsigned char byte : packedBytes) {
            bitset<8> bits(byte);
            encoded += bits.to_string();
        }
        // Remove the padding bits at the end.
        if (padding > 0) {
            encoded.erase(encoded.end() - padding, encoded.end());
        }

        // Decode the encoded bit-string using the reverse mapping.
        string decoded;
        string temp;
        for (char bit : encoded) {
            temp.push_back(bit);
            if (revCodes.find(temp) != revCodes.end()) {
                decoded.push_back(revCodes[temp]);
                temp.clear();
            }
        }

        // Write the decoded (original) text to the output file.
        ofstream out(outputFile, ios::binary);
        if (!out) {
            cerr << "Error: Cannot open output file " << outputFile << endl;
            return;
        }
        out << decoded;
        out.close();

        cout << "Decompression complete. Decoded data written to \"" << outputFile << "\"." << endl;
    }
};

int main() {
    HuffmanCoding huffman;
    
    string inputFile = "";          // Original file to compress.
    string compressedFile = "";  // File to store encoded data.
    string codeFile = "";             // File to store the Huffman codes.
    string outputFile = "";      // decompressed file

    // Compress the input file
    huffman.compress(inputFile, compressedFile, codeFile);
    
    // Decompress the file back to text
    huffman.decompress(compressedFile, codeFile, outputFile);
    
    return 0;
}
