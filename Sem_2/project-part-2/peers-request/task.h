#pragma once

#include "peer.h"
#include "torrent_file.h"
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <fstream>
#include <variant>
#include <list>
#include <map>
#include <sstream>
#include <cpr/cpr.h>
#include <iostream>

std::pair<std::map<std::string, std::variant<std::string, size_t, std::list<std::variant<std::string, size_t >>>>, size_t> ParseDict(const std::string& str);
 
class TorrentTracker {
public:
    /*
     * url - адрес трекера, берется из поля announce в .torrent-файле
     */
    TorrentTracker(const std::string& url) : url_(url) {}

    /*
     * Получить список пиров у трекера и сохранить его для дальнейшей работы.
     * Запрос пиров происходит посредством HTTP GET запроса, данные передаются в формате bencode.
     * Такой же формат использовался в .torrent файле.
     * Подсказка: посмотрите, что было написано в main.cpp в домашнем задании torrent-file
     *
     * tf: структура с разобранными данными из .torrent файла из предыдущего домашнего задания.
     * peerId: id, под которым представляется наш клиент.
     * port: порт, на котором наш клиент будет слушать входящие соединения (пока что мы не слушаем и на этот порт никто
     *  не сможет подключиться).
     */
    void UpdatePeers(const TorrentFile& tf, std::string peerId, int port){
        cpr::Response res = cpr::Get(
            cpr::Url{url_},
            cpr::Parameters {
                    {"info_hash", tf.infoHash},
                    {"peer_id", peerId},
                    {"port", std::to_string(port)},
                    {"uploaded", std::to_string(0)},
                    {"downloaded", std::to_string(0)},
                    {"left", std::to_string(tf.length)},
                    {"compact", std::to_string(1)}
            },
            cpr::Timeout{20000}
        );

        // if (res.status_code == 200) {
        //     if (res.text.find("failure reason") == std::string::npos) {
        //         std::cout << "Successfully requested peers from " << tf.announce << std::endl;
        //     } else {
        //         std::cerr << "Server responded '" << res.text << "'" << std::endl;
        //     }
        // }
        // std::ofstream test("out.txt");
        // test << res.text << std::endl;
        // std::cout << res.text << std::endl;

        auto dict_response = ParseDict(res.text); // Parse response
        auto map_from_response = dict_response.first; // get data out of pair 
        std::string peers = std::get<std::string>(map_from_response["peers"]); // get raw peers string
        int peers_position = res.text.find("peers");    // shortcuts for a correct determination of number of peers
        int peers_colon_position = res.text.find(":", peers_position);
        int len_of_peers = std::stoi(res.text.substr(peers_position + 5, peers_colon_position - peers_position - 5 ));

        // loop over all peers
        for(int k = 0; k < len_of_peers; k += 6){
            long long id = 0;
            std::string id_str;

            // Convert chars to a 4 byte hex
            for(int i = 0; i < 4; ++i){
                unsigned long long x = (unsigned char)(peers[i + k]);
                id += (x << (24 - 8 * i));
            }

            //convert each byte to it's corresponding digit
            id_str += std::to_string((id & 0xFF000000) >> 24);
            id_str += ".";
            id_str += std::to_string((id & 0x00FF0000) >> 16);
            id_str += ".";
            id_str += std::to_string((id & 0x0000FF00) >> 8);
            id_str += ".";
            id_str += std::to_string((id & 0x000000FF));
               
            // std::cout << id_str << std::endl << std::endl;

            // convert port just to hex
            int port_ = 0;
            for(int i = 4; i < 6; ++i){
                unsigned long long x = (unsigned char)(peers[i + k]);
                port_ += (x << (8 - 8 * (i - 4)));
            }

            // construct Peer from data provided
            Peer current_peer;
            current_peer.ip = id_str;
            current_peer.port = port_;
            peers_.push_back(current_peer);
        }
        

    }

    /*
     * Отдает полученный ранее список пиров
     */
    const std::vector<Peer>& GetPeers() const{
        return peers_;
    }

private:
    std::string url_;
    std::vector<Peer> peers_;
};



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

std::pair<std::map<std::string, std::variant<std::string, size_t, std::list<std::variant<std::string, size_t >>>>, size_t> ParseDict(const std::string& str){
    int cur_pos = 1;
    std::map<std::string, std::variant<std::string, size_t, std::list<std::variant<std::string, size_t >>>> mp;
    while(str[cur_pos] != 'e'){
        std::pair<std::string, size_t> key = ParseString(str.substr(cur_pos));
        //value can be int or string or list or whatever
        std::pair<std::variant<std::string, size_t, std::list<std::variant<std::string, size_t>>>, size_t> dict_item;
        if(isdigit(str[cur_pos + key.second])){
            dict_item = ParseString(str.substr(cur_pos + key.second));
        }else if(str[cur_pos + key.second] == 'l'){
            dict_item = ParseList(str.substr(cur_pos + key.second));
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
            // std::cout << "in info\n";
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
            // std::cout << "after info\n";
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
    // std::cout << TFile.announce << std::endl;
    // std::cout << TFile.comment << std::endl;
    // std::cout << TFile.pieceLength << std::endl;
    // std::cout << TFile.length << std::endl;
    // std::cout << TFile.name << std::endl;


    return TFile;

}
