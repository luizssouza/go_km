#pragma once

namespace helpers::file
{

bool file_exist( const std::string& directory )
{
    return std::ifstream{ directory.c_str() }.good( );
}

bool is_directory( const std::string& directory )
{
    return directory.find( ':' ) != std::string::npos && directory.find( '\\' ) != std::string::npos;
}

} // helpers::file