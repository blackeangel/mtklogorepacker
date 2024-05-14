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
    uint32_t block[1];                  //offset 0x200 number of pictures (little-endian value)
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
        //Hight, Wight
        {1920,  1080},   // Full HD
        {1280,  720},    // HD
        {1366,  768},    // HD
        {2560,  1440},   // 2K
        {3840,  2160},   // 4K
        {4096,  2160},    // 4K DCI
        {2408,  1080},   //poco 5
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
        {640,   480},
        {640,   384},
        {720,   480},
        {768,   480},
        {800,   480},
        {848,   480},
        {852,   480},
        {853,   480},
        {854,   480},
        {800,   600},
        {960,   640},
        {1024,  576},
        {1024,  768},
        {1152,  768},
        {1280,  768},
        {1280,  800},
        {1360,  768},
        {1366,  768},
        {1152,  864},
        {1152,  900},
        {1152,  870},
        {1120,  832},
        {1440,  900},
        {1440,  960},
        {1280,  1024},
        {1280,  960},
        {1400,  1050},
        {1680,  1050},
        {1600,  1200},
        {1920,  1200},
        {2048,  1152},
        {2048,  1536},
        {2560,  1600},
        {2880,  1600},
        {2560,  2048},
        {3200,  2048},
        {3200,  2400},
        {3840,  2400},
        {4096,  3072},
        {5120,  3200},
        {5120,  4096},
        {6400,  4096},
        {6400,  4800},
        {7680,  4800},
        {640,   360},
        {960,   540},
        {1280,  720},
        {1600,  900},
        {1920,  1080},
        {2560,  1440},
        {3200,  1800},
        {3840,  2160},
        {5120,  2880},
        {7680,  4320},
        {15360, 8640},
        {54,    38},
        {54,    48},
        {24,    135},
        {1,     135},
        {2,     79},
        {32,    34},
        {32,    23}
        // Добавьте здесь другие распространенные разрешения при необходимости
};

//unpack voids -->
// Функция для чтения структуры MTK_logo из файла
bool read_MTK_logo_from_file(const std::string& filename, MTK_logo& data);
// Функция для угадывания разрешения изображения на основе размера файла и глубины цвета
Resolution guess_resolution(size_t file_size);
void write_png_file(const std::string &filename, int width, int height, std::vector<unsigned char> &data);
int ffff(std::string output_png, std::vector<unsigned char> &data);
// Функция для распаковки данных
void unpack_zlib(const std::vector<char> &compressed_data, std::string output_file);
void unpack_logo(const std::string &logo_file, const std::string &output_dir);
//<-- unpack voids
//pack voids -->
std::string replacetxt(std::string str,std::string oldSubstring, std::string newSubstring);
void read_png_file(const char *file_name, std::vector<unsigned char>& image);
void write_rgba_file(const char *file_name, const std::vector<unsigned char>& image);
void pack_logo(const std::string &output_dir, const std::string &logo_file);
//<-- pack voids