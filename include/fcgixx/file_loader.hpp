#pragma once

#include <string>
#include <fstream>

namespace runpac { namespace fcgixx {

struct file_loader
{

    template<typename T = std::string>
    static T load(const char* filename)
    {
        typedef typename T::value_type value_type;
        std::ifstream file(filename);
        if (file.is_open()) {
            return T( (std::istreambuf_iterator<value_type>(file)),
                      (std::istreambuf_iterator<value_type>()) );
        }
        return T();
    }


    template<typename T = std::string>
    static T load(const T& filename)
    {
        return load(filename.c_str());
    }

};


} } // ns


