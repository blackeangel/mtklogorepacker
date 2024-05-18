#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "../zlib/zlib.h"
#include "../libpng/png.h"

struct MTK_logo {
    struct {
        uint8_t start[4];               // offset 0x0 start file                            |
        uint32_t total_blocks[1];       // offset 0x04 total_blocks too?                    |
        char magic[4];                  //offset 0x08 'logo'                                |=>header 512 bytes
        uint8_t hvost[500];             //ofsset 0x13..0x199 any data in header            _|
    } header;
    uint32_t count_pictures[1];                  //offset 0x200 number of pictures (little-endian value)
    uint32_t total_blocks[1];           //offset 0x205 total data size excluding header (512 bytes) (little-endian value)
};

constexpr int SIZE_MTK_HEADER = 512; // Размер MTK заголовка
constexpr int SIZE_INT = 4; // Размер целочисленного значения
constexpr int PAD_TO = 32; // Значение для выравнивания

// Структура для хранения информации о разрешении экрана
struct Resolution {
    int height;
    int width;
};

// Распространенные разрешения экрана
static std::vector<Resolution> common_resolutions = {
   // Hight,    Wight
        {1,     102	},
        {1,     135},
        {2,     79},
        {2,     152},
        {24,    135},
        {24,    152},
        {32,    23},
        {32,    34},
        {40,    28},
        {40,    32},
        {54,    38},
        {54,    48},
        {160,   120},
        {240,   160},
        {320,   240},
        {360,   240},
        {376,   240},
        {384,   240},
        {400,   240},
        {428,   240},
        {432,   240},
        {480,   270},
        {480,   272},
        {480,   320},
        {480,   360},
        {640,   360},
        {640,   384},
        {640,   480},
        {720,   480},
        {768,   480},
        {800,   480},
        {848,   480},
        {852,   480},
        {853,   480},
        {854,   480},
        {800,   600},
        {960,   540},
        {960,   640},
        {1024,  576},
        {1024,  768},
        {1152,  768},
        {1280,  720},
        {1280,  768},
        {1280,  800},
        {1360,  768},
        {1366,  768},
        {1152,  864},
        {1152,  870},
        {1152,  900},
        {1120,  832},
        {1440,  900},
        {1440,  960},
        {1280,  720},
        {1280,  960},
        {1280,  1024},
        {1400,  1050},
        {1600,  720},
        {1600,  900},
        {1600,  1200},
        {1640,  720},
        {1680,  1050},
        {1920,  720},
        {1920,  1080},
        {1920,  1200},
        {2048,  1152},
        {2048,  1536},
        {2400,  1080},
        {2408,  1080},   // poco 5
        {2560,  1440},
        {2560,  1600},
        {2880,  1600},
        {2560,  2048},
        {3200,  1800},
        {3200,  2048},
        {3200,  2400},
        {3840,  2160},
        {3840,  2400},
        {4096,  2160},
        {4096,  3072},
        {5120,  2880},
        {5120,  3200},
        {5120,  4096},
        {6400,  4096},
        {6400,  4800},
        {7680,  4320},
        {7680,  4800},
        {15360, 8640},

        {20	,	138	},
        {24	,	135	},
        {27	,	15	},
        {	27	,	30	},
        {	28	,	169	},
        {	29	,	163	},
        {	36	,	27	},
        {	51	,	36	},
        {	51	,	218	},
        {	52	,	304	},
        {	54	,	38	},
        {	54	,	48	},
        {	64	,	57	},
        {	105	,	32	},
        {	105	,	63	},
        {	121	,	84	},
        {	121	,	108	},
        {	139	,	45	},
        {	160	,	120	},
        {	240	,	160	},
        {	320	,	240	},
        {	360	,	640	},
        {	400	,	240	},
        {	480	,	320	},
        {	570	,	320	},
        {	640	,	360	},
        {	640	,	480	},
        {	800	,	480	},
        {	800	,	600	},
        {	853	,	480	},
        {	960	,	540	},
        {	1024	,	576	},
        {	1024	,	600	},
        {	1024	,	768	},
        {	1152	,	864	},
        {	1280	,	720	},
        {	1280	,	1024	},
        {	1400	,	1050	},
        {	1440	,	900	},
        {	1600	,	1200	},
        {	1680	,	1050	},
        {	1920	,	1080	},
        {	1920	,	1200	},
        // Добавьте здесь другие распространенные разрешения при необходимости
};

// Функция для чтения структуры MTK_logo из файла
bool read_MTK_logo_from_file(const std::string& filename, MTK_logo& data);

// Функция для угадывания разрешения изображения на основе размера файла и глубины цвета
Resolution guess_resolution(size_t file_size);

void write_png_file(const std::string &filename, int width, int height, std::vector<unsigned char> &data);
//void write_png_file(const char* file_name, const std::vector<unsigned char>& bgra_data, uint32_t width, uint32_t height);

// Function to convert RGBA byte buffer to BGRA
std::vector<unsigned char> rgba_to_bgra(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h);

// Функция для конвертации RGBA в BGRA
std::vector<unsigned char> rgba_to_bgra(const std::vector<unsigned char>& rgba);

// Функция для конвертации BGRA в RGBA
std::vector<unsigned char> bgra_to_rgba(const std::vector<unsigned char>& bgra);

// Function to convert RGBA byte buffer to BGRA Big-Endian
std::vector<unsigned char> rgba_to_bgra_be(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h);

// Function to convert RGBA byte buffer to BGRA Little-Endian
std::vector<unsigned char> rgba_to_bgra_le(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h);

// Function to convert RGBA byte buffer to RGBA Big-Endian
std::vector<unsigned char> rgba_to_rgba_be(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h);

void unpack_logo(const std::string &logo_file, const std::string &output_dir);

void read_png_file(const char *file_name, std::vector<unsigned char>& image);

//void write_rgba_file(const char *file_name, const std::vector<unsigned char>& image);

void pack_logo(const std::string &output_dir, const std::string &logo_file);

// Функция для распаковки данных
std::vector<unsigned char> unpack_zlib(const std::vector<char> &compressed_data);
std::vector<unsigned char> pack_zlib(const std::vector<unsigned char> &decompressed_data);

std::string replacetxt(std::string str, std::string oldSubstring, std::string newSubstring);
void copyFile(const std::string& source_path, const std::string& destination_path);
void copy_part_file(const std::string &logo_file, const std::string &output_dir, const int bytes_to_copy);
std::string getAbsolutePath(const std::string& relative_path);