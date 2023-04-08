#include "task.h"
bool is_palindrome(const std::string& s) {
    int l = -1;
    int r = s.size();
    if(s.size() <= 1){
        return true;
    }
    while(l < r){
        for(int i = l + 1; i <= r; ++i){
            if(isalpha(s[i])){
                l = i;
                break;
            }
        }
        if(l == -1){
            return true;
        }
        for(int i = r - 1; i >= l; --i){
            if(isalpha(s[i])){
                r = i;
                break;
            }
        }
        if(tolower(s[l]) != tolower(s[r])){
            return false;
        }

    }
    return true;
}





