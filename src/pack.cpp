#include "main.h"

void pack_logo(const std::string &output_dir, const std::string &logo_file) {
    std::vector<std::string> images;
    std::map<int, int> offsets;
    std::map<int, int> sizes;
    std::string real_path_logo = getAbsolutePath(logo_file);
    for (const auto &entry: std::filesystem::directory_iterator(output_dir)) {
        std::string file = entry.path().filename().string();
        if (file.substr(0, 4) == "img-" && file.substr(file.size() - 4) == ".png") {
            images.push_back(file);
        }
    }
    std::sort(images.begin(), images.end()); //отсортируем по порядку, на всякий случай

    std::string header_file = output_dir + "/header";
    if (std::filesystem::exists(header_file) && std::filesystem::is_regular_file(header_file)) {
        // Копируем файл с новым именем
        copyFile(header_file, logo_file);
    } else {
        std::cerr << "Error: File " << header_file << " does not exist or is not a regular file." << std::endl;
        exit(-1);
    }

    int bytes_written = 0;
    std::fstream new_logo(logo_file, std::ios::binary | std::ios::in | std::ios::out);
    if (!new_logo.is_open()) {
        std::cerr << "Error: Could not open file " << logo_file << " for writing" << std::endl;
        exit(-1);
    }
    new_logo.seekp(0, std::ios_base::end); //переходим в конец заголовочного файла
    int picture_count = images.size();
    int bloc_size = (SIZE_INT * 2) + (SIZE_INT * picture_count);
    std::cout << "Writing MTK header to file \"" << logo_file << "\" (" << SIZE_MTK_HEADER << " bytes)..." << std::endl;
    //new_logo.write(mtk_header, SIZE_MTK_HEADER);
    bytes_written += SIZE_MTK_HEADER;
    std::cout << "Writing pictures count, which is " << picture_count << " (" << SIZE_INT << " bytes)..." << std::endl;
    new_logo.write(reinterpret_cast<const char *>(&picture_count), SIZE_INT);
    bytes_written += SIZE_INT;
    new_logo.write(std::string(4, '\0').c_str(), 4); //запишем 4 байта, потом по верху запишем общий размер
    bytes_written += SIZE_INT;

    // Read and compress images, Write offsets map.
    std::cout << "Writing offsets map (" << picture_count << " * " << SIZE_INT << " = " << picture_count * SIZE_INT << " bytes)" << std::endl;
    std::cout << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << " IMAGE FILE  |   OFFSET   | SIZE (bytes) " << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    int i = 1;
    std::vector<unsigned char> tmp_images_z;
    for (const auto &img: images) {

        std::vector<unsigned char> image;
        std::string inpute_file = output_dir + "/" + img;
        read_png_file(inpute_file.c_str(), image);
        if (image.empty()) {
            std::cerr << "Error: Failed to read PNG file" << std::endl;
        }
        std::vector<unsigned char> image_z = pack_zlib(image);
        tmp_images_z.insert(tmp_images_z.end(), image_z.begin(), image_z.end()); //добавляем в конец вектора, чтобы потом всё вывалить
        sizes[i] = image_z.size();

        if (i == 1) {
            offsets[i] = bloc_size;
        } else {
            offsets[i] = offsets[i - 1] + sizes[i - 1];
        }
        new_logo.write(reinterpret_cast<const char *>(&offsets[i]), SIZE_INT);
        printf(" %s | 0x%08X | %010d\n", img.c_str(), offsets[i], sizes[i]);
        std::fflush(stdout);
        bytes_written += SIZE_INT;
        i++;
    }
    bloc_size += tmp_images_z.size();
    new_logo.write(reinterpret_cast<const char *>(tmp_images_z.data()), tmp_images_z.size());
    bytes_written += tmp_images_z.size();

    std::cout << "---------------------------------------" << std::endl;
    printf("                              %zu\n", tmp_images_z.size());
    std::cout << std::endl;

    std::cout << "Total block size (8 bytes + map + pictures): " << bloc_size << " bytes"<< std::endl;
    std::cout << "Writing total block size (" << SIZE_INT << " bytes)..." << std::endl;
    new_logo.seekp(SIZE_INT, std::ios_base::beg);
    new_logo.write(reinterpret_cast<const char *>(&bloc_size), SIZE_INT);
    new_logo.seekp(516, std::ios_base::beg);
    new_logo.write(reinterpret_cast<const char *>(&bloc_size), SIZE_INT);
    //std::cout << "Done! The file was successfully created at path: \n" << real_path_logo << std::endl;
    printf("Done! The file was successfully created at path:\n%s\n", real_path_logo.c_str());
}
