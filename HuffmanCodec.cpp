// ���嵼���� Python �ṹ
//

#include "pch.h"

// ----------------------------------------
//               �����ʼ��
// ----------------------------------------

static const char* cbcodec_string_desc = "Huffman codec class.";

static PyObject* CBCodec_Init(CBCodec* self, PyObject* pArgs, PyObject* kwds) {
	if (self != nullptr) {
		self->codec_instance = nullptr;
		self->input_buffer = nullptr;
		self->output_buffer = nullptr;
		self->input_buffer_len = 0;
		self->output_buffer_len = 0;
		return reinterpret_cast<PyObject*>(self);
	}
	else {
		PyErr_SetString(PyExc_TypeError, "Memory allocation failed.");
		return NULL;
	}
}

static void CBCodec_Destruct(CBCodec* self) {
	if (self->codec_instance) {
		delete self->codec_instance;
		self->codec_instance = nullptr;
		delete self->input_buffer;
		self->input_buffer = nullptr;
		delete self->output_buffer;
		self->output_buffer = nullptr;
	}
	Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

static PyObject* CBCodec_Str(CBCodec* self) {
	return Py_BuildValue("s", cbcodec_string_desc);
}

static PyObject* CBCodec_Repr(CBCodec* self) {
	return CBCodec_Str(self);
}

// ----------------------------------------
//               �����Ա����
// ----------------------------------------

static PyObject* CBCodec_SetCodecWithValue(CBCodec* self, PyObject* pArgs) {
	PyObject* res = Py_None;
	Py_INCREF(Py_None);

	if (self->codec_instance != nullptr) {
		delete self->codec_instance;
		self->codec_instance = nullptr;
	}
	PyObject* list_of_code;
	PyObject* list_of_value;
	PyArg_ParseTuple(pArgs, "OO", &list_of_code, &list_of_value);
	if (!PySequence_Check(list_of_code) || !PySequence_Check(list_of_value)) {
		PyErr_SetString(PyExc_TypeError, "List of codes were required.");
		return res;
	}
	list_of_code = PySequence_Fast(list_of_code, "Iterable object is required.");
	list_of_value = PySequence_Fast(list_of_value, "Iterable object is required.");
	PyObject* item_code;
	PyObject* item_value;

	int len_code = PySequence_Fast_GET_SIZE(list_of_code);
	
	int* tmp_int = new int;
	char* char_code = new char[len_code];
	double* float_val = new double[len_code];

	for (int i = 0; i < len_code; ++i) {
		item_code = PySequence_Fast_GET_ITEM(list_of_code, i);
		item_value = PySequence_Fast_GET_ITEM(list_of_value, i);

		if (!PyArg_Parse(item_code, "i", tmp_int) || !PyArg_Parse(item_value, "d", float_val + i)) {
			PyErr_SetString(PyExc_TypeError, "Code must be int -> double.");
			return res;
		}

		// ǿת�� byte
		char_code[i] = static_cast<T_Code>(*tmp_int);
	}

	self->codec_instance = new CppByteCodec(char_code, float_val, len_code);

	delete[] char_code;
	delete[] float_val;
	delete tmp_int;
	Py_DECREF(list_of_code);
	Py_DECREF(list_of_value);

	return res;
}

static PyObject* CBCodec_SetCodec(CBCodec* self, PyObject* pArgs) {
	PyObject* res = Py_None;
	Py_INCREF(Py_None);

	if (self->codec_instance != nullptr) {
		delete self->codec_instance;
		self->codec_instance = nullptr;
	}

	PyObject* list_of_code;
	PyArg_ParseTuple(pArgs, "O", &list_of_code);
	if (!PySequence_Check(list_of_code)) {
		PyErr_SetString(PyExc_TypeError, "List of codes were required.");
		return res;
	}
	list_of_code = PySequence_Fast(list_of_code, "Iterable object is required.");
	PyObject* item_code;

	int len_code = PySequence_Fast_GET_SIZE(list_of_code);

	int* tmp_int = new int;
	char* char_code = new char[len_code];

	for (int i = 0; i < len_code; ++i) {
		item_code = PySequence_Fast_GET_ITEM(list_of_code, i);

		if (!PyArg_Parse(item_code, "i", tmp_int)) {
			PyErr_SetString(PyExc_TypeError, "Code must be int.");
			return res;
		}

		// ǿת�� ֧�ֵĸ�ʽ
		char_code[i] = static_cast<T_Code>(*tmp_int);
	}

	self->codec_instance = new CppByteCodec(char_code, len_code);

	delete[] char_code;
	delete tmp_int;
	Py_DECREF(list_of_code);

	return res;
}

static PyObject* CBCodec_Encode(CBCodec* self, PyObject* pArgs) {
	PyObject* res = Py_None;

	// �����ı�����
	if (self->codec_instance == nullptr) {
		PyErr_SetString(PyExc_NotImplementedError, "Codec is not initialized yet.");
		Py_INCREF(Py_None);
		return res;
	}

	// ��������ʽ
	PyObject* list_of_source_code;
	PyArg_ParseTuple(pArgs, "O", &list_of_source_code);
	// ����������
	if (!PySequence_Check(list_of_source_code)) {
		PyErr_SetString(PyExc_TypeError, "List of codes were required.");
		Py_INCREF(Py_None);
		return res;
	}

	// ת��FAST Sequence
	PyObject* raw_source_code = PySequence_Fast(list_of_source_code, "List of codes were required.");
	// Ԫ��ָ��
	PyObject* tmp_item;

	// ��ȡ����
	int len_source = PySequence_Fast_GET_SIZE(raw_source_code);
	// Ԥ�����볤��
	int len_target = len_source * self->codec_instance->average_code_len();

	// ����ת���ռ�
	volatile int tmp_source;

	/*   ------------------ �ڴ���� ------------------    */

	if (self->input_buffer_len < len_source) {
		self->input_buffer_len = len_source << 1;
		delete self->input_buffer;
		self->input_buffer = nullptr;
	}
	if (self->input_buffer == nullptr) {
		self->input_buffer = new char[self->input_buffer_len]{ 0 };
	}
	else {
		for (int i = 0; i < self->input_buffer_len; ++i)
			self->input_buffer[i] = 0;
	}
	if (self->output_buffer_len < len_target) {
		self->output_buffer_len = len_target << 1;
		delete self->output_buffer;
		self->output_buffer = nullptr;
	}
	if (self->output_buffer == nullptr) {
		self->output_buffer = new char[self->output_buffer_len]{ 0 };
	}
	else {
		for (int i = 0; i < self->output_buffer_len; ++i)
			self->output_buffer[i] = 0;
	}

	// ����ԭʼ������
	char* source_code_buffer = self->input_buffer;
	// ������뻺����
	char* byte_buffer = self->output_buffer;

	/*   ------------------ �ڴ���� ------------------    */

	// ȡ���йܶ���
	for (int i = 0; i < len_source; ++i) {
		// ����Ԫ��
		tmp_item = PySequence_Fast_GET_ITEM(raw_source_code, i);
		// ����ʽ
		if (!PyArg_Parse(tmp_item, "i", &tmp_source)) {
			PyErr_SetString(PyExc_TypeError, "Code type int were required.");
			Py_INCREF(Py_None);
			return res;
		}
		// ��̬����ת��
		source_code_buffer[i] = static_cast<T_Code>(tmp_source);
	}

	// ����
	int encode_len = 0;
	try {
		// ���ú��Ľ����� 
		encode_len = self->codec_instance->encode(source_code_buffer, len_source, byte_buffer, len_target);

	}
	catch (std::overflow_error e) {
		PyErr_SetString(PyExc_IOError, "Encoding result larger than expected.");
		Py_INCREF(Py_None);
		return res;
	}
	catch (UndefinedSourceCode ex) {
		PyErr_SetString(PyExc_IOError, "Unidentified source code.");
		Py_INCREF(Py_None);
		return res;
	}

	// �����й��ֽڴ�
	res = Py_BuildValue("y#", byte_buffer, encode_len);

	/*   ------------------ �ڴ��ͷ� ------------------    */

	//delete[] byte_buffer;
	//delete[] source_code_buffer;
	Py_DECREF(raw_source_code);

	/*   ------------------ �ڴ��ͷ� ------------------    */

	return res;
}

static PyObject* CBCodec_Decode(CBCodec* self, PyObject* pArgs) {
	PyObject* res = Py_None;

	// ��������
	if (self->codec_instance == nullptr) {
		PyErr_SetString(PyExc_NotImplementedError, "Codec is not initialized yet.");
		Py_INCREF(Py_None);
		return res;
	}

	// ������
	PyObject* list_of_source_code;
	PyArg_ParseTuple(pArgs, "O", &list_of_source_code);
	// ����������
	if (!PySequence_Check(list_of_source_code)) {
		PyErr_SetString(PyExc_TypeError, "List of codes were required.");
		Py_INCREF(Py_None);
		return res;
	}

	/*   ------------------ ���ü��� ------------------    */

	// ת��FAST Item
	PyObject* raw_source_code = PySequence_Fast(list_of_source_code, "List of codes were required.");

	/*   ------------------ ���ü��� ------------------    */

	// Ԫ��ָ��
	PyObject* tmp_item;

	// ��ȡ����
	int len_source = PySequence_Fast_GET_SIZE(raw_source_code);
	// Ԥ�����볤��
	int len_target = len_source * 9 / self->codec_instance->average_code_len();

	// ����ת���ռ�
	volatile int tmp_source = 0;

	/*   ------------------ �ڴ���� ------------------    */

	if (self->input_buffer_len < len_source) {
		self->input_buffer_len = len_source << 1;
		delete self->input_buffer;
		self->input_buffer = nullptr;
	}
	if (self->input_buffer == nullptr) {
		self->input_buffer = new char[self->input_buffer_len]{ 0 };
	}
	else {
		for (int i = 0; i < self->input_buffer_len; ++i)
			self->input_buffer[i] = 0;
	}
	if (self->output_buffer_len < len_target) {
		self->output_buffer_len = len_target << 1;
		delete self->output_buffer;
		self->output_buffer = nullptr;
	}
	if (self->output_buffer == nullptr) {
		self->output_buffer = new char[self->output_buffer_len]{ 0 };
	}
	else {
		for (int i = 0; i < self->output_buffer_len; ++i)
			self->output_buffer[i] = 0;
	}

	// ����ԭʼ������
	char* source_code_buffer = self->input_buffer;
	// ������뻺����
	char* byte_buffer = self->output_buffer;

	/*   ------------------ �ڴ���� ------------------    */

	// �����ڴ�������
	if (source_code_buffer == nullptr || byte_buffer == nullptr) {
		PyErr_SetString(PyExc_MemoryError, "Memory allocation failed.");
		Py_INCREF(Py_None);
		return res;
	}

	// ��ȡԪ�أ�ִ��ת��
	for (int i = 0; i < len_source; ++i) {
		tmp_item = PySequence_Fast_GET_ITEM(raw_source_code, i);
		if (!PyArg_Parse(tmp_item, "i", &tmp_source)) {
			PyErr_SetString(PyExc_TypeError, "Code type int were required.");
			Py_INCREF(Py_None);
			return res;
		}

		// ��̬����ת��
		source_code_buffer[i] = static_cast<char>(tmp_source);
	}

	// ����
	int decode_len = 0;
	try {
		// ���ú��Ľ�����
		decode_len = self->codec_instance->decode(source_code_buffer, len_source, byte_buffer, len_target);
	}
	catch (std::overflow_error e) {
		PyErr_SetString(PyExc_IOError, "Decoding result larger than expected.");
		Py_INCREF(Py_None);
		return res;
	}


	// ����GC����
	res = PyList_New(decode_len);
	// д���йܿռ�
	for (int i = 0; i < decode_len; ++i) {
		// �����йܶ���
		tmp_source = byte_buffer[i];
		tmp_item = Py_BuildValue("i", tmp_source);
		// д��
		PyList_SetItem(res, i, tmp_item);
	}

	/*   ------------------ �ڴ��ͷ� ------------------    */

	// �ͷŶ��ڴ�
	//delete[] byte_buffer;
	//delete[] source_code_buffer;
	Py_DECREF(raw_source_code);

	/*   ------------------ �ڴ��ͷ� ------------------    */

	return res;
}

// ----------------------------------------
//               ע��ṹ
// ----------------------------------------

static PyMemberDef CBCodec_DataMembers[] = {
	{const_cast<char*>("huffman codec instance"), T_NONE, offsetof(CBCodec, codec_instance), 0, const_cast<char*>("Huffman codec for bytes encoding.")},
	{NULL, NULL, NULL, 0, NULL}
};

// ----------------------------------------
//               ע�᷽��
// ----------------------------------------

static PyMethodDef CBCodec_MethodMembers[] = {
	{"set_codec", reinterpret_cast<PyCFunction>(CBCodec_SetCodec), METH_VARARGS, "set codec with static byte samples."},
	{"set_codec_with_value", reinterpret_cast<PyCFunction>(CBCodec_SetCodecWithValue), METH_VARARGS, "set codec with bytes and its value."},
	{"encode", reinterpret_cast<PyCFunction>(CBCodec_Encode), METH_VARARGS, "encode byte sequence."},
	{"decode", reinterpret_cast<PyCFunction>(CBCodec_Decode), METH_VARARGS, "decode byte sequence."},
	{NULL, NULL, NULL, NULL}
};

// ----------------------------------------
//               ע��ģ��
// ----------------------------------------

static PyMethodDef ModuleMethods[] = {
	{NULL, NULL, 0, NULL}
};

static PyModuleDef CBCodec_ModuleDef = 
{
	PyModuleDef_HEAD_INIT,
	"huffman_encoder",
	"Huffman bytes encoder.",
	-1,
	ModuleMethods
};

static PyTypeObject CBCodec_ClassInfo;

// ----------------------------------------
//               �����ʼ������
// ----------------------------------------

PyMODINIT_FUNC PyInit_huffman(void) {
	PyObject* res = Py_None;

	CBCodec_ClassInfo.tp_new = PyType_GenericNew;
	CBCodec_ClassInfo.tp_name = "huffman_encoder.cbcodec";
	CBCodec_ClassInfo.tp_basicsize = sizeof(CBCodec);
	CBCodec_ClassInfo.tp_itemsize = 0;
	CBCodec_ClassInfo.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
	CBCodec_ClassInfo.tp_doc = "huffman codec, dictionary size is 1 byte, man encode len is 4 bytes.";
	CBCodec_ClassInfo.tp_str = (reprfunc)CBCodec_Str;
	CBCodec_ClassInfo.tp_dealloc = (destructor)CBCodec_Destruct;
	CBCodec_ClassInfo.tp_repr = (reprfunc)CBCodec_Repr;
	CBCodec_ClassInfo.tp_members = CBCodec_DataMembers;
	CBCodec_ClassInfo.tp_methods = CBCodec_MethodMembers;


	if (PyType_Ready(&CBCodec_ClassInfo) < 0) {
		PyErr_SetString(PyExc_ImportError, "class import error");
		return NULL;
	}
	res = PyModule_Create(&CBCodec_ModuleDef);

	if (res == 0) {
		PyErr_SetString(PyExc_ImportError, "Module create failed.");
		return NULL;
	}

	Py_INCREF(&CBCodec_ClassInfo);

	PyModule_AddObject(res, "codec", reinterpret_cast<PyObject*>(&CBCodec_ClassInfo));

	return res;
}