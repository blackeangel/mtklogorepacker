#include "main.h"

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <unpack/pack> <file/folder> <folder/file>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string logo_file;
    std::string dir;

    if (mode == "unpack") {
       logo_file = argv[2];
        dir = argv[3];
        unpack_logo(logo_file, dir);
    } else if (mode == "pack") {
       logo_file = argv[3];
        dir = argv[2];
        pack_logo(dir, logo_file);
    } else {
        std::cerr << "Error: Invalid mode. Use 'unpack' or 'pack'." << std::endl;
        return 1;
    }

    return 0;
}