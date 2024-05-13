#include "main.h"

const std::string LOGO_REPACK = "logo.repack.bin";

/*===================================================*/

// Функция для чтения структуры MTK_logo из файла
bool read_MTK_logo_from_file(const std::string& filename, MTK_logo& data) {
    // Открываем файл для чтения в бинарном режиме
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Couldn't open " << filename << std::endl;
        return false;
    }

    // Считываем данные из файла и сохраняем их в структуру MTK_logo
    file.read(reinterpret_cast<char*>(&data), sizeof(data));

    // Проверяем, что чтение прошло успешно
    if (!file) {
        std::cerr << "Error: Couldn't read data from " << filename << std::endl;
        return false;
    }

    // Закрываем файл
    file.close();

    return true;
}
// Функция для угадывания разрешения изображения на основе размера файла и глубины цвета
Resolution guess_resolution(size_t file_size) {
    // Предполагаемое количество пикселей в изображении (ширина * высота)
    size_t total_pixels = file_size / 4; // 4 байта на пиксель (32 бита)

    // Предполагаемая сторона изображения (ширина и высота)
    int assumed_side = static_cast<int>(sqrt(total_pixels));

    // Ищем подходящее разрешение из списка распространенных разрешений
    for (const auto &resolution: common_resolutions) {
        // Проверяем, что ширина и высота соответствуют сторонам изображения
        if (resolution.width * resolution.height == total_pixels) {
            return resolution;
        }
    }

    // Если не удалось найти точное соответствие, вернем предполагаемое разрешение
    return {assumed_side, assumed_side};
}

void write_png_file(const std::string &filename, int width, int height, std::vector<unsigned char> &data) {
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Error: Couldn't open " << filename << " for writing." << std::endl;
        return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        std::cerr << "Error: png_create_write_struct failed" << std::endl;
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, nullptr);
        std::cerr << "Error: png_create_info_struct failed" << std::endl;
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        std::cerr << "Error: PNG error during writing" << std::endl;
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    std::vector<png_bytep> row_pointers(height);
    for (int y = 0; y < height; ++y) {
        row_pointers[y] = data.data() + y * width * 4; // 4 channels (RGBA)
    }

    png_write_image(png_ptr, row_pointers.data());
    png_write_end(png_ptr, nullptr);

    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

int ffff(std::string output_png, std::vector<unsigned char> &data) {

    // Угадываем разрешение изображения
    Resolution guessed_resolution = guess_resolution(data.size());

    // Вычисляем ширину и высоту изображения
    int width = guessed_resolution.width; // Замените это на фактическую ширину изображения
    int height = guessed_resolution.height; // Замените это на фактическую высоту изображения

    // Создаем файл PNG
    write_png_file(output_png, width, height, data);

    return 0;
}

// Функция для распаковки данных
void unpack_zlib(const std::vector<char> &compressed_data, std::string output_file) {
    std::vector<unsigned char> uncompressed_data_tmp(3000 * 3000 * 4, 0); // Увеличиваем размер буфера для распакованных данных
    uLongf destLen = uncompressed_data_tmp.size();
    int result = uncompress(reinterpret_cast<Bytef *>(uncompressed_data_tmp.data()), &destLen, reinterpret_cast<const Bytef *>(compressed_data.data()), compressed_data.size());
    uncompressed_data_tmp.resize(destLen); // Уменьшаем размер до оригинального
    std::vector<unsigned char> uncompressed_data = uncompressed_data_tmp;
    if (result == Z_OK) {
        auto tt = ffff(output_file, uncompressed_data);
    } else {
        std::cerr << "Error: zlib decompression failed with error code " << result << std::endl;
    }
}

void unpack_logo(const std::string &logo_file, const std::string &output_dir) {
    std::ifstream logo_bin(logo_file, std::ios::binary);
    if (!logo_bin.is_open()) {
        std::cerr << "Error: Couldn't open " << logo_file << std::endl;
        return;
    }

    int picture_count;
    int block_size;
    // Создаем экземпляр структуры MTK_logo для хранения данных
    MTK_logo data;

    // Читаем структуру MTK_logo из файла
    if (read_MTK_logo_from_file(logo_file, data)) {
        std::cout << "Found \"" << data.header.magic << "\" signature at offset 0x08" << std::endl;
        picture_count = data.block[0];
        std::cout << "File contains " << picture_count << " pictures!" << std::endl;
        block_size = data.total_blocks[0];
        std::cout << "Total block size (8 bytes + map + pictures): " << std::to_string(block_size) << " bytes" << std::endl;
    }else
    {
        std::cout << "Unsupport logo.bin" <<std::endl;
        exit(-1);
    }

    std::map<int, int> offsets;
    std::map<int, int> sizes;
    logo_bin.seekg(sizeof(MTK_logo)); //offset 0x208: starts of pictures offset's
    for (int i = 0; i < picture_count; ++i) {
        int offset;
        logo_bin.read(reinterpret_cast<char *>(&offset), SIZE_INT);
        offsets[i] = offset;
    }

    for (int i = 0; i < picture_count - 1; ++i) {
        sizes[i] = offsets[i + 1] - offsets[i];
    }
    sizes[picture_count - 1] = block_size - offsets[picture_count - 1];

    std::cout << std::endl;
    std::cout << "    img   |   offset   | size (bytes)   |  file" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    int images_size = 0;
    if (!std::filesystem::exists(output_dir)) {
        std::filesystem::create_directories(output_dir);
    }
    for (int i = 0; i < picture_count; ++i) {
        std::vector<char> image_z(sizes[i]);
        logo_bin.read(image_z.data(), sizes[i]);
        images_size += sizes[i];
        std::printf("    %03d   |   0x%06X   |   %07d   |   img-%03d.png\n", i + 1, offsets[i], sizes[i], i + 1);
        std::fflush(stdout);
       /*std::cout << "    " << std::setw(3) << std::setfill('0') << i + 1 << "   |   "
                  << "0x" << std::hex << std::setw(6) << std::setfill('0') << offsets[i] << "   |   "
                  << std::dec << std::setw(7) << std::setfill('0') << sizes[i] << "   |   "
                  << "img-" << std::setw(3) << std::setfill('0') << i + 1 << ".png" << std::endl;
       */
        char filename_l[256];
        std::string prefix = output_dir + "/img-";
        std::sprintf(filename_l, "%s%03d%s", prefix.c_str(), i+1, ".png");
        std::string filename = filename_l;
        unpack_zlib(image_z, filename.data());
    }

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "                         " << images_size << std::endl;
    std::cout << std::endl;

    //скопируем голову в новый файл, чтобы было с чем работать потом -->
    const int bytes_to_copy = sizeof(MTK_logo)-8;
    // Открываем входной файл для чтения в бинарном режиме
    std::ifstream input_file(logo_file, std::ios::binary);
    if (!input_file) {
        std::cerr << "Error: Unable to open input file " << logo_file << std::endl;
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