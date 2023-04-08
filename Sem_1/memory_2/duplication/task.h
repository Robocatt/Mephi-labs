#include <vector>
#include <string>
#include <memory>



const std::vector<std::shared_ptr<std::string>> DeDuplicate(const std::vector<std::unique_ptr<std::string>> &in){

    std::vector<std::shared_ptr<std::string>> out;

    out.push_back(std::make_shared<std::string>(*(in[0].get())));

    std::vector<std::string*> arr_ptrs;

    for (size_t i = 0; i < in.size(); ++i){
        arr_ptrs.push_back(in[i].get());
    }
    
    for (size_t i = 1; i < in.size(); ++i){
        int j = i - 1;
        while ((j >= 0) && (*arr_ptrs[i] != *arr_ptrs[j])){
            j--;
        }
        if (j != -1){
            out.push_back(out[j]);
        }
        else{
            out.push_back(std::make_shared<std::string>(*arr_ptrs[i]));
        }
    }
    return out;
}

const std::vector<std::unique_ptr<std::string>> Duplicate(const std::vector<std::shared_ptr<std::string>> in){
    std::vector<std::unique_ptr<std::string>> out;
    for (size_t i = 0; i < in.size(); ++i){
        out.push_back(std::make_unique<std::string>(*in[i]));
    }
    return out;
}