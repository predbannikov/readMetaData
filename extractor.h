#ifndef EXTRACTOR_H
#define EXTRACTOR_H
#include <vector>
#include <string>

using vint_t = uint64_t;

class Extractor {
public:
    std::vector<char>::const_iterator begin;
    std::vector<char>::const_iterator end;
    virtual void extract(std::vector<char>::const_iterator &it, size_t length = 0) = 0;
    vint_t getVInteger(std::vector<char>::const_iterator &it);
};

class Name : public Extractor{
public:
    Name();
//    std::string data;
    vint_t length;
    void extract(std::vector<char>::const_iterator &it, size_t len = 0) override;

};

#endif // EXTRACTOR_H
