#ifndef CARDPOOL_HPP
#define CARDPOOL_HPP

#include <random>
#include "card.hpp"

std::random_device rd;
std::mt19937 mt;

class CardPool {
    int realUpperBound;
    Card cards[52];

    void swap_back(Card *p) {
        Card tmp = *p;
        *p = cards[realUpperBound];
        cards[realUpperBound] = tmp;
        realUpperBound--;
    }

public:
    CardPool(): realUpperBound(51) {
        int ind = 0;
        for(Card a: SeqGenerator()) {
            cards[ind++] = a;
        }
        assert(ind == realUpperBound + 1);
    }

    void pickout(Card card) {
        Card *p = std::find(cards, cards + realUpperBound + 1, card);
        if(p == cards + realUpperBound + 1) throw "card not found";
        swap_back(p);
    }

    void put_back(int num=1) {
        realUpperBound += num;
    }

    void recover() {
        realUpperBound = 51;
    }

    Card pick() {
        int rndNum = std::uniform_int_distribution<>(0, realUpperBound)(mt);
        auto p = &(cards[rndNum]);
        Card ret = *p;
        swap_back(p);
        return ret;
    }
};

#endif // CARDPOOL_HPP
