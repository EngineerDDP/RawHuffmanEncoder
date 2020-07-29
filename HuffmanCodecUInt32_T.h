#pragma once

#include <map>
#include <queue>
#include <vector>

using std::vector;
using std::map;
using std::priority_queue;
using std::queue;

// ���������ڵ�
// ���洢��Ϣ
template <typename T>
class HuffmanNode {
private:
	int val;

public:
	// ԭʼ��
	T Code;
	// �ڵ�洢�Ĺ��������뻺��
	unsigned int huffmanCode;
	// �ڵ�洢�Ĺ��������볤��
	unsigned int huffmanCodeLen;
	// �����ӽڵ�
	HuffmanNode<T>* child[2];

public:
	// ͨ��Ȩ�س�ʼ������
	// �����ڹ�����Ҷ�ӽڵ�
	HuffmanNode(int v) : val(v) {
		Code = 0;
		child[0] = nullptr;
		child[1] = nullptr;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}

	// ʹ�ñ����Ȩ�ع�������
	// �����ڹ���Ҷ�ӽڵ�
	HuffmanNode(int v, T code) : val(v), Code(code) {
		child[0] = nullptr;
		child[1] = nullptr;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}

	// ʹ������������Ȩ�ع����ڵ�
	// �����ڹ����Ǹ���Ҷ�ӽڵ�
	HuffmanNode(int v, HuffmanNode* l, HuffmanNode* r) {
		val = v;
		Code = 0;
		child[0] = l;
		child[1] = r;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}

	// Ȩ�رȽ�����
	bool operator < (const HuffmanNode& x) const {
		return this->val < x.val;
	}

	// Ȩ�رȽ�����
	bool operator > (const HuffmanNode& x) const {
		return this->val > x.val;
	}

	// ��ȡȨ��
	int getVal() {
		return this->val;
	}
};

// ���ñȽ���
// ����ָ�����ıȽ������бȽ�
template<typename T>
class pGreater {
public:
	bool operator()(T* a, T* b) {
		return *a > * b;
	}
};

// ��ԭʼ�����������������ı�����
// getSourceCode() ��ȡԭʼ��
// getHuffmanCode() ��ȡ����������
// getHuffmanCodeLen() ��ȡ�����������bit����
// ����������洢���Ҷ���ģ���λ��0�������λ�� getHuffmanCodeLen() λ�洢����Ч�Ĺ���������ֵ
template<typename T>
class HuffmanCodec {
private:
	T sourceCode;
	unsigned huffmanCode;
	unsigned huffmanCodeLen;
public:

	HuffmanCodec() {
		sourceCode = 0;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}
	HuffmanCodec(T srcCode, unsigned hCode, unsigned hLen) {
		sourceCode = srcCode;
		huffmanCode = hCode;
		huffmanCodeLen = hLen;
	}

	// const �޶������������ж��Ƿ�洢����Ч�Ĺ���������
	// @return: ��Ч���� true����Ч���� false�����ƴ���
	bool hasCodec() const {
		return huffmanCodeLen > 0;
	}

	// �� const �޶����������ڻ�ȡԭʼ����Ϣ����������
	T& getSourceCode() {
		return sourceCode;
	}

	// �� const �޶����������ڻ�ȡ���������룬��������
	unsigned& getHuffmanCode() {
		return huffmanCode;
	}

	// �� const �޶����������ڻ�ȡ���������볤�ȣ���������
	unsigned& getHuffmanCodeLen() {
		return huffmanCodeLen;
	}
};

template <typename T>
class HuffmanTree {
private:
	// ��������������
	// ���ڹ��������������ٽ���
	HuffmanNode<T>* root;
	// Ҷ�ӽڵ�Ĺ�ϣ����
	// ���ڿ�������ָ�����룬���ٱ���
	map<T, HuffmanNode<T>*> leafs;
	// �������볤�ȣ����ڹ��Ʊ����С
	int sum_len;

	// ʹ��DFSΪÿ���ڵ㴴������������
	// @param current:	DFS��ǰ��
	// @param prefix:	DFS����ǰ׺
	// @param len:		DFS���
	void buildCode(HuffmanNode<T>* current, unsigned prefix, unsigned len) {

		// Huffman tree node always has both childs
		if (current->child[0] != nullptr) {
			buildCode(current->child[0], (prefix << 1) | 0, len + 1);
			buildCode(current->child[1], (prefix << 1) | 1, len + 1);
		}
		else {
			current->huffmanCode = prefix;
			current->huffmanCodeLen = len;
			sum_len += len;
		}
	}
	// ʹ��DFS������������֤ÿ���ڵ�Ҫô������Ҫô�ɴ���ǲ���֤����ָ����Ч��
	// @param root:	DFS ��ǰ��
	void destruct_node(HuffmanNode<T>* root) {
		if (root->child[0] == nullptr && root->child[1] == nullptr) 
			if (leafs.find(root->Code) != leafs.end())
				leafs.erase(leafs.find(root->Code));

		if (root->child[0] != nullptr) {
			destruct_node(root->child[0]);
			root->child[0] = nullptr;
		}
		if (root->child[1] != nullptr) {
			destruct_node(root->child[1]);
			root->child[1] = nullptr;
		}

		delete root;
	}
public:
	// Ĭ�Ϲ��캯��
	HuffmanTree() : sum_len(0) {
		root = new HuffmanNode<T>(-1);
	}

	// ʹ��BFS��ڵ��������
	// �����������쳣��ȫ��������һ�������жϣ��м��ڵ㽫���ɴ
	~HuffmanTree(){
		// DFS ��������֤���нڵ��жϺ�ɴ�
		destruct_node(root);
		// ����root�ڵ�
		root = nullptr;
		// ��պ����
		leafs.clear();
	}
	// �������볤��
	// @return :	�����������볤��
	int average_code_len() {
		return sum_len / leafs.size();
	}
	// ʹ�ô���ѹ�������������
	// @param codes: ��������
	// @param value: ��Ӧ�������е�Ȩ������
	// @param len:   �������鳤��
	void buildTree(const T* codes, const int* values, int len) {
		// ʹ��vector�������������
		priority_queue<HuffmanNode<T>*, vector<HuffmanNode<T>*>, pGreater<HuffmanNode<T>>> forest;
		// ��ʱ����
		// a: ���ӽڵ�
		// b: ���ӽڵ�
		// r: ������
		HuffmanNode<T>* a, * b, * r;

		// ����С����
		for (int i = 0; i < len; ++i) {
			HuffmanNode<T>* node = new HuffmanNode<T>(values[i], codes[i]);
			forest.push(node);
			// ����Ҷ�ӽڵ��ϣ����
			leafs[node->Code] = node;
		}

		// ÿ��ȡ������Сֵ����������
		// �ж� size ��ֹԽ��
		while (forest.size() > 2) {
			a = forest.top();
			forest.pop();
			b = forest.top();
			forest.pop();

			// combine two trees
			r = new HuffmanNode<T>(a->getVal() + b->getVal(), a, b);
			forest.push(r);
		}

		// ���н�ʣ������
		if (forest.size() == 2) {
			a = forest.top();
			forest.pop();
			root->child[1] = a;
		}
		// ���н�ʣһ����
		if (forest.size() == 1) {
			b = forest.top();
			forest.pop();
			root->child[0] = b;
		}

		// Ϊ����Ҷ�ӽڵ�������������
		buildCode(root, 0, 0);
	}

	// ����һ�������������Ӧ��ԭʼ��
	// @param hCode: ��Ӧ����������Ĵ洢��������Ĭ�ϴӵ�һ��bit��ʼ���ң�ֱ���ҵ�һ�������Ĺ���������
	//				 �������а����Ĺ����������������ģ� Ĭ�ϴ����λ��ʼ������
	// @return: ���� HuffmanCodec �࣬����ԭʼ���HuffmanCode����Ϣ��
	//			����ֻ���ص�һ��ƥ����������������룬�����bit�����ԣ�
	//			���� getHuffmanCodeLen() �����鿴ƥ���˶೤��huffman���롣
	HuffmanCodec<T> process(unsigned int hCode) const {
		unsigned int mask = 0x80000000;
		unsigned int codeLen = 0;

		// ʹ�ö��������ң�O(log(n))�ĸ��Ӷ�
		HuffmanNode<T>* node = root;
		// ѭ�����ң�ֱ��Ҷ�ӽڵ�
		while (node->huffmanCodeLen == 0) {
			// ����ȡ����ȡ0��1��ֵ
			unsigned int tmp = !!(hCode & mask);
			mask = mask >> 1;
			// ʹ��ָ���������ƥ�䣬�����жϹ���
			node = node->child[tmp];
		}
		return HuffmanCodec<T>(node->Code, node->huffmanCode, node->huffmanCodeLen);
	}

	// ����һ��ԭʼ��Ķ�Ӧ����������
	// @param srcCode: ԭʼ��
	// @return: ���� HuffmanCodec �࣬����ԭʼ���HuffmanCode����Ϣ��
	//			ʹ�� hasCodec() ���ü���Ƿ���ҵ�����Ч�Ĺ���������
	HuffmanCodec<T> find(T srcCode) const {
		// ʹ��std::map �еĺ�������ң�O(log(n))���Ӷ�
		auto node = leafs.find(srcCode);
		if (node != leafs.end()) {
			return HuffmanCodec<T>(node->second->Code, node->second->huffmanCode, node->second->huffmanCodeLen);
		}
		return HuffmanCodec<T>();
	}
};