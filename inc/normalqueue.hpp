#ifndef NORMALQUEUE_HPP
#define NORMALQUEUE_HPP

#include "stdint.h"

class NormalQueue8 {
    static constexpr uint16_t SIZE = 128*4; //размер очереди
public: 
    NormalQueue8();
    uint8_t arr[SIZE];
    uint8_t* head{nullptr};
    uint8_t* tail{nullptr};

    uint16_t QueSize();
    void push(uint8_t val);
    uint8_t pop();
    bool isEmpty();
    static NormalQueue8* pThis;
};


#endif //NORMALQUEUE_HPP