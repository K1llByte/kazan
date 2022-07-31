#include "utils/string.hpp"
//#include <boost/algorithm/string.hpp>

namespace kzn {
    std::vector<std::string> split(const std::string& cmd, const char separator)
    {
        size_t num_sparator = 0;
        for(size_t i = 0 ; i < cmd.length(); ++i)
            if(cmd[i] != separator && (i == 0 || cmd[i-1] == separator))
            {
                ++num_sparator;
            }

        std::vector<std::string> res(num_sparator);
        size_t idx = 0;
        size_t begin = 0;
        size_t end = 0;
        for(size_t i = 0 ; i < cmd.length() ; ++i)
        {
            if(cmd[i] == separator || cmd[i] == '\n')
            {
                if(end != begin)
                { 
                    std::string_view tmp(cmd.c_str(), end);
                    tmp.remove_prefix(begin);
                    
                    res[idx++] = std::string(tmp);
                }
                begin = ++end;
            }
            else
            {
                ++end;
            }
        }
        std::string_view tmp(cmd.c_str(),end);
        tmp.remove_prefix(begin);

        if(!tmp.empty())
            res[idx] = std::string(tmp);

        return res;
    }

    //std::vector<std::string> kzn::split(const std::string& cmd, const char separator)
    //{
    //	std::vector<std::string> result;
    //	boost::split(result, cmd, boost::is_any_of(" "));
    //	return result;
    //}
}