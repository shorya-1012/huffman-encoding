# Huffman Encoding Algorithm

This repository contains an implementation of the **Huffman Encoding Algorithm** in C++. The Huffman algorithm is a popular losless compression algorithm used to encode data by assigning shorter codes to frequently occurring characters.

## Features

- **Encoding**: Compress text files to reduce file size.
- **Decoding**: Decompress encoded files back to their original form.

## Getting Started

### Prerequisites

Make sure you have a C++ compiler installed, such as `g++`. This program uses a `Makefile` for easy compilation.

### Compilation

1. Compile the program:
   ```bash
   make
   ```

### Usage

After compiling, you can use the program with the following flags:

- `-e`: Encode a file
- `-d`: Decode an encoded file

#### Examples

To encode a file:

```bash
./huffman -e sample.txt
```

This will create a file named encoded.dat in the same directory as the program.

To decode an encoded file:

```bash
./huffman -d encoded.dat
```

This will create a file named decoded.txt in the same directory as the program.

### File Structure

- **sample.txt**: Example text file to encode.
- **encoded.dat**: Output file after encoding (binary format).
- **decoded.txt**: Output file after decoding, which should match the original content.
