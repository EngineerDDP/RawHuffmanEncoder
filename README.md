# RawHuffmanEncoder
�������������ģ�飬�������䳤���벿�֣�������ԭʼ�볤�ȡ������ֵ���κζ�����Ϣ��

## ʹ�÷���

����ֱ����Ȩ�����ñ����

```
import huffman as hf

codec = hf.codec()
keys = [-1,0,1]
codec.set_codec(keys)

a = [-1,0,-1,0,0,0,1,1,1,-1]

codes = codec.encode(a)
b = codec.decode(codes)
```

Ҳ�������ô�Ȩ�����

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

## ���Ĵ���

### ������

CodecUInt8_T.h

C++����������࣬���ܴ�Ȩ�빹��͵�Ȩ�빹�죬��encode��decode������Ա������ֻ�����ڶ�T_Code��char�����͵����ݽ��б��롣
```
class CharCodex{
	// ʹ�ø����ı���ͳ��ָ��ʹ��������
	// @param codes:ԭʼ��	
	// @param value:�����ܶ�
	// @param len:	ԭʼ�����鳤��
	CharCodex(const T_Code* codes, const double* value, int len);
  
 	// ʹ�ø���������ԭʼ�빹��������
	// ʵ�ֵ��ǵȸ��ʱ䳤����
	// @param codes:ԭʼ��
	// @param len:	ԭʼ�����鳤��
	CharCodex(const T_Code* codes, int len);
  
	// ����ƽ�����볤��
	// @return: ����ƽ�����볤��
	int average_code_len();
  
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
}
```
### ���������뷺��

HuffmanCodecUInt32_T.h

���������������ͣ�����Թ���32bit�ı䳤���������������Ҫ���ܴ洢��ջ����ΪPOD��ʽ�����������ݵ����Ͳ��ޣ���Ҫ�п��õ�С���������

```
template<typename T>
class HuffmanTree{
	// ʹ�ô���ѹ�������������
	// @param codes: ��������
	// @param value: ��Ӧ�������е�Ȩ������
	// @param len:   �������鳤��
	void buildTree(const T* codes, const int* values, int len);
  
 	// ����һ�������������Ӧ��ԭʼ��
	// @param hCode: ��Ӧ����������Ĵ洢��������Ĭ�ϴӵ�һ��bit��ʼ���ң�ֱ���ҵ�һ�������Ĺ���������
	//				 �������а����Ĺ����������������ģ� Ĭ�ϴ����λ��ʼ������
	// @return: ���� HuffmanCodec �࣬����ԭʼ���HuffmanCode����Ϣ��
	//			����ֻ���ص�һ��ƥ����������������룬�����bit�����ԣ�
	//			���� getHuffmanCodeLen() �����鿴ƥ���˶೤��huffman���롣
	HuffmanCodec<T> process(unsigned int hCode) const;
  
	// ����һ��ԭʼ��Ķ�Ӧ����������
	// @param srcCode: ԭʼ��
	// @return: ���� HuffmanCodec �࣬����ԭʼ���HuffmanCode����Ϣ��
	//			ʹ�� hasCodec() ���ü���Ƿ���ҵ�����Ч�Ĺ���������
	HuffmanCodec<T> find(T srcCode) const;

};

// ��ԭʼ�����������������ı�����
// getSourceCode() ��ȡԭʼ��
// getHuffmanCode() ��ȡ����������
// getHuffmanCodeLen() ��ȡ�����������bit����
// ����������洢���Ҷ���ģ���λ��0�������λ�� getHuffmanCodeLen() λ�洢����Ч�Ĺ���������ֵ
template<typename T>
class HuffmanCodec;
```
