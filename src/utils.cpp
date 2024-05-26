#include "main.h"

std::string getAbsolutePath(const std::string& relative_path) {
    try {
        // Получаем абсолютный путь
        std::filesystem::path absolute_path = std::filesystem::absolute(relative_path);
        // Возвращаем абсолютный путь в виде std::string
        return absolute_path.string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return "";
    }
}

void copyFile(const std::string &source_path, const std::string &destination_path) {
    try {
        // Если целевой файл уже существует, удаляем его
        if (std::filesystem::exists(destination_path)) std::filesystem::remove(destination_path);
        // Копируем файл
        std::filesystem::copy_file(source_path, destination_path, std::filesystem::copy_options::overwrite_existing);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(-1);
    }
}

std::string replacetxt(std::string str, std::string oldSubstring, std::string newSubstring) {
    size_t pos = str.find(oldSubstring);
    if (pos != std::string::npos) {
        str.replace(pos, oldSubstring.length(), newSubstring);
    } else {
        std::cout << "Substring not found" << std::endl;
    }
    return str;
}

// Функция для распаковки данных
std::vector<unsigned char> unpack_zlib(const std::vector<char> &compressed_data) {
    std::vector<unsigned char> uncompressed_data_tmp(3000 * 3000 * 4, 0); // Увеличиваем размер буфера для распакованных данных
    uLongf destLen = uncompressed_data_tmp.size();
    int result = uncompress(reinterpret_cast<Bytef *>(uncompressed_data_tmp.data()), &destLen, reinterpret_cast<const Bytef *>(compressed_data.data()), compressed_data.size());
    uncompressed_data_tmp.resize(destLen); // Уменьшаем размер до оригинального
    std::vector<unsigned char> uncompressed_data = uncompressed_data_tmp;
    if (!(result == Z_OK)) {
        std::cerr << "Error: zlib decompression failed with error code " << result << std::endl;
        exit(-1);
    }
    return uncompressed_data;
}

std::vector<unsigned char> pack_zlib(const std::vector<unsigned char> &decompressed_data) {
    // Создаем вектор для сжатых данных
    std::vector<unsigned char> compressed_data;

    // Размер буфера для сжатия
    uLong source_len = decompressed_data.size();
    uLong dest_len = compressBound(source_len);

    // Выделяем память для буфера сжатых данных
    compressed_data.resize(dest_len);

    // Сжимаем данные с максимальным уровнем сжатия
    int result = compress2(&compressed_data[0], &dest_len, &decompressed_data[0], source_len, Z_BEST_COMPRESSION);
    if (result != Z_OK) {
        std::cerr << "Compression failed: " << result << std::endl;
        return {};
    }

    // Уменьшаем размер вектора до фактического размера сжатых данных
    compressed_data.resize(dest_len);

    return compressed_data;
}

void copy_part_file(const std::string &logo_file, const std::string &output_dir, const int bytes_to_copy) {
// Открываем входной файл для чтения в бинарном режиме
    std::ifstream input_file(logo_file, std::ios::binary);

    if (!input_file) {
        std::cerr << "Error: Unable to open input file " << logo_file <<
                  std::endl;
        exit(-1);
    }
    std::string output_filename = output_dir + "/header";
// Открываем выходной файл для записи в бинарном режиме
    std::ofstream output_file(output_filename, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Unable to open output file " << output_filename << std::endl;
        exit(-1);
    }

// Читаем первые bytes_to_copy байт из входного файла
    char buffer[bytes_to_copy];
    input_file.read(buffer, bytes_to_copy);

    if (!input_file) {
        std::cerr << "Error: Unable to read from input file " << logo_file << std::endl;
        exit(-1);
    }

// Записываем прочитанные байты в выходной файл
    output_file.write(buffer, bytes_to_copy);

    if (!output_file) {
        std::cerr << "Error: Unable to write to output file " << output_filename << std::endl;
        exit(-1);
    }

// Закрываем файлы
    input_file.close();
    output_file.close();
}

// Функция для получения текущего времени в формате "YYYYMMDDHHMMSS"
std::string get_current_time_str() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d%H%M%S");
    return ss.str();
}


// Функция для упаковки std::vector<unsigned char> в ZIP архив
/*
 std::string entry_name = "hello.txt";
    try {
        pack_to_zip(data, entry_name);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
 */
void pack_to_zip(const std::vector<unsigned char>& data, const std::string& entry_name) {
    std::string zip_filename = "logo_" + get_current_time_str() + ".zip";

    zipFile zf = zipOpen(zip_filename.c_str(), APPEND_STATUS_CREATE);
    if (zf == nullptr) {
        throw std::runtime_error("Could not open zip archive for writing");
    }

    zip_fileinfo zi;
    memset(&zi, 0, sizeof(zip_fileinfo));

    int err = zipOpenNewFileInZip(zf, entry_name.c_str(), &zi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    if (err != ZIP_OK) {
        zipClose(zf, nullptr);
        throw std::runtime_error("Could not open new file in zip archive");
    }

    err = zipWriteInFileInZip(zf, data.data(), data.size());
    if (err != ZIP_OK) {
        zipCloseFileInZip(zf);
        zipClose(zf, nullptr);
        throw std::runtime_error("Could not write data to zip archive");
    }

    zipCloseFileInZip(zf);
    zipClose(zf, nullptr);

    std::cout << "Данные успешно упакованы в " << zip_filename << std::endl;
}
