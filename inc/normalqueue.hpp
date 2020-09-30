#ifndef NORMALQUEUE_HPP
#define NORMALQUEUE_HPP

class NormalQueue8 {
public: 
    uint8_t arr[SIZE];
    uint8_t* head;
    uint8_t* tail;

    uint8_t QueSize();
    void push(uint8_t val);
    uint8_t pop();
    bool isEmpty();

private:
    static constexpr uint16_t SIZE = 128*4; //размер очереди
};


#endif //NORMALQUEUE_HPP