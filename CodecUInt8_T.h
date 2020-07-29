#pragma once

#include "HuffmanCodecUInt32_T.h"
#include <iostream>

using std::exception;

typedef char T_Code;


class UndefinedSourceCode : exception {
private:
	int source;
public:
	UndefinedSourceCode(int src) : source(src) {}
};

// 使用Huffman不定长编码进行编码的编码器
class CharCodex {
private:
	HuffmanTree<T_Code>* tree;
private:
	// 构造空编码器
	CharCodex() = delete;
	// 写入哈夫曼编码表，尚未实现
	void encode_codec(char* buffer, int len) const;
	// 解析哈夫曼编码表，尚未实现
	void decode_codec(char* source_buffer, int source_len) const;
public:
	// 使用给定的编码和出现概率构造编码器
	// @param codes:原始码	
	// @param value:概率密度
	// @param len:	原始码数组长度
	CharCodex(const T_Code* codes, const double* value, int len) {
		tree = new HuffmanTree<char>();
		int* values = new int[len];
		for (int i = 0; i < len; ++i) values[i] = static_cast<int>(100 * value[i]);
		tree->buildTree(codes, values, len);
	}

	// 使用给定数量的原始码构建编码器
	// 实现的是等概率变长编码
	// @param codes:原始码
	// @param len:	原始码数组长度
	CharCodex(const T_Code* codes, int len) {
		tree = new HuffmanTree<char>();
		int* values = new int[len];
		for (int i = 0; i < len; ++i) values[i] = 1;
		tree->buildTree(codes, values, len);
	}
	// 析构
	~CharCodex() {
		delete tree;
		this->tree = nullptr;
	}

	// 返回平均编码长度
	// @return: 返回平均编码长度
	int average_code_len() { return tree->average_code_len(); }

	// 对原始码进行编码
	// @param source:		待编码的原始码序列，char数组
	// @param source_len:	待编码的原始序列长度
	// @param buffer:		写入缓冲区
	// @param buffer_len:	写入缓冲区长度
	// @return: 返回写入了多少字节
	int encode(const T_Code* source, int source_len, char* buffer, int buffer_len) const;

	// 对编码值进行解码
	// @param source:		待解码的编码序列，char数组
	// @param source_len:	待解码的编码序列长度
	// @param buffer:		写入缓冲区
	// @param buffer_len:	写入缓冲区长度
	// @return: 返回解码了多少字节
	int decode(const char* source, int source_len, T_Code* buffer, int buffer_len) const;
};