#ifndef COE_TOOLS_MAIN_H
#define COE_TOOLS_MAIN_H

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

struct RGB {
	uint8 red;
	uint8 green;
	uint8 blue;
};

class FileBytes {
private:
	std::vector<uint8> bytes;
	std::vector<uint8>::iterator it;
public:
	FileBytes(char *filename);
	uint8 get(int i);
	uint size();

	void start();
	bool hasNext8();
	bool hasNext16();
	uint8 read8();
	uint16 read16();
	long pos();
	void seek(uint32 pos);
	void read(uint8 *ptr, uint32 size) {
		for (uint32 i = 0; i < size; i++) {
			*ptr++ = *it++;
		}
	}
};

class Sprite {
private:
	uint8 * _bytes;
	uint32 _width, _height;
public:
	Sprite(uint32 width, uint32 height) {
		_width = width;
		_height = height;
		_bytes = new uint8[_width * _height];
	}


};

void extractSpr(char *sprFilename, char *palFilename);
void printPalette(FileBytes &bytes);


#endif //COE_TOOLS_MAIN_H
