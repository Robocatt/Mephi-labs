#pragma once

#include <string>
#include <vector>
#include <openssl/sha.h>
#include <fstream>
#include <variant>
#include <list>
#include <map>
#include <sstream>


//for debug
#include <iostream>

struct TorrentFile {
    std::string announce; // 	The URL of the tracker for the torrent.
    std::string comment; // Any user comment for the torrent. Optional.
    std::vector<std::string> pieceHashes; // A (byte) string consisting of the concatenation of all 20-byte SHA1 hash values, one per piece.
    size_t pieceLength; // The number of bytes in each piece. Files in a torrents are divided into "pieces" with a specific length.
    size_t length; // The length of the file, in bytes.
    std::string name; // The proposed filename of the file.
    std::string infoHash; // The “infohash” is the SHA1 Hash over the all "info" section
};

/*
 * Функция парсит .torrent файл и загружает информацию из него в структуру `TorrentFile`. Как устроен .torrent файл, можно
 * почитать в открытых источниках (например http://www.bittorrent.org/beps/bep_0003.html).
 * После парсинга файла нужно также заполнить поле `infoHash`, которое не хранится в файле в явном виде и должно быть
 * вычислено. Алгоритм вычисления этого поля можно найти в открытых источника, как правило, там же,
 * где описание формата .torrent файлов.
 * Данные из файла и infoHash будут использованы для запроса пиров у торрент-трекера. Если структура `TorrentFile`
 * была заполнена правильно, то трекер найдет нужную раздачу в своей базе и ответит списком пиров. Если данные неверны,
 * то сервер ответит ошибкой.
 */


//pair <T, size_T> - first param is for data, which we get out of a string, second is
// for how far should we continue in our basic string (data)


std::pair<std::string, size_t> ParseString(const std::string& str){
    size_t num_size_in_string = str.substr(0, str.find(':')).size();
    long long len = stoll(str.substr(0, str.find(':')));

    if(str.size() < len){
        std::cout << "length of a string is less than a len\n";
    }

    std::string cur_str = str.substr(num_size_in_string + 1, len);
    return {cur_str, len + 1 + num_size_in_string}; 
}

std::pair<size_t, size_t> ParseInt(const std::string& str){
    size_t end = str.find('e');
    if(end == std::string::npos){
        std::cout << "error end of int does not found\n ";
    }
    std::string value = str.substr(1, end - 1);
    std::stringstream sstream(value);
    size_t value_size_t;
    sstream >> value_size_t;
    return {value_size_t, end + 1};
}

std::pair<std::list<std::variant<std::string, size_t>>, size_t> ParseList(const std::string& str){
    int cur_pos = 1;//do not forget to add 1 in cut of basic string 
    std::list<std::variant<std::string, size_t>> lst;
    while(str[cur_pos] != 'e'){
        std::pair<std::variant<std::string, size_t>, size_t> list_item;
        if(isdigit(str[cur_pos])){
            list_item = ParseString(str.substr(1));
        }else{
            list_item = ParseInt(str.substr(1));
        }
        lst.push_back(list_item.first);
        cur_pos += list_item.second;
    }
    return {lst, cur_pos + 1}; //after 'e' 
}

std::pair<std::map<std::string, std::variant<std::string, size_t>>, size_t> ParseDict(const std::string& str){
    int cur_pos = 1;
    std::map<std::string, std::variant<std::string, size_t>> mp;
    while(str[cur_pos] != 'e'){
        std::pair<std::string, size_t> key = ParseString(str.substr(cur_pos));
        //value can be int or string or list or whatever
        std::pair<std::variant<std::string, size_t>, size_t> dict_item;
        if(isdigit(str[cur_pos + key.second])){
            dict_item = ParseString(str.substr(cur_pos + key.second));
        }else{
            dict_item = ParseInt(str.substr(cur_pos + key.second));
        }
        mp[key.first] = dict_item.first;
        cur_pos += key.second + dict_item.second;
    }
    return{mp, cur_pos + 1}; //after 'e'
}


TorrentFile LoadTorrentFile(const std::string& filename) {

    std::ifstream file(filename);
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    std::string data(size, ' ');
    file.seekg(0);

    file.read(&data[0], size); 


    int cur_pos = 1;
    
    // std::cout << data;
    // std::cout << data.size() << std::endl;
    
    TorrentFile TFile;
    
    while(cur_pos != data.size() - 1){
        auto global_key = ParseString(data.substr(cur_pos));
        cur_pos += global_key.second;
        if(global_key.first == "announce"){
            auto res = ParseString(data.substr(cur_pos));
            TFile.announce = res.first;
            cur_pos += res.second;
        }else if(global_key.first == "info"){
            std::cout << "in info\n";
            auto res = ParseDict(data.substr(cur_pos));

            unsigned char SHA_info[20];
            std::string full_info_string = data.substr(cur_pos, res.second);
            const unsigned char* to_encode = reinterpret_cast<const unsigned char *>(full_info_string.c_str());
            SHA1(to_encode, full_info_string.size(), SHA_info);

            std::string s;
            s.resize(20);
            for(int i = 0; i < 20; ++i){
                s[i] = SHA_info[i];
            }
            TFile.infoHash = s;

            //6d4795dee7aeb88e03e5336ca7c9fcfa1e206d
            // std::string string_formated;
            // for(auto i : s){
            //     string_formated.push_back(char(i));
            // }
            // TFile.infoHash = string_formated;
            // std::cout << string_formated;

            
            cur_pos += res.second;
            for( auto [key, val] : res.first ){
                if(key == "name"){
                    TFile.name = std::get<std::string>(val);
                }else if(key == "length"){
                    TFile.length = std::get<size_t>(val);
                }else if(key == "piece length"){
                    TFile.pieceLength = std::get<size_t>(val);
                }else if(key == "pieces"){
                    std::string str_pieces = std::get<std::string>(val);
                    int cur_pos = 0;
                    while(cur_pos < str_pieces.size()){
                        TFile.pieceHashes.push_back(str_pieces.substr(cur_pos, std::min(size_t(20), str_pieces.size() - cur_pos)));
                        cur_pos += 20;
                    }
                }
            }
            std::cout << "after info\n";
        }else if(global_key.first == "announce-list"){
            auto res = ParseList(data.substr(cur_pos));
            cur_pos += res.second;
            std::cout << "announce-list does not supported in this task\n"; 
        }else if(global_key.first == "creation date"){
            auto res = ParseInt(data.substr(cur_pos));
            cur_pos += res.second;
            std::cout << "creation date does not supported in this task\n";
        }else if(global_key.first == "comment"){
            auto res = ParseString(data.substr(cur_pos));
            TFile.comment = res.first;
            cur_pos += res.second;
        }else if(global_key.first == "created by"){
            auto res = ParseString(data.substr(cur_pos));
            cur_pos += res.second;
            std::cout << "created by does not supported in this task\n";
        }else if(global_key.first == "private"){
            auto res = ParseInt(data.substr(cur_pos));
            cur_pos += res.second;
            int private_value = res.first;//maybe i'll need it 
        }else if(global_key.first == "url-list"){
            auto res = ParseList(data.substr(cur_pos));
            cur_pos += res.second;
            std::cout << "url-list does not supported in this task\n";
        }else{
            std::cout << "strange parse in global dict\n";
        }
    }
    std::cout << TFile.announce << std::endl;
    std::cout << TFile.comment << std::endl;
    std::cout << TFile.pieceLength << std::endl;
    std::cout << TFile.length << std::endl;
    std::cout << TFile.name << std::endl;


    return TFile;

}

