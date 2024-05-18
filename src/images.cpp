#include "main.h"

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

uint16_t rgba2rgb565(uint32_t color32) {
    uint16_t r = ((color32 & 0xF8000000) >> 16) & 0b11111;
    uint16_t g = ((color32 & 0x00FC0000) >> 13) & 0b111111;
    uint16_t b = ((color32 & 0x0000F800) >> 11) & 0b11111;
    return (r << 11) | (g << 5) | b;
}

uint32_t rgb5652rgba(uint16_t color16) {
    uint32_t r = (static_cast<uint32_t>(color16 & 0xF800) << 16) & 0xFF000000;
    uint32_t g = (static_cast<uint32_t>(color16 & 0x07E0) << 13) & 0x00FF0000;
    uint32_t b = (static_cast<uint32_t>(color16 & 0x001F) << 11) & 0x0000FF00;
    return r | g | b | 0x000000FF;
}

// Helper function to convert RGBA to BGRA
uint32_t rgba2bgra(uint32_t color32) {
    uint8_t r = (color32 >> 24) & 0xFF;
    uint8_t g = (color32 >> 16) & 0xFF;
    uint8_t b = (color32 >> 8) & 0xFF;
    uint8_t a = color32 & 0xFF;
    return (b << 24) | (g << 16) | (r << 8) | a;
}

// Helper function to convert RGBA to BGRA Big-Endian
uint32_t rgba2bgra_be(uint32_t color32) {
    uint8_t r = (color32 >> 24) & 0xFF;
    uint8_t g = (color32 >> 16) & 0xFF;
    uint8_t b = (color32 >> 8) & 0xFF;
    uint8_t a = color32 & 0xFF;
    return (b << 24) | (g << 16) | (r << 8) | a;
}

// Helper function to convert RGBA to BGRA Little-Endian
uint32_t rgba2bgra_le(uint32_t color32) {
    uint8_t r = (color32 >> 24) & 0xFF;
    uint8_t g = (color32 >> 16) & 0xFF;
    uint8_t b = (color32 >> 8) & 0xFF;
    uint8_t a = color32 & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | b;
}
/*
// Function to convert RGBA byte buffer to BGRA
std::vector<unsigned char> rgba_to_bgra(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h) {
    size_t pixels = w * h;
    std::vector<unsigned char> bgra_data(pixels * 4);

    for (size_t i = 0; i < pixels; ++i) {
        uint32_t color32;
        std::memcpy(&color32, &rgba_data[i * 4], sizeof(color32));
        color32 = __builtin_bswap32(color32); // Ensure RGBA is in BigEndian
        uint32_t bgra_color = rgba2bgra(color32);
        std::memcpy(&bgra_data[i * 4], &bgra_color, sizeof(bgra_color));
    }

    return bgra_data;
}
*/
// Функция для конвертации RGBA в BGRA
std::vector<unsigned char> rgba_to_bgra(const std::vector<unsigned char>& rgba) {
    if (rgba.size() % 4 != 0) {
        throw std::invalid_argument("Input data size is not a multiple of 4");
    }

    std::vector<unsigned char> bgra(rgba.size());

    for (size_t i = 0; i < rgba.size(); i += 4) {
        bgra[i] = rgba[i + 2];     // B
        bgra[i + 1] = rgba[i + 1]; // G
        bgra[i + 2] = rgba[i];     // R
        bgra[i + 3] = rgba[i + 3]; // A
    }

    return bgra;
}

// Функция для конвертации BGRA в RGBA
std::vector<unsigned char> bgra_to_rgba(const std::vector<unsigned char>& bgra) {
    if (bgra.size() % 4 != 0) {
        throw std::invalid_argument("Input data size is not a multiple of 4");
    }

    std::vector<unsigned char> rgba(bgra.size());

    for (size_t i = 0; i < bgra.size(); i += 4) {
        rgba[i] = bgra[i + 2];     // R
        rgba[i + 1] = bgra[i + 1]; // G
        rgba[i + 2] = bgra[i];     // B
        rgba[i + 3] = bgra[i + 3]; // A
    }

    return rgba;
}

// Function to convert RGBA byte buffer to BGRA Big-Endian
std::vector<unsigned char> rgba_to_bgra_be(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h) {
    size_t pixels = w * h;
    std::vector<unsigned char> bgra_data(pixels * 4);

    for (size_t i = 0; i < pixels; ++i) {
        uint32_t color32;
        std::memcpy(&color32, &rgba_data[i * 4], sizeof(color32));
        color32 = __builtin_bswap32(color32); // Ensure RGBA is in BigEndian
        uint32_t bgra_color = rgba2bgra_be(color32);
        std::memcpy(&bgra_data[i * 4], &bgra_color, sizeof(bgra_color));
    }

    return bgra_data;
}

