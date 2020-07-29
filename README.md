# RawHuffmanEncoder
极简哈夫曼编码模块，仅包含变长编码部分，不包含原始码长度、编码字典等任何多余信息。

## 使用方法

可以直接无权重设置编码表

```
import huffman as hf

codec = hf.codec()
keys = [-1,0,1]
codec.set_codec(keys)

a = [-1,0,-1,0,0,0,1,1,1,-1]

codes = codec.encode(a)
b = codec.decode(codes)
```

也可以设置带权编码表

```
import huffman as hf

codec = hf.codec()
keys = [-1,0,1]
vals = [0.1, 0.8, 0.1]
codec.set_codec_with_value(keys, vals)

a = [-1,0,-1,0,0,0,1,1,1,-1]

codes = codec.encode(a)
b = codec.decode(codes)
```

## 核心代码

### 编码器

CodecUInt8_T.h

C++顶层编码器类，接受带权码构造和等权码构造，有encode和decode两个成员方法。只适用于对T_Code（char）类型的数据进行编码。
```
class CharCodex{
	// 使用给定的编码和出现概率构造编码器
	// @param codes:原始码	
	// @param value:概率密度
	// @param len:	原始码数组长度
	CharCodex(const T_Code* codes, const double* value, int len);
  
 	// 使用给定数量的原始码构建编码器
	// 实现的是等概率变长编码
	// @param codes:原始码
	// @param len:	原始码数组长度
	CharCodex(const T_Code* codes, int len);
  
	// 返回平均编码长度
	// @return: 返回平均编码长度
	int average_code_len();
  
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
}
```
### 哈夫曼编码泛型

HuffmanCodecUInt32_T.h

哈夫曼编码树泛型，最长可以构造32bit的变长编码表，被编码数据要求能存储于栈中且为POD格式。被编码数据的类型不限，需要有可用的小于运算符。

```
template<typename T>
class HuffmanTree{
	// 使用大根堆构建哈夫曼编码
	// @param codes: 编码序列
	// @param value: 对应编码序列的权重数组
	// @param len:   编码数组长度
	void buildTree(const T* codes, const int* values, int len);
  
 	// 查找一个哈夫曼编码对应的原始码
	// @param hCode: 对应哈夫曼编码的存储缓冲区，默认从第一个bit开始查找，直到找到一个完整的哈夫曼编码
	//				 本参数中包含的哈夫曼编码是左对齐的， 默认从最高位开始搜索。
	// @return: 返回 HuffmanCodec 类，包含原始码和HuffmanCode码信息，
	//			查找只返回第一个匹配的完整哈夫曼编码，后面的bit被忽略，
	//			调用 getHuffmanCodeLen() 方法查看匹配了多长的huffman编码。
	HuffmanCodec<T> process(unsigned int hCode) const;
  
	// 查找一个原始码的对应哈夫曼编码
	// @param srcCode: 原始码
	// @return: 返回 HuffmanCodec 类，包含原始码和HuffmanCode码信息，
	//			使用 hasCodec() 调用检查是否查找到了有效的哈夫曼编码
	HuffmanCodec<T> find(T srcCode) const;

};

// 从原始码编码至哈夫曼编码的编码结果
// getSourceCode() 获取原始码
// getHuffmanCode() 获取哈夫曼编码
// getHuffmanCodeLen() 获取哈夫曼编码的bit长度
// 哈夫曼编码存储是右对齐的，高位置0，从最低位至 getHuffmanCodeLen() 位存储了有效的哈夫曼编码值
template<typename T>
class HuffmanCodec;
```
