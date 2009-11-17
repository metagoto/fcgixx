#pragma once

#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <fcgixx/util/file_loader.hpp>


namespace runpac { namespace fcgixx { namespace tpl { namespace loader {

struct filesys
{

    std::string load(const std::string& id)
    {
        std::string filename(boost::str(boost::format(path_format.c_str()) % id));
        return boost::algorithm::replace_all_copy(util::file_loader::load(filename), "\r", "");
    }

    void set_format(const std::string& format)
    {
        path_format = format;
    }

    std::string path_format;

};


} } } } // ns


