#include <string>
#include <iterator>

using namespace std;

string normalize(string_view path) {
    std::string str;
    for (size_t i = 0; i < path.size(); ++i){
        str += path[i];
    }

    size_t i = 0;
    if ( (str.size() == 1 && str[i] == '.') || (str.size() == 2 && str == "..") )
    {
        return str;
    }

    if(str.size() > 2){
        for (size_t i = 0; i < str.size() - 2; ++i)
        {
            if (str[i] == '/' && str[i + 1] == '.' && str[i + 2] == '.' && int(str.substr(0, i).rfind('/')) != -1)
            {
                str.erase(str.substr(0, i).rfind('/'), i + 3 - str.substr(0, i).rfind('/'));
            }
        }
    }

    for (size_t i = 0; i < str.size() - 1; ++i)
    {
        if (str[i] == '/' && str[i + 1] == '.' && str[i + 2] != '.'){
            str.erase(i + 1, 1);
        }
    }

    if (str[str.size() - 1] == '/' && str.size() >= 2){
        str.erase(str.size() - 1);
    }

    for (size_t i = 0; i < 2; i++)
    {
        if (str[i] == '/' && str[i + 1] == '.' && str[i + 2]=='.'){
            str.erase(1,3);
            i--;
        }
    }

    if (str.size() > 2)
    {
        for (size_t i = 0; i < str.size() - 1; ++i)
        {
            if (str[i] == '/' && str[i + 1] == '/'){
                str.erase(i, 1);
                i--;
            }
        }
    }
    return str;




}

