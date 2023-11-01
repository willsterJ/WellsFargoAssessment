#ifndef WELLSFARGO_COMPARATORFUNCTOR_H
#define WELLSFARGO_COMPARATORFUNCTOR_H

#include <vector>
#include <functional>
#include <algorithm>
#include <string>

struct MyStruct {
    int x1;
    int get_x2() const { return x2; }
    const std::string& get_x3() const { return x3; }
    MyStruct(int x1, int x2, std::string x3){
        this->x1 = x1;
        this->x2 = x2;
        this->x3 = x3;
    }
private:
    int x2;
    std::string x3;
};

template <typename ObjType>
class comparer_builder {
private:
    //typedef bool (*T)(const ObjType& a, const ObjType& b);
    using T = std::function<bool(const ObjType&, const ObjType&)>;
    std::vector<T> m_ComparatorVec;
public:
    // by function that handles member variable argument and returns reference of object self
    template <typename MemberType>
    inline comparer_builder by(MemberType ObjType::*mVar) {
        m_ComparatorVec.push_back([mVar](const ObjType& a, const ObjType& b) -> bool {
            return a.*mVar < b.*mVar;
        });
        return *this;
    }
    // by function that handles member functinos argument and returns reference of object self
    template <typename MemberType>
    inline comparer_builder by(MemberType (ObjType::*mFunc)() const) {
        m_ComparatorVec.push_back([mFunc](const ObjType& a, const ObjType& b) -> bool {
            return (a.*mFunc)() < (b.*mFunc)();
        });
        return *this;
    }
    // functor operator
    inline bool operator()(const ObjType& a, const ObjType& b) const {
        for (const auto& compareFunc : m_ComparatorVec) {
            if (compareFunc(a, b))
                return true;
            if (compareFunc(b, a))
                return false;
        }
        return false;
    }
};

void printMyStructVec(std::vector<MyStruct> vec);

#endif //WELLSFARGO_COMPARATORFUNCTOR_H
