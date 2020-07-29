#include "CodecUInt8_T.h"

using std::overflow_error;

int CharCodex::encode(const char* source, int source_len, char* buffer, int buffer_len) const {
	int bytes_write = 0;
	int bits_write = 0;

	if (source_len == 0)
		return 0;

	for (int i = 0; i < source_len; ++i) {
		// ƥ�� ԭʼ��
		auto key = tree->find(source[i]);
		// ƥ�䲻���ͱ���
		if (!key.hasCodec()) {
			throw UndefinedSourceCode(source[i]);
		}
		// ��ȡԭʼ���ԭʼ�볤�ȣ�ʹ�����ã�ѭ���������Զ���������ջ�洢����ƫ����Ѱַ
		unsigned int tmp = key.getHuffmanCode();
		unsigned int tmp_len = key.getHuffmanCodeLen();

		while (tmp_len != 0) {
			// �жϵ�ǰ�ֽ���δд������Ƿ��ܹ����µ�ǰ�Ĺ���������
			if (tmp_len >= static_cast<unsigned>(8 - bits_write)) {
				// ����治�£���洢���µĲ��֣�������һ��byte
				buffer[bytes_write] |= (tmp >> (tmp_len - 8 + bits_write));
				tmp_len -= 8 - bits_write;
				bits_write = 0;
				bytes_write++;
			}
			else {
				// ������Դ洢����洢������ ʣ�೤�� tmp_len 
				buffer[bytes_write] |= (tmp << (8 - bits_write - tmp_len));
				bits_write = bits_write + tmp_len;
				tmp_len = 0;
			}
			if (bytes_write >= buffer_len) {
				throw overflow_error("buffer not enough!");
			}
		}
	}
	return bytes_write + !!bits_write;
}

int CharCodex::decode(const char* source, int source_len, char* buffer, int buffer_len) const
{
	int bytes_write = 0;
	int bytes_read = 0;

	unsigned int hCodeBuffer = 0;
	int hCodeBuffered = 0;

	if (source_len == 0)
		return 0;

	do {

		// ���ǡ�ÿ���д��һ�����ϵ�byte
		while (hCodeBuffered <= 24 && bytes_read != source_len) {
			// ƫ�ƣ�д��
			hCodeBuffer |= static_cast<unsigned char>(source[bytes_read]) << (24 - hCodeBuffered);
			hCodeBuffered += 8;
			bytes_read++;
		}

		// ����ԭʼ��
		auto res = tree->process(hCodeBuffer);
		// ����ƫ����
		hCodeBuffer = hCodeBuffer << res.getHuffmanCodeLen();
		hCodeBuffered -= res.getHuffmanCodeLen();
		// д��ԭʼ��
		buffer[bytes_write] = res.getSourceCode();
		bytes_write++;
		if (bytes_write == buffer_len) {
			throw overflow_error("buffer not enough!");

		}
	} while (hCodeBuffered > 0);

	return bytes_write;
}