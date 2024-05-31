#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "../zlib/zlib.h"
#include "../libpng/png.h"
#include "../minizip/zip.h"
#include "../minizip/unzip.h"



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
        {1,      102},
        {1,      135},
        {1,      163},
        {1,	     169},
        {1,	     304},
        {2,       79},
        {2,      152},
        {20,	 138},
        {24,     135},
        {24,     152},
        {27,	  15},
        {27,	  30},
        {28,	 169},
        {29,	 163},
        {32,      23},
        {32,      34},
        {36,	  27},
        {36,      32},
        {40,      28},
        {40,      32},
        {51,	  36},
        {51,	 218},
        {52,	 304},
        {54,      38},
        {54,      48},
        {57,	  64},
        {64,	  45},
        {64,	  57},
        {105,	  32},
        {105,	  63},
        {121,	  84},
        {121,	 108},
        {128,	  72},
        {139,	  45},
        {160,    120},
        {214,    286},
        {240,    160},
        {250,    680},
        {263,      2},
        {263,     47},
        {320,    240},
        {360,    240},
        {360,	 640},
        {376,    240},
        {384,    240},
        {400,    240},
        {428,    240},
        {432,    240},
        {445,    197},
        {480,    270},
        {480,    272},
        {480,    320},
        {480,    360},
        {570,	 320},
        {640,    360},
        {640,    384},
        {640,    480},
        {720,    480},
        {768,    480},
        {800,    480},
        {800,	 600},
        {800,   2560},
        {848,    480},
        {852,    480},
        {853,    480},
        {854,    480},
        {856,	 480},
        {960,    540},
        {960,    640},
        {1024,   576},
        {1024,	 600},
        {1024,   768},
        {1152,   768},
        {1280,   720},
        {1280,   768},
        {1280,   800},
        {1360,   768},
        {1366,   768},
        {1152,   864},
        {1152,   870},
        {1152,   900},
        {1120,   832},
        {1280,   720},
        {1280,   800},
        {1280,   960},
        {1280,  1024},
        {1400,  1050},
        {1440,   900},
        {1440,   960},
        {1600,   720},
        {1600,   900},
        {1600,  1200},
        {1640,   720},
        {1680,  1050},
        {1920,   720},
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

//Функция замены текста
std::string replacetxt(std::string str, std::string oldSubstring, std::string newSubstring);

//Функция копировангия файла
void copyFile(const std::string& source_path, const std::string& destination_path);

//Функция копирования части файла
void copy_part_file(const std::string &logo_file, const std::string &output_dir, const int bytes_to_copy);
//Функция получения абсолютного пути
std::string getAbsolutePath(const std::string& relative_path);
// Функция для получения имени файла из пути
std::string get_filename_from_path(const std::string& path);
// Функция для получения пути без расширения файла
std::string get_path_without_extension(const std::string &path);
// Функция для получения имени файла без расширения
std::string get_filename_without_extension(const std::string& path);
// Функция для получения расширения файла
std::string get_file_extension(const std::string& path);
// Функция для получения текущего времени в формате "YYYYMMDDHHMMSS"
std::string get_current_time_str();

//zip-->
//
void pack_to_zip(const std::vector<unsigned char>& data, const std::string& entry_name);
// Функция для распаковки ZIP архива
void extract_zip(const std::string& zip_path, const std::string& extract_dir);
// Функция для добавления файла в ZIP архив
void add_file_in_zip(const std::string& zip_path, const std::string& file_path, const std::string& entry_name);
// Функция для вывода списка файлов в ZIP архиве
void list_files_in_zip(const std::string& zip_path);
// Функция для создания папки в ZIP архиве
void add_folder_to_zip(const std::string& zip_path, const std::string& folder_name);
// Функция для распаковки файлов из ZIP архива
void extract_files_from_zip(const std::string& zip_path, const std::string& extract_dir, const std::vector<std::string>& files_to_extract);
// Функция для добавления файла в ZIP архив
void add_file_to_zip(zipFile zf, const std::string& file_path, const std::string& entry_name);
// Функция для добавления нескольких файлов или папок в ZIP архив
void add_files_to_zip(const std::string& zip_path, const std::vector<std::string>& files, std::string& base_path);
// Функция для записи строки в файл внутри ZIP архива
void write_string_to_zip(const std::string& zip_filename, const std::string& file_inside_zip, const std::string& content);
// <--zip