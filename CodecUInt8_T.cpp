#include "CodecUInt8_T.h"

using std::overflow_error;

int CharCodex::encode(const char* source, int source_len, char* buffer, int buffer_len) const {
	int bytes_write = 0;
	int bits_write = 0;

	if (source_len == 0)
		return 0;

	for (int i = 0; i < source_len; ++i) {
		// 匹配 原始码
		auto key = tree->find(source[i]);
		// 匹配不到就报错
		if (!key.hasCodec()) {
			throw UndefinedSourceCode(source[i]);
		}
		// 获取原始码和原始码长度，使用引用，循环结束就自动析构，用栈存储，用偏移量寻址
		unsigned int tmp = key.getHuffmanCode();
		unsigned int tmp_len = key.getHuffmanCodeLen();

		while (tmp_len != 0) {
			// 判断当前字节尚未写入的量是否能够存下当前的哈夫曼编码
			if (tmp_len >= static_cast<unsigned>(8 - bits_write)) {
				// 如果存不下，则存储存下的部分，续存下一个byte
				buffer[bytes_write] |= (tmp >> (tmp_len - 8 + bits_write));
				tmp_len -= 8 - bits_write;
				bits_write = 0;
				bytes_write++;
			}
			else {
				// 如果可以存储，则存储并置零 剩余长度 tmp_len 
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

		// 如果恰好可以写入一个以上的byte
		while (hCodeBuffered <= 24 && bytes_read != source_len) {
			// 偏移，写入
			hCodeBuffer |= static_cast<unsigned char>(source[bytes_read]) << (24 - hCodeBuffered);
			hCodeBuffered += 8;
			bytes_read++;
		}

		// 查找原始码
		auto res = tree->process(hCodeBuffer);
		// 修正偏移量
		hCodeBuffer = hCodeBuffer << res.getHuffmanCodeLen();
		hCodeBuffered -= res.getHuffmanCodeLen();
		// 写回原始码
		buffer[bytes_write] = res.getSourceCode();
		bytes_write++;
		if (bytes_write == buffer_len) {
			throw overflow_error("buffer not enough!");

		}
	} while (hCodeBuffered > 0);

	return bytes_write;
}