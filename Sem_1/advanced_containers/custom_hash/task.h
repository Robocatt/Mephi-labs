#include <unordered_map>
#include <vector>
#include <string>

struct SuperKey {
    std::string StrPart;
    int IntPart;
    float FloatPart;

    bool operator==(const SuperKey& rhs) const {
        return this->StrPart == rhs.StrPart && this->IntPart == rhs.IntPart && this->FloatPart == rhs.FloatPart;
    }
};
namespace std{

    template<>
    struct hash<SuperKey>{
        std::size_t operator()(const SuperKey& Key) const
        {
             return ((std::hash<std::string>()(Key.StrPart) ^ (std::hash<int>()(Key.IntPart) << 1)) >> 1)
               ^ (std::hash<float>()(Key.FloatPart) << 1);
        }

    };


}
/*
 * Напишите хэш-функцию, реализовав специализацию шаблона std::hash для типа SuperKey
 * Напишите функцию PopulateHashMap, которая добавляет в hashMap пары ключ-значение из данного вектора toAdd
 */
void PopulateHashMap(
    std::unordered_map<SuperKey, std::string>& hashMap,
    const std::vector<std::pair<SuperKey, std::string>>& toAdd
);