// Function to convert RGBA byte buffer to BGRA Little-Endian
std::vector<unsigned char> rgba_to_bgra_le(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h) {
    size_t pixels = w * h;
    std::vector<unsigned char> bgra_data(pixels * 4);

    for (size_t i = 0; i < pixels; ++i) {
        uint32_t color32;
        std::memcpy(&color32, &rgba_data[i * 4], sizeof(color32));
        uint32_t bgra_color = rgba2bgra_le(color32);
        std::memcpy(&bgra_data[i * 4], &bgra_color, sizeof(bgra_color));
    }

    return bgra_data;
}

// Function to convert RGBA byte buffer to RGBA Big-Endian
std::vector<unsigned char> rgba_to_rgba_be(const std::vector<unsigned char>& rgba_data, uint32_t w, uint32_t h) {
    size_t pixels = w * h;
    std::vector<unsigned char> rgba_be_data(pixels * 4);

    for (size_t i = 0; i < pixels; ++i) {
        uint32_t color32;
        std::memcpy(&color32, &rgba_data[i * 4], sizeof(color32));
        color32 = __builtin_bswap32(color32); // Swap to Big-Endian
        std::memcpy(&rgba_be_data[i * 4], &color32, sizeof(color32));
    }

    return rgba_be_data;
}

std::vector<uint8_t> u32be_to_u32le(std::istream& reader, size_t words) {
    std::vector<uint8_t> rgbale;
    rgbale.reserve(words * sizeof(uint32_t));
    
    for (size_t i = 0; i < words; ++i) {
        // Read color32 in big endian.
        uint32_t color32;
        reader.read(reinterpret_cast<char*>(&color32), sizeof(uint32_t));
        
        // Convert color32 from big endian to little endian and from RGBA to BGRA.
        uint32_t bgra;
        std::memcpy(&bgra, &color32, sizeof(uint32_t));
        bgra = rgba2bgra(bgra);
        
        // Write bgra as little endian into rgbale.
        for (size_t j = 0; j < sizeof(uint32_t); ++j) {
            rgbale.push_back(reinterpret_cast<uint8_t*>(&bgra)[j]);
        }
    }
    return rgbale;
}
/*
template <typename O, typename R>
std::vector<uint8_t> rgba_to_bgra(R& reader, uint32_t w, uint32_t h) {
    size_t pixels = static_cast<size_t>(w * h);
    std::vector<uint8_t> bgra;
    bgra.reserve(pixels * 4);
    
    for (size_t i = 0; i < pixels; ++i) {
        // 'pivot' rgba is always BigEndian.
        uint32_t color32;
        reader.read(reinterpret_cast<char*>(&color32), sizeof(uint32_t));
        
        // Convert color32 from RGBA to BGRA.
        uint32_t bgra_color = rgba2bgra(color32);
        
        // Write bgra_color into bgra vector.
        for (size_t j = 0; j < sizeof(uint32_t); ++j) {
            bgra.push_back(reinterpret_cast<uint8_t*>(&bgra_color)[j]);
        }
    }
    return bgra;
}
*/
template <typename O, typename R>
std::vector<uint8_t> rgba_to_rgb565(R& reader, uint32_t w, uint32_t h) {
    size_t pixels = static_cast<size_t>(w * h);
    std::vector<uint8_t> rgb565;
    rgb565.reserve(pixels * 2);
    
    for (size_t i = 0; i < pixels; ++i) {
        // 'pivot' rgba is always BigEndian.
        uint32_t color32;
        reader.read(reinterpret_cast<char*>(&color32), sizeof(uint32_t));
        
        // Convert color32 from RGBA to RGB565.
        uint16_t rgb565_color = rgba2rgb565(color32);
        
        // Write rgb565_color into rgb565 vector.
        for (size_t j = 0; j < sizeof(uint16_t); ++j) {
            rgb565.push_back(reinterpret_cast<uint8_t*>(&rgb565_color)[j]);
        }
    }
    
    return rgb565;
}

template <typename B>
std::vector<uint8_t> rgb565_to_rgba(const uint8_t* data, uint32_t w, uint32_t h) {
    size_t pixels = static_cast<size_t>(w * h);
    std::vector<uint8_t> rgba;
    rgba.reserve(pixels * 4);
    
    const uint16_t* data_reader = reinterpret_cast<const uint16_t*>(data);
    for (size_t i = 0; i < pixels; ++i) {
        uint16_t color16 = data_reader[i];
        uint32_t color32 = rgb5652rgba(color16);
        
        // Write color32 into rgba vector.
        for (size_t j = 0; j < sizeof(uint32_t); ++j) {
            rgba.push_back(reinterpret_cast<uint8_t*>(&color32)[j]);
        }
    }
    
    return rgba;
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
