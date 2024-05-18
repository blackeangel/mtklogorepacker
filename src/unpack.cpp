#include "main.h"

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
        picture_count = data.count_pictures[0];
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

    printf("--------------------------------------------------------\n");
    printf("%8s  |  %10s  |  %6s |  %s\n", "NUMBER", "OFFSET", "SIZE (bytes)", "FILE");
    printf("--------------------------------------------------------\n");

    int images_size = 0;

    if (!std::filesystem::exists(output_dir)) {
        std::filesystem::create_directories(output_dir);
    }
    std::string real_folder = getAbsolutePath(output_dir);

    for (int i = 0; i < picture_count; ++i) {
        std::vector<char> image_z(sizes[i]);
        logo_bin.read(image_z.data(), sizes[i]);
        images_size += sizes[i];
        std::printf("    %03d   |   0x%06X   |   %010d  |  img-%03d.png\n", i + 1, offsets[i], sizes[i], i + 1);
        std::fflush(stdout);
        char filename_l[256];
        std::string prefix = output_dir + "/img-";
        std::sprintf(filename_l, "%s%03d%s", prefix.c_str(), i+1, ".png");
        std::string filename = filename_l;

        std::vector<unsigned char> data = unpack_zlib(image_z);
        // Угадываем разрешение изображения
        Resolution guessed_resolution = guess_resolution(data.size());

        // Вычисляем ширину и высоту изображения
        int width = guessed_resolution.width; // Замените это на фактическую ширину изображения
        int height = guessed_resolution.height; // Замените это на фактическую высоту изображения

        // Создаем файл PNG
        //write_png_file(filename.data(), width, height, data);

        try {
            // Конвертируем данные RGBA в данные BGRA BE
            //std::vector<unsigned char> bgra_data = rgba_to_bgra(data, width, height);
            std::vector<unsigned char> bgra_data = rgba_to_bgra(data);
            // Сохраняем данные BGRA в PNG файл
            write_png_file(filename, width, height, bgra_data);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            exit(-1);
        }

    }

    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "                             " << images_size << std::endl;
    std::cout << std::endl;
    //
    copy_part_file(logo_file, output_dir, sizeof(MTK_logo) - 8);
    printf("All files are extracted to the folder: \n\"%s\"\n", real_folder.c_str());
}