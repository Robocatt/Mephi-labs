#include<unordered_map>
#include<list>
#include<cstdlib>
template <typename K, typename V>
class LruCache {
private:
    std::list<K> ls;
    std::unordered_map<K, std::pair<V, typename std::list<K>::iterator>> mp;
    size_t size;
public:
    LruCache(size_t max_size) : size(max_size) {}

    void Put(const K& key, const V& value){
        auto pos = mp.find(key);
        if(pos == mp.end()){
            ls.push_front(key);
            mp[key] = {value, ls.begin()};
            if(mp.size() > size){
                mp.erase(ls.back());
                ls.pop_back();
            }
        }else{
            ls.erase((pos -> second).second );
            ls.push_front(key);
            mp[key] = {value, ls.begin()};
        }
    }

    bool Get(const K& key, V* value){
        auto pos = mp.find(key);
        if(pos == mp.end()){
            return false;
        }
        ls.erase((pos -> second).second );
        ls.push_front(key);
        mp[key] = {(pos -> second).first, ls.begin()};
        *value = (pos -> second).first;
        return true;
    }
};
