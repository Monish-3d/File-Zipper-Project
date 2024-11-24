#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <bitset>

using namespace std;

struct ByteNode {
    ByteNode* left;
    ByteNode* right;
    char data;
    int frequency;

    ByteNode(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(ByteNode* left, ByteNode* right) {
        return left->frequency > right->frequency;
    }
};

class HuffCompression {
private:
    static string sb;
    static unordered_map<char, string> huffmap;

public:
    static void compress(const string& src, const string& dst) {
        ifstream inStream(src, ios::binary);
        vector<char> bytes((istreambuf_iterator<char>(inStream)), istreambuf_iterator<char>());
        inStream.close();

        vector<char> huffmanBytes = createZip(bytes);

        ofstream outStream(dst, ios::binary);
        outStream.write(reinterpret_cast<const char*>(&huffmanBytes[0]), huffmanBytes.size());
        outStream.close();

        // Saving these Huffman codes at same destination as compressed file
        ofstream huffmapFile(dst + ".huffmap");
        for (const auto& pair : huffmap) {
            huffmapFile << static_cast<int>(pair.first) << ' ' << pair.second << '\n';
        }
        huffmapFile.close();

        cout << "Compression complete. Saved Huffman Codes:\n";
    
    }

private:
    static vector<char> createZip(const vector<char>& bytes) {
        priority_queue<ByteNode*, vector<ByteNode*>, Compare> nodes = getByteNodes(bytes);
        ByteNode* root = createHuffmanTree(nodes);
        unordered_map<char, string> huffmanCodes = getHuffCodes(root);
        vector<char> huffmanCodeBytes = zipBytesWithCodes(bytes, huffmanCodes);
        return huffmanCodeBytes;
    }

    static priority_queue<ByteNode*, vector<ByteNode*>, Compare> getByteNodes(const vector<char>& bytes) {
        priority_queue<ByteNode*, vector<ByteNode*>, Compare> nodes;
        unordered_map<char, int> tempMap;
        for (char b : bytes) {
            tempMap[b]++;
        }
        for (const auto& entry : tempMap) {
            nodes.push(new ByteNode(entry.first, entry.second));
        }
        return nodes;
    }

    static ByteNode* createHuffmanTree(priority_queue<ByteNode*, vector<ByteNode*>, Compare>& nodes) {
        while (nodes.size() > 1) {
            ByteNode* left = nodes.top();
            nodes.pop();
            ByteNode* right = nodes.top();
            nodes.pop();
            ByteNode* parent = new ByteNode('\0', left->frequency + right->frequency);
            parent->left = left;
            parent->right = right;
            nodes.push(parent);
        }
        return nodes.top();
    }

    static unordered_map<char, string> getHuffCodes(ByteNode* root) {
        if (root == nullptr) return unordered_map<char, string>();
        getHuffCodes(root->left, "0", sb);
        getHuffCodes(root->right, "1", sb);
        return huffmap;
    }

    static void getHuffCodes(ByteNode* node, const string& code, string& sb) {
        string sb2 = sb + code;
        if (node != nullptr) {
            if (node->data == '\0') {
                getHuffCodes(node->left, "0", sb2);
                getHuffCodes(node->right, "1", sb2);
            } else {
                huffmap[node->data] = sb2;
            }
        }
    }

    static vector<char> zipBytesWithCodes(const vector<char>& bytes, const unordered_map<char, string>& huffCodes) {
        string strBuilder;
        for (char b : bytes) {
            strBuilder += huffCodes.at(b);
        }

        int length = (strBuilder.length() + 7) / 8;
        vector<char> huffCodeBytes(length);
        for (int i = 0, idx = 0; i < strBuilder.length(); i += 8, idx++) {
            string strByte = strBuilder.substr(i, 8);
            bitset<8> bits(strByte);
            huffCodeBytes[idx] = static_cast<char>(bits.to_ulong());
        }

        // adding the padding length to the compressed data
        int paddingBits = (8 - (strBuilder.length() % 8)) % 8;
        huffCodeBytes.push_back(static_cast<char>(paddingBits));

        return huffCodeBytes;
    }

public:
    static void decompress(const string& src, const string& dst) {
        ifstream inStream(src, ios::binary);
        vector<char> huffmanBytes((istreambuf_iterator<char>(inStream)), istreambuf_iterator<char>());
        inStream.close();

        unordered_map<char, string> huffmanCodes;
        ifstream huffmapFile(src + ".huffmap");

        if (!huffmapFile.is_open()) {
            cout << "Failed to open the .huffmap file." << endl;
            return;
        }

        int asciiVal;
        string code;
        while (huffmapFile >> asciiVal >> code) {
            huffmanCodes[static_cast<char>(asciiVal)] = code;
        }
        huffmapFile.close();

        vector<char> bytes = decomp(huffmanCodes, huffmanBytes);

        ofstream outStream(dst, ios::binary);
        outStream.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
        outStream.close();

        cout << "Decompression complete.\n";
    }

    static vector<char> decomp(const unordered_map<char, string>& huffmanCodes, const vector<char>& huffmanBytes) {
        int paddingBits = static_cast<int>(huffmanBytes.back());
        string sb;
        for (size_t i = 0; i < huffmanBytes.size() - 1; i++) {
            bitset<8> bits(huffmanBytes[i]);
            sb += bits.to_string();
        }

        // Removing padding
        sb = sb.substr(0, sb.length() - paddingBits);

        unordered_map<string, char> reverseMap;
        for (const auto& entry : huffmanCodes) {
            reverseMap[entry.second] = entry.first;
        }

        vector<char> list;
        size_t i = 0;
        while (i < sb.length()) {
            size_t count = 1;
            while (i + count <= sb.length() && reverseMap.find(sb.substr(i, count)) == reverseMap.end()) {
                count++;
            }
            if (i + count <= sb.length()) {
                list.push_back(reverseMap[sb.substr(i, count)]);
                i += count;
            } else {
                cout << "Error: Unable to decode sequence at index " << i << endl;
                break;
            }
        }

        return list;
    }
};

string HuffCompression::sb;
unordered_map<char, string> HuffCompression::huffmap;

int main() {
    string sourceFile = "";
    string compressedFile = "";
    string decompressedFile = "";

    //HuffCompression::compress(sourceFile, compressedFile);
    //HuffCompression::decompress(compressedFile, decompressedFile);

    return 0;
}

