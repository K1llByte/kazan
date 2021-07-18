#ifndef PIPELINE
#define PIPELINE

#include <vector>
#include <string>

namespace kzn
{

class Pipeline
{
    private:
    static std::vector<char> read_file(const std::string& file_path);
    void create_pipeline(const std::string& vert_path, const std::string& frag_path);
    
    public:
    Pipeline(const std::string& vert_path, const std::string& frag_path);
};

}

#endif // PIPELINE