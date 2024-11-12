#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>
using namespace std;

class Node {
  optional<char> _ch;
  size_t _freq;
  Node *_left;
  Node *_right;

public:
  Node(char _ch, size_t _freq)
      : _ch(_ch), _freq(_freq), _left(nullptr), _right(nullptr) {}
  Node(size_t _freq, Node *left, Node *right)
      : _ch(nullopt), _freq(_freq), _left(left), _right(right) {}

  size_t freq() const { return this->_freq; }
  optional<char> ch() const { return this->_ch; }
  Node *left() const { return this->_left; }
  Node *right() const { return this->_right; }

  friend ostream &operator<<(ostream &out, const Node &n);
};
ostream &operator<<(ostream &out, const Node &n) {
  if (n._ch.has_value()) {
    out << n._ch.value() << " : " << n._freq;
  } else {
    out << n._freq;
  }
  return out;
}

class HuffManTree {
  Node *_root;

  void display_huffman_tree(Node *node, int indent = 0) {
    if (!node)
      return;
    if (node->right()) {
      display_huffman_tree(node->right(), indent + 4);
    }
    for (int i = 0; i < indent; i++) {
      cout << " ";
    }
    cout << *node << std::endl;
    if (node->left()) {
      display_huffman_tree(node->left(), indent + 4);
    }
  }

  void free_tree(Node *node) {
    if (!node)
      return;
    free_tree(node->left());
    free_tree(node->right());
    delete node;
  }

public:
  HuffManTree(Node *root) : _root(root) {}
  Node *root() const { return this->_root; }
  void debug() { display_huffman_tree(this->_root); }
  ~HuffManTree() { free_tree(this->_root); }
};

class PriorityQueue {
  std::vector<Node *> arr;

  size_t parent(int i) const { return (i - 1) / 2; }
  size_t left(int i) const { return (2 * i) + 1; }
  size_t right(int i) const { return (2 * i) + 2; }

  void swap(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
  }

  void heapify(int i) {
    int smallest = i;
    // check for smallest on left
    if (left(i) < arr.size() && arr[left(i)]->freq() < arr[smallest]->freq()) {
      smallest = left(i);
    }
    // check for smallest on right
    if (right(i) < arr.size() &&
        arr[right(i)]->freq() < arr[smallest]->freq()) {
      smallest = right(i);
    }

    if (smallest != i) {
      swap(&arr[i], &arr[smallest]);
      heapify(smallest);
    }
  }

public:
  void push(Node *node) {
    arr.push_back(node);
    int i = arr.size() - 1; // index of newly placed element
    while (i > 0 && arr[i]->freq() < arr[parent(i)]->freq()) {
      swap(&arr[i], &arr[parent(i)]);
      i = parent(i);
    }
  }

  Node *pop() {
    auto val = this->arr[0];
    swap(&arr[0], &arr[arr.size() - 1]);
    this->arr.pop_back();
    heapify(0);
    return val;
  }

  size_t size() const { return this->arr.size(); }
};

unordered_map<char, size_t> calaculate_char_freq(const string file_name) {
  unordered_map<char, size_t> freq;
  ifstream infile(file_name);
  if (!infile.is_open()) {
    cerr << "Unable to open target file" << endl;
    exit(EXIT_FAILURE);
  }
  char chr;
  while (infile.get(chr)) {
    freq[chr]++;
  }
  infile.close();
  return freq;
}

HuffManTree construct_huffman_tree(string str) {
  unordered_map<char, size_t> freq_map = calaculate_char_freq(str);
  PriorityQueue pq;
  for (const auto i : freq_map) {
    pq.push(new Node(i.first, i.second));
  }
  while (pq.size() > 1) {
    auto first = pq.pop();
    auto second = pq.pop();
    size_t merged_freq = first->freq() + second->freq();
    pq.push(new Node(merged_freq, first, second));
  }
  HuffManTree huffman_tree(pq.pop());
  return huffman_tree;
}

void generate_codes(Node *node, unordered_map<char, string> &map,
                    string code = "") {
  if (!node)
    return;
  if (node->ch().has_value()) {
    map[node->ch().value()] = code;
  }
  generate_codes(node->left(), map, code + '0');
  generate_codes(node->right(), map, code + '1');
}

unordered_map<char, string> retrieve_codes(Node *root) {
  unordered_map<char, string> codes;
  generate_codes(root, codes);
  return codes;
}

void write_encoded_file(string encoded_string,
                        unordered_map<char, string> codes_map) {
  ofstream outfile("encoded.dat", ios::binary);
  size_t map_size = codes_map.size();

  // write the map size (helps with decoding)
  outfile.write(reinterpret_cast<const char *>(&map_size), sizeof(map_size));

  for (auto &[chr, code] : codes_map) {
    outfile.write(reinterpret_cast<const char *>(&chr), sizeof(chr));
    size_t code_size = code.size();
    outfile.write(reinterpret_cast<const char *>(&code_size),
                  sizeof(code_size));
    outfile.write(code.c_str(), code_size);
  }

  // write the actual encoded string
  size_t valid_bit_count = encoded_string.size();
  outfile.write(reinterpret_cast<char *>(&valid_bit_count),
                sizeof(valid_bit_count));
  char bit_buffer = 0;
  uint8_t bit_count = 0;

  for (char bit : encoded_string) {
    bit_buffer = (bit_buffer << 1) | (bit - '0');
    bit_count++;

    if (bit_count == 8) {
      outfile.put(bit_buffer);
      bit_buffer = 0;
      bit_count = 0;
    }
  }

  if (bit_count > 0) {
    bit_buffer <<= (8 - bit_count);
    outfile.put(bit_buffer);
  }

  outfile.close();
}

void encode(string file_name) {
  HuffManTree huffman_tree = construct_huffman_tree(file_name);
  /*huffman_tree.debug();*/
  auto codes_map = retrieve_codes(huffman_tree.root());
  ifstream infile(file_name);
  string encoded_str = "";
  char chr;
  while (infile.get(chr)) {
    encoded_str.append(codes_map[chr]);
  }

  write_encoded_file(encoded_str, codes_map);
}

void decode(string file_name) {
  ofstream outfile("decoded.txt");
  ifstream binary_file(file_name);
  if (!binary_file.is_open()) {
    cerr << "Unable to open file" << endl;
    exit(1);
  }
  unordered_map<string, char> codes_map;
  size_t map_size;
  char ch;
  binary_file.read(reinterpret_cast<char *>(&map_size), sizeof(map_size));
  while (map_size) {
    binary_file.read(reinterpret_cast<char *>(&ch), sizeof(ch));
    size_t code_size = 0;
    binary_file.read(reinterpret_cast<char *>(&code_size), sizeof(code_size));
    string code(code_size, '\0');
    binary_file.read(&code[0], code_size);
    codes_map[code] = ch;
    map_size--;
  }
  // decode actual string
  size_t valid_bit_count = 0;
  binary_file.read(reinterpret_cast<char *>(&valid_bit_count),
                   sizeof(valid_bit_count));
  string encoded_str = "";
  char byte;
  size_t bits_read = 0;

  while (binary_file.read(&byte, sizeof(byte))) {
    for (int i = 0; i < 8; i++) {
      if (bits_read >= valid_bit_count) {
        break;
      }
      bool bit = (byte >> (7 - i)) & 1;
      bits_read++;

      encoded_str += bit ? '1' : '0';

      if (codes_map.find(encoded_str) != codes_map.end()) {
        outfile << codes_map[encoded_str];
        encoded_str.clear();
      }
    }
    if (codes_map.find(encoded_str) != codes_map.end()) {
      outfile << codes_map[encoded_str];
      encoded_str.clear();
    }
  }

  binary_file.close();
  outfile.close();
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << "Incorrect Usage" << endl;
    cout << "use : ./huffman [-e | -d] <file-name>" << endl;
    exit(EXIT_FAILURE);
  }
  if (strcmp(argv[1], "-e") == 0) {
    cout << "encoding file ..." << endl;
    encode(argv[2]);
    cout << "File encoded successfully" << endl;
  } else if (strcmp(argv[1], "-d") == 0) {
    cout << "decoding file ..." << endl;
    decode(argv[2]);
    cout << "File decoded successfully" << endl;
  }
  return 0;
}
