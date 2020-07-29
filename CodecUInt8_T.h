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

// ʹ��Huffman������������б���ı�����
class CharCodex {
private:
	HuffmanTree<T_Code>* tree;
private:
	// ����ձ�����
	CharCodex() = delete;
	// д��������������δʵ��
	void encode_codec(char* buffer, int len) const;
	// �����������������δʵ��
	void decode_codec(char* source_buffer, int source_len) const;
public:
	// ʹ�ø����ı���ͳ��ָ��ʹ��������
	// @param codes:ԭʼ��	
	// @param value:�����ܶ�
	// @param len:	ԭʼ�����鳤��
	CharCodex(const T_Code* codes, const double* value, int len) {
		tree = new HuffmanTree<char>();
		int* values = new int[len];
		for (int i = 0; i < len; ++i) values[i] = static_cast<int>(100 * value[i]);
		tree->buildTree(codes, values, len);
	}

	// ʹ�ø���������ԭʼ�빹��������
	// ʵ�ֵ��ǵȸ��ʱ䳤����
	// @param codes:ԭʼ��
	// @param len:	ԭʼ�����鳤��
	CharCodex(const T_Code* codes, int len) {
		tree = new HuffmanTree<char>();
		int* values = new int[len];
		for (int i = 0; i < len; ++i) values[i] = 1;
		tree->buildTree(codes, values, len);
	}
	// ����
	~CharCodex() {
		delete tree;
		this->tree = nullptr;
	}

	// ����ƽ�����볤��
	// @return: ����ƽ�����볤��
	int average_code_len() { return tree->average_code_len(); }

	// ��ԭʼ����б���
	// @param source:		�������ԭʼ�����У�char����
	// @param source_len:	�������ԭʼ���г���
	// @param buffer:		д�뻺����
	// @param buffer_len:	д�뻺��������
	// @return: ����д���˶����ֽ�
	int encode(const T_Code* source, int source_len, char* buffer, int buffer_len) const;

	// �Ա���ֵ���н���
	// @param source:		������ı������У�char����
	// @param source_len:	������ı������г���
	// @param buffer:		д�뻺����
	// @param buffer_len:	д�뻺��������
	// @return: ���ؽ����˶����ֽ�
	int decode(const char* source, int source_len, T_Code* buffer, int buffer_len) const;
};