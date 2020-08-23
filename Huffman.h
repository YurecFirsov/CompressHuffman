#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>


typedef unsigned char byte;


struct IInputStream {
	virtual bool Read(byte& value) = 0;
};

struct IOutputStream {
	virtual void Write(byte value) = 0;
};


void Encode(IInputStream& original, IOutputStream& compressed);
void Decode(IInputStream& compressed, IOutputStream& original);

