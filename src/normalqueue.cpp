#include "normalqueue.hpp"

NormalQueue8* NormalQueue8::pThis=nullptr;

NormalQueue8::NormalQueue8() {
    pThis=this;
}

/*!< return current queue size >*/
uint16_t NormalQueue8::QueSize() {
    if(head >= tail) {
        return 1 + head - tail; // если голова больше хвоста возвращаем нормальный размер
    } else {
        return 1 + SIZE - (tail - head); // если наоборот возвращаем размер с учетом размера Массива.
    }
}

void NormalQueue8::push(uint8_t val) {
    /*!< очередь пуста >*/
    if(head == nullptr) {
        head = tail = &arr[0]; // указываем на первый элемент
        *(head) = val; // записываем элемент 
    } else {
        if(head == &arr[SIZE-1]) { //если последний элемент в массиве
            if(QueSize() == SIZE) { //если очередь заполнена
                return; // не записываем ничего 
            }
            head = &arr[0]; *(head) = val; //записываем элемент в начало массива 
        } else {
            if(QueSize() == SIZE) {//если очередь заполнена
                return;
            }
            head++; *(head) = val; // переходим к следующему элементу в массиве и перезаписываем его значением 
        }
    }
}

uint8_t NormalQueue8::pop() {
    if(head == nullptr) {return 0;} //надо проверять чтобы не вызывалась из пустой очереди
    if (head == tail) { // в очереди один элемент
        uint8_t x = *(head); head = nullptr;
        return x; //возвращаем последний элемент из очереди
    } else {
        if(tail == &arr[SIZE-1]) { //если хвост указывает на последний элемент
            uint8_t x = *(tail); tail=&arr[0]; // перемещаем хвост на начало массива
            return x; //возвращаем элемент из очереди
        }   else {
            uint8_t x = *(tail); tail++; //перемешаем хвост на следующий элемент
            return x; //возвращаем элемент из очереди
        }
    }
}

bool NormalQueue8::isEmpty() {
    if(head == nullptr) return true; //если пустая возвращаем единицу
    else return false;
}