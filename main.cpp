#include <iostream>
#include "Huffman.h"
#include <fstream>
#include <sstream>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Основное приложение
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Входной поток
class CInputStream : public IInputStream {
public:
    CInputStream(const std::vector<byte>& _body) : index(0), body(_body) {}

    virtual bool Read(byte& value);

private:
    unsigned int index;
    const std::vector<unsigned char>& body;
};

bool CInputStream::Read(byte& value)
{
    if (index >= body.size()) {
        return false;
    }

    value = body[index++];
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Выходной поток
class COutputStream : public IOutputStream {
public:
    COutputStream(std::vector<byte>& _body) : body(_body) { body.clear(); }
    virtual void Write(byte value) { body.push_back(value); }

private:
    std::vector<byte>& body;
};

// Считываем все файлы, что перечислены во входе
void fillInputs(std::vector< std::vector<byte> >& inputs)
{
    inputs.clear();

    std::string currentFileName;
    while (getline( std::cin, currentFileName )) {
        if (currentFileName.empty()) {
            continue;
        }

        // Есть еще один файл, который следует закодировать
        inputs.push_back(std::vector<byte>());

        std::ifstream file;
        file.open(currentFileName.c_str());

        byte value;
        while (file >> value) {
            (*inputs.rbegin()).push_back(value);
        }
        //only one file
        break;
    }
}

bool isEqual(const std::vector<byte>& v1, const std::vector<byte>& v2)
{
    if (v1.size() != v2.size()) {
        return false;
    }

    for( unsigned int i = 0; i < v1.size(); i++ ) {
        if( v1[i] != v2[i] ) {
            return false;
        }
    }

    return true;
}

int calculateSize(const std::vector< std::vector<byte> >& array)
{
    int result = 0;
    for ( unsigned int i  = 0; i < array.size(); i++ ) {
        const std::vector<byte>& data = array[i];
        result += data.size();
    }

    return result;
}


int main()
{
    // Получаем данные, которые нужно закодировать
    std::vector< std::vector<byte> > input;

    fillInputs(input);

    // Сжимаем данные
    std::vector< std::vector<byte> > compressed;
    compressed.resize(input.size());
    for (unsigned int i = 0; i < input.size(); i++) {
        CInputStream iStream(input[i]);
        COutputStream oStream(compressed[i]);
        Encode(iStream, oStream);
    }

    // Распаковываем сжатые данные и проверяем, что они совпадают с оригиналом
    for (unsigned int i = 0; i < input.size(); i++) {
        std::vector<byte> output;
        CInputStream iStream(compressed[i]);
        COutputStream oStream(output);
        Decode(iStream, oStream);

        std::ofstream outFile("/home/Yurec/repos/testing/work/compress", std::ios::out | std::ios::binary);
        for (const auto &e : output) outFile << e;
        std::ofstream outFile_origin("/home/Yurec/repos/testing/work/origin", std::ios::out | std::ios::binary);
        for (const auto &e : input[i]) outFile_origin << e;
        if (!isEqual(input[i], output)) {
            std::cout << -1;
            return 0;
        }
    }

    // Вычисляем степень сжатия
    std::cout << (100. * calculateSize(compressed) / calculateSize(input));

    return 0;
}
