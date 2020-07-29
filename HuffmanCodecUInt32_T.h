#pragma once

#include <map>
#include <queue>
#include <vector>

using std::vector;
using std::map;
using std::priority_queue;
using std::queue;

// 哈夫曼树节点
// 仅存储信息
template <typename T>
class HuffmanNode {
private:
	int val;

public:
	// 原始码
	T Code;
	// 节点存储的哈夫曼编码缓存
	unsigned int huffmanCode;
	// 节点存储的哈夫曼编码长度
	unsigned int huffmanCodeLen;
	// 左右子节点
	HuffmanNode<T>* child[2];

public:
	// 通过权重初始化对象
	// 适用于构建非叶子节点
	HuffmanNode(int v) : val(v) {
		Code = 0;
		child[0] = nullptr;
		child[1] = nullptr;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}

	// 使用编码和权重构建对象
	// 适用于构建叶子节点
	HuffmanNode(int v, T code) : val(v), Code(code) {
		child[0] = nullptr;
		child[1] = nullptr;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}

	// 使用左右子树和权重构建节点
	// 适用于构建非根非叶子节点
	HuffmanNode(int v, HuffmanNode* l, HuffmanNode* r) {
		val = v;
		Code = 0;
		child[0] = l;
		child[1] = r;
		huffmanCode = 0;
		huffmanCodeLen = 0;
	}

	// 权重比较重载
	bool operator < (const HuffmanNode& x) const {
		return this->val < x.val;
	}

	// 权重比较重载
	bool operator > (const HuffmanNode& x) const {
		return this->val > x.val;
	}

	// 获取权重
	int getVal() {
		return this->val;
	}
};

// 引用比较器
// 调用指向对象的比较器进行比较
template<typename T>
class pGreater {
public:
	bool operator()(T* a, T* b) {
		return *a > * b;
	}
};

// 从原始码编码至哈夫曼编码的编码结果
// getSourceCode() 获取原始码
// getHuffmanCode() 获取哈夫曼编码
// getHuffmanCodeLen() 获取哈夫曼编码的bit长度
// 哈夫曼编码存储是右对齐的，高位置0，从最低位至 getHuffmanCodeLen() 位存储了有效的哈夫曼编码值
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

	// const 限定方法，用于判断是否存储了有效的哈夫曼编码
	// @return: 有效返回 true，无效返回 false，复制传参
	bool hasCodec() const {
		return huffmanCodeLen > 0;
	}

	// 非 const 限定方法，用于获取原始码信息，返回引用
	T& getSourceCode() {
		return sourceCode;
	}

	// 非 const 限定方法，用于获取哈夫曼编码，返回引用
	unsigned& getHuffmanCode() {
		return huffmanCode;
	}

	// 非 const 限定方法，用于获取哈夫曼编码长度，返回引用
	unsigned& getHuffmanCodeLen() {
		return huffmanCodeLen;
	}
};

template <typename T>
class HuffmanTree {
private:
	// 哈夫曼编码树根
	// 用于构建分析树，快速解码
	HuffmanNode<T>* root;
	// 叶子节点的哈希索引
	// 用于快速索引指定编码，快速编码
	map<T, HuffmanNode<T>*> leafs;
	// 期望编码长度，用于估计编码大小
	int sum_len;

	// 使用DFS为每个节点创建哈夫曼编码
	// @param current:	DFS当前根
	// @param prefix:	DFS回溯前缀
	// @param len:		DFS深度
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
	// 使用DFS进行析构，保证每个节点要么被析构要么可达，但是不保证悬挂指针有效。
	// @param root:	DFS 当前根
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
	// 默认构造函数
	HuffmanTree() : sum_len(0) {
		root = new HuffmanNode<T>(-1);
	}

	// 使用BFS逐节点进行析构
	// 析构函数有异常安全性隐患，一旦触发中断，中间层节点将不可达。
	~HuffmanTree(){
		// DFS 析构，保证所有节点中断后可达
		destruct_node(root);
		// 重置root节点
		root = nullptr;
		// 清空红黑树
		leafs.clear();
	}
	// 期望编码长度
	// @return :	返回期望编码长度
	int average_code_len() {
		return sum_len / leafs.size();
	}
	// 使用大根堆构建哈夫曼编码
	// @param codes: 编码序列
	// @param value: 对应编码序列的权重数组
	// @param len:   编码数组长度
	void buildTree(const T* codes, const int* values, int len) {
		// 使用vector容器构建大根堆
		priority_queue<HuffmanNode<T>*, vector<HuffmanNode<T>*>, pGreater<HuffmanNode<T>>> forest;
		// 临时变量
		// a: 左子节点
		// b: 右子节点
		// r: 新树根
		HuffmanNode<T>* a, * b, * r;

		// 构建小根堆
		for (int i = 0; i < len; ++i) {
			HuffmanNode<T>* node = new HuffmanNode<T>(values[i], codes[i]);
			forest.push(node);
			// 构建叶子节点哈希索引
			leafs[node->Code] = node;
		}

		// 每次取两个最小值，构建新树
		// 判断 size 防止越界
		while (forest.size() > 2) {
			a = forest.top();
			forest.pop();
			b = forest.top();
			forest.pop();

			// combine two trees
			r = new HuffmanNode<T>(a->getVal() + b->getVal(), a, b);
			forest.push(r);
		}

		// 堆中仅剩两个树
		if (forest.size() == 2) {
			a = forest.top();
			forest.pop();
			root->child[1] = a;
		}
		// 堆中仅剩一个树
		if (forest.size() == 1) {
			b = forest.top();
			forest.pop();
			root->child[0] = b;
		}

		// 为所有叶子节点分配哈夫曼编码
		buildCode(root, 0, 0);
	}

	// 查找一个哈夫曼编码对应的原始码
	// @param hCode: 对应哈夫曼编码的存储缓冲区，默认从第一个bit开始查找，直到找到一个完整的哈夫曼编码
	//				 本参数中包含的哈夫曼编码是左对齐的， 默认从最高位开始搜索。
	// @return: 返回 HuffmanCodec 类，包含原始码和HuffmanCode码信息，
	//			查找只返回第一个匹配的完整哈夫曼编码，后面的bit被忽略，
	//			调用 getHuffmanCodeLen() 方法查看匹配了多长的huffman编码。
	HuffmanCodec<T> process(unsigned int hCode) const {
		unsigned int mask = 0x80000000;
		unsigned int codeLen = 0;

		// 使用二叉树查找，O(log(n))的复杂度
		HuffmanNode<T>* node = root;
		// 循环查找，直到叶子节点
		while (node->huffmanCodeLen == 0) {
			// 两次取反获取0或1的值
			unsigned int tmp = !!(hCode & mask);
			mask = mask >> 1;
			// 使用指针数组快速匹配，减少判断过程
			node = node->child[tmp];
		}
		return HuffmanCodec<T>(node->Code, node->huffmanCode, node->huffmanCodeLen);
	}

	// 查找一个原始码的对应哈夫曼编码
	// @param srcCode: 原始码
	// @return: 返回 HuffmanCodec 类，包含原始码和HuffmanCode码信息，
	//			使用 hasCodec() 调用检查是否查找到了有效的哈夫曼编码
	HuffmanCodec<T> find(T srcCode) const {
		// 使用std::map 中的红黑树查找，O(log(n))复杂度
		auto node = leafs.find(srcCode);
		if (node != leafs.end()) {
			return HuffmanCodec<T>(node->second->Code, node->second->huffmanCode, node->second->huffmanCodeLen);
		}
		return HuffmanCodec<T>();
	}
};