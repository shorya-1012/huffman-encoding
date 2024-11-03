CC=g++
CFLAGS=-Wall -Wextra -g

huffman: huffman_encoding.cpp
	${CC} ${CFLAGS} -o huffman huffman_encoding.cpp

clean:
	rm huffman
