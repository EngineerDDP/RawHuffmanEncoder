huffman.so : HuffmanCodec.o HuffmanCodecUInt32_T.o CodecUInt8_T.o pch.o
	    cc -shared HuffmanCodec.o HuffmanCodecUInt32_T.o CodecUInt8_T.o pch.o -o huffman.so

HuffmanCodec.o : HuffmanCodecUInt32_T.o CodecUInt8_T.o pch.o
	    cc -fPIC -I /usr/include/python3.7 -c HuffmanCodec.cpp

pch.o : pch.cpp pch.h
	    cc -fPIC -I /usr/include/python3.7 -c pch.cpp

CodecUInt8_T.o : CodecUInt8_T.cpp CodecUInt8_T.h
	    cc -fPIC -c CodecUInt8_T.cpp

HuffmanCodecUInt32_T.o : HuffmanCodecUInt32_T.cpp HuffmanCodecUInt32_T.h
	    cc -fPIC -c HuffmanCodecUInt32_T.cpp

clean : 
	    rm huffman.so HuffmanCodec.o HuffmanCodecUInt32_T.o CodecUInt8_T.o pch.o
