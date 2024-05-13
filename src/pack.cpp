#include "main.h"

bool compare_images(const std::string &a, const std::string &b) {
    return a < b;
}
void copyFile(const std::string& source_path, const std::string& destination_path) {
    try {
        // Если целевой файл уже существует, удаляем его
        if (std::filesystem::exists(destination_path)) std::filesystem::remove(destination_path);
        // Копируем файл
        std::filesystem::copy_file(source_path, destination_path, std::filesystem::copy_options::overwrite_existing);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
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

void read_png_file(const char *file_name, std::vector<unsigned char> &image) {
    png_byte header[8];
    int width;
    int height;
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        std::cerr << "Error: File " << file_name << " could not be opened for reading" << std::endl;
        return;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fclose(fp);
        std::cerr << "Error: " << file_name << " is not a PNG file" << std::endl;
        return;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        std::cerr << "Error: png_create_read_struct failed" << std::endl;
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        std::cerr << "Error: png_create_info_struct failed" << std::endl;
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        std::cerr << "Error: Error during init_io" << std::endl;
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    image.resize(rowbytes * height);

    std::vector<png_bytep> row_pointers(height);
    for (int y = 0; y < height; y++)
        row_pointers[y] = &image[y * rowbytes];

    png_read_image(png_ptr, row_pointers.data());

    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

}

void write_rgba_file(const char *file_name, const std::vector<unsigned char> &image) {
    std::ofstream file(file_name, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_name << " for writing" << std::endl;
        return;
    }
    file.write(reinterpret_cast<const char *>(image.data()), image.size());
}

void pack_logo(const std::string &output_dir, const std::string &logo_file) {
    std::vector<std::string> images;
    std::map<int, int> offsets;
    std::map<int, int> sizes;

    for (const auto &entry: std::filesystem::directory_iterator(output_dir)) {
        std::string file = entry.path().filename().string();
        if (file.substr(0, 4) == "img-" && file.substr(file.size() - 4) == ".png") {

            std::vector<unsigned char> image;
            std::string inpute_file = output_dir + "/" + file;
            read_png_file(inpute_file.c_str(), image);
            if (image.empty()) {
                std::cerr << "Error: Failed to read PNG file" << std::endl;
            }
            std::string output_file = output_dir + "/" + replacetxt(file, "png", "raw");
            write_rgba_file(output_file.c_str(), image);

            images.push_back(output_file);
        }
    }
    std::sort(images.begin(), images.end());
    std::string header_file = output_dir + "/header";
    if (std::filesystem::exists(header_file) && std::filesystem::is_regular_file(header_file)) {
        // Копируем файл с новым именем
        copyFile(header_file, logo_file);
    } else {
        std::cerr << "Error: File " << header_file << " does not exist or is not a regular file." << std::endl;
    }
/*
    std::ifstream logo_bin(LOGO_BIN, std::ios::binary);
    if (!logo_bin.is_open()) {
        std::cerr << "Error: Could not open file " << LOGO_BIN << std::endl;
        exit(-1);
    }

    std::cout << std::endl;
    std::cout << "Reading MTK header from file \"" << LOGO_BIN << "\" (" << SIZE_MTK_HEADER << " bytes)" << std::endl;
    char mtk_header[SIZE_MTK_HEADER];
    logo_bin.read(mtk_header, SIZE_MTK_HEADER);
    if (std::string_view(mtk_header + 8, 4) == "LOGO") {
        std::cout << "Found \"LOGO\" signature at offset 0x08" << std::endl;
    } else {
        std::cout << "No \"LOGO\" signature found at offset 0x08, continue anyway..." << std::endl;
    }
*/
    int bytes_written = 0;
    std::ofstream new_logo(logo_file, std::ios::binary| std::ios::app);
    if (!new_logo.is_open()) {
        std::cerr << "Error: Could not open file " << logo_file << " for writing" << std::endl;
        exit(-1);
    }
    new_logo.seekp(0, std::ios_base::end); //переходим в конец заголовочного файла
    int picture_count = images.size();
    int bloc_size = (SIZE_INT * 2) + (SIZE_INT * picture_count);
    //std::cout << "Writing MTK header to file \"" << logo_file << "\" (" << SIZE_MTK_HEADER << " bytes)..." << std::endl;
    //new_logo.write(mtk_header, SIZE_MTK_HEADER);
    //bytes_written += SIZE_MTK_HEADER;
    //std::cout << "Writing pictures count, which is " << picture_count << " (" << SIZE_INT << " bytes)..." << std::endl;
    //new_logo.write(reinterpret_cast<const char*>(&picture_count), SIZE_INT);
    //bytes_written += SIZE_INT;

    // Read and compress images.
    int i = 0;
    for (const auto& img : images) {
        std::filesystem::path filename_bin = img;
        std::filesystem::path filename_z = output_dir +"/"+ filename_bin.stem().string() + ".z";

        std::ifstream f(filename_bin, std::ios::binary);
        if (!f.is_open()) {
            std::cerr << "Error: Could not open file " << filename_bin << std::endl;
            exit(-1);
        }
        std::vector<char> image_bin((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        f.close();

        std::vector<char> image_z;
        if (std::filesystem::exists(filename_z)) {
            std::ifstream fz(filename_z, std::ios::binary);
            if (!fz.is_open()) {
                std::cerr << "Error: Could not open file " << filename_z << std::endl;
                exit(-1);
            }
            image_z = std::vector<char>((std::istreambuf_iterator<char>(fz)), std::istreambuf_iterator<char>());
        } else {
            std::vector<char> compressed(image_bin.size() * 2);
            uLongf compressed_size = compressed.size();
            compress2(reinterpret_cast<Bytef*>(compressed.data()), &compressed_size, reinterpret_cast<const Bytef*>(image_bin.data()), image_bin.size(), Z_BEST_COMPRESSION);
            compressed.resize(compressed_size);
            std::ofstream fz(filename_z, std::ios::binary);
            if (!fz.is_open()) {
                std::cerr << "Error: Could not open file " << filename_z << " for writing" << std::endl;
                exit(-1);
            }
            fz.write(compressed.data(), compressed.size());
            image_z = std::move(compressed);
        }

        sizes[i] = image_z.size();
        if (i == 0) {
            offsets[i] = bloc_size;
        } else {
            offsets[i] = offsets[i - 1] + sizes[i - 1];
        }
        bloc_size += sizes[i];
        i++;
    }
    std::cout << "File " << " created successfully" << std::endl;
/*
    std::cout << "Total block size (8 bytes + map + pictures): " << bloc_size << std::endl;
    std::cout << "Writing total block size (" << SIZE_INT << " bytes)..." << std::endl;
    new_logo.write(reinterpret_cast<const char*>(&bloc_size), SIZE_INT);
    bytes_written += SIZE_INT;
*/
    // Write offsets map.
    std::cout << "Writing offsets map (" << picture_count << " * " << SIZE_INT << " = " << picture_count * SIZE_INT << " bytes)" << std::endl;
    std::cout << std::endl;
    std::cout << "    img   |   offset   | size (bytes)" << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    int images_size = 0;
    for (int i = 0; i < picture_count; ++i) {
        std::cout << "     " << std::setw(2) << std::setfill('0') << i + 1 << "   | 0x" << std::hex << std::setw(8) << std::setfill('0') << offsets[i] << " | " << std::dec << std::setw(10) << sizes[i] << std::endl;
        new_logo.write(reinterpret_cast<const char*>(&offsets[i]), SIZE_INT);
        bytes_written += SIZE_INT;
        images_size += sizes[i];
    }
    // Write compressed images.
    for (const auto& img : images) {
        std::filesystem::path filename_bin = img;
        std::filesystem::path filename_z = output_dir +"/"+ filename_bin.stem().string() + ".z";

        std::ifstream fz(filename_z, std::ios::binary);
        if (!fz.is_open()) {
            std::cerr << "Error: Could not open file " << filename_z << std::endl;
            exit(-1);
        }
        std::vector<char> image_z((std::istreambuf_iterator<char>(fz)), std::istreambuf_iterator<char>());
        new_logo.write(image_z.data(), image_z.size());
        bytes_written += image_z.size();
    }
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "                       " << images_size << std::endl;
    std::cout << std::endl;
    // Pad to 32 bytes
    int pad_len = PAD_TO - (bytes_written % PAD_TO);
    std::cout << "Writing " << PAD_TO << " bytes to pad to " << pad_len << std::endl;
    new_logo.write(std::string(pad_len, '\0').c_str(), pad_len);
    bytes_written += pad_len;

}
