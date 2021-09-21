#ifndef EXTRACTOR_H
#define EXTRACTOR_H
#include <vector>
#include <string>

#define EMPTY_SPACE_LEFT                5           ////просто для выравнивания при печати
#define EMPTY_SPACE_AFTER_LEFT          45
#define EMPTY_SPACE_RIGHT_NUMBER        7
using vint_t = uint64_t;

#include <vector>

/* Класс для View объектов, получает уведомления если необходимо перерисовать объект*/
//class Observer
//{
//public:
//    virtual void getString() = 0;
//};

/* Класс для модели, где будут храниться все данные и алгоритмы */
//class Observable
//{
//public:
//    void addObserver(Observer* observer)
//    {
//        _observers.push_back(observer);
//    }
//    void notifyUpdate()
//    {
//        int size = _observers.size();
//        for (int i = 0; i < size; i++)
//        {
//            _observers[i]->update();
//        }
//    }
//private:
//    std::vector<Observer*> _observers;
//};



struct Extractor {
    enum TYPE_VARIABLE {TYPE_STRING, TYPE_VINT } type;
public:
    vint_t vtmp;
    std::string stmp;
    std::vector<char>::const_iterator begin;
    std::vector<char>::const_iterator end;
    std::vector<char>::const_iterator itmp;
    virtual void extract(std::vector<char>::const_iterator &it, size_t length = 0) = 0;
    void fillString(std::string &str, size_t length);
    vint_t getVInteger(std::vector<char>::const_iterator &it);
};

struct SizeData : public Extractor{
    SizeData() { type = TYPE_VINT; }
    uint64_t get();
    vint_t size;
    void extract(std::vector<char>::const_iterator &it, size_t len = 0) override;
};

struct Name : public Extractor{
    Name() { type = TYPE_STRING; }
    vint_t length;
    void extract(std::vector<char>::const_iterator &it, size_t len = 0) override;
//    std::string getString();
    void setString(std::string &str);

};

#endif // EXTRACTOR_H
