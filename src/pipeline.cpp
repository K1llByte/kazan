#include "pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace kzn
{

std::vector<char> Pipeline::read_file(const std::string& file_path)
{
    // TODO: validate file size limit

    // Open file at the end to compute size
    std::ifstream file{file_path, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file: " + file_path);
    }

    const size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

void Pipeline::create_pipeline(const std::string& vert_path, const std::string& frag_path)
{
    std::vector<char> vert_bin = read_file(vert_path);
    std::vector<char> frag_bin = read_file(frag_path);

    std::cout << vert_bin.size() << '\n';
    std::cout << frag_bin.size() << '\n';
}

Pipeline::Pipeline(const std::string& vert_path, const std::string& frag_path)
{
    create_pipeline(vert_path, frag_path);
}

}