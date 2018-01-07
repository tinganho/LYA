//
// Created by Tingan Ho on 2018-01-07.
//

#ifndef LYA_UTF8_H
#define LYA_UTF8_H


class Utf8Iterator {
public:
    Utf8Iterator(std::string::const_iterator it);
    Utf8Iterator(const Utf8Iterator& source);
    ~Utf8Iterator();
    Utf8Iterator& operator=(const Utf8Iterator& rhs);
private:
    std::string::const_iterator mStringIterator;
};

#endif //LYA_UTF8_H
