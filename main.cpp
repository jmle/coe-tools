#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <vector>
#include "lib/bitmap/bitmap_image.hpp"
#include "main.h"


RGB getColor(uint8 byte, FileBytes *palette) {
    int pi = (byte * 3) & 0x3FF;
    uint8 r = palette->get(pi)     << 2;
    uint8 g = palette->get(pi + 1) << 2;
    uint8 b = palette->get(pi + 2) << 2;

    return RGB{r, g, b};
}

void extractDat(char *datFilename, char *palFilename) {
    auto datBytes = new FileBytes(datFilename);
    auto palBytes = new FileBytes(palFilename);

    bitmap_image image(320, 200);

    for (int y = 0; y < 200; ++y) {
        for (int x = 0; x < 320; ++x) {
            RGB color = getColor(datBytes->get(320 * y + x), palBytes);
            image.set_pixel(x, y, color);
        }
    }

    image.save_image(strcat(datFilename, ".bmp"));
}

int main(int argc, char *argv[]) {
    char *command = argv[1];
    if (strcmp(command, "dat") == 0) {
        extractDat(argv[2], argv[3]);
    } else if (strcmp(command, "spr") == 0) {
        extractSpr(argv[2], argv[3]);
    } else if (strcmp(command, "map") == 0) {

    } else if (strcmp(command, "pal") == 0) {
        auto bytes = new FileBytes(argv[2]);
        printPalette(*bytes);
    }
    return 0;
}

void unpackSprite(uint8 *data, uint8 *dest) {
	uint16 width = *(data + 0);
	uint16 height = *(data + 2);

	data += 10;

	for (uint p = 0; p < 4; p++) {
		uint y = 0, yc = height;
		uint8 *row = dest + p;
		while (yc) {
			uint8 cmd = *data++;
			if (cmd == 0) {
				--yc;
				y++;
				row = dest + y * width + p;
			} else if (cmd < 0) {
				row += -cmd * 4;
			} else {
				while (cmd--) {
					*row = *data++;
					row += 4;
				}
			}
		}
	}
}

void extractSpr(char *sprFilename, char *palFilename) {
    auto sprBytes = new FileBytes(sprFilename);
    auto palBytes = new FileBytes(palFilename);

    sprBytes->start();
    uint16 firstOffs = sprBytes->read16();
    auto *offsets = new uint32[(firstOffs * 8) + 1];
    uint offsetsCount = 1;
    offsets[0] = firstOffs * 16;
    while (sprBytes->pos() != offsets[0]) {
		offsets[offsetsCount] = sprBytes->read16() * 16;
		if (!offsets[offsetsCount] || offsets[offsetsCount] >= sprBytes->size())
			break;
		offsetsCount++;
	}
	offsets[offsetsCount] = sprBytes->size();

	for (uint i = 0; i < offsetsCount; i++) {
		uint32 size = offsets[i + 1] - offsets[i];
		auto *data = new uint8[size];
		sprBytes->seek(offsets[i]);
		sprBytes->read(data, size);

		uint16 width = *(data + 0);
		uint16 height = *(data + 2);
		auto *dest = new uint8[width * height]();
		unpackSprite(data, dest);

		bitmap_image image(width, height);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				RGB color = getColor(*(dest + (width * y + x)), palBytes);
				image.set_pixel(x, y, color);
			}
		}
		image.save_image(strcat(sprFilename, ".bmp"));

		delete[] data;
		delete[] dest;
	}

	delete[] offsets;

}

void printPalette(FileBytes &bytes) {
    for (int i = 0; i < bytes.size() / 3; i++) {
        int pi = i * 3;
        uint8 r = bytes.get(pi);
        uint8 g = bytes.get(pi + 1);
        uint8 b = bytes.get(pi + 2);
        char color[100];
        snprintf(color, sizeof color, "\033[0m%x\033[48;2;%d;%d;%dm\n", i, r, g, b);
        std::cout << color << std::endl;
    }
}

FileBytes::FileBytes(char *filename) {
	std::basic_ifstream<char> input(filename, std::ios::binary);
	std::vector<uint8> buffer(std::istreambuf_iterator<char>(input), {});
	bytes = buffer;
	it = bytes.end();
}

uint8 FileBytes::get(int i) {
	return bytes.at(i);
}

uint FileBytes::size() {
	return bytes.size();
}

uint8 FileBytes::read8() {
	return *it++;
}

uint16 FileBytes::read16() {
	return *it++ | (*it++ << 8);
}

void FileBytes::start() {
	it = bytes.begin();
}

bool FileBytes::hasNext8() {
	return it + 1 != bytes.end();
}

bool FileBytes::hasNext16() {
	return it + 1 != bytes.end() && it + 2 != bytes.end();
}

long FileBytes::pos() {
	return it - bytes.begin();
}

void FileBytes::seek(uint32 pos) {
	start();
	it = it + pos;
}
