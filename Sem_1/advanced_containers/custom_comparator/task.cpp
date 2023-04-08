#include "task.h"

bool operator <(const SuperKey& lhs, const SuperKey& rhs) {
    if(lhs.StrPart != rhs.StrPart){
        return (lhs.StrPart < rhs.StrPart ? true : false);
    }
    if(lhs.IntPart != rhs.IntPart){
        return (lhs.IntPart < rhs.IntPart ? true : false);
    }
    if(lhs.FloatPart != rhs.FloatPart){
        return (lhs.FloatPart < rhs.FloatPart ? true : false);
    }
    return false;
}

void PopulateMap(
    std::map<SuperKey, std::string>& map,
    const std::vector<std::pair<SuperKey, std::string>>& toAdd
) {
    for (auto i : toAdd){
        map[i.first] = i.second;
    }
    

}
