#ifndef CARDS_HPP
#define CARDS_HPP

#include "card.hpp"

class Cards {
protected:
    char cardNum = 0;
public:
    Card cards[7];

    inline int getCardNum() const {
        return cardNum;
    }

    Cards() {}
    Cards(const char *cardsStr) {
        for(int i=0; cardsStr[i]; i+=2) {
            cards[cardNum++] = Card(cardsStr+i);
        }
    }

    const Card *begin() const {
        return cards;
    }

    const Card *end() const {
        return &(cards[cardNum]);
    }
};

inline Cards operator "" _cards(const char *cardName) {
    return Cards(cardName);
}

#endif // CARDS_HPP
