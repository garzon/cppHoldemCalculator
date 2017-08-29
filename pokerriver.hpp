#ifndef POKERRIVER_HPP
#define POKERRIVER_HPP

#include <functional>
#include <memory>

#include "cards.hpp"
#include "seqgenerator.hpp"
#include "cardpool.hpp"

struct PokerRiver: public Cards {
    std::shared_ptr<CardPool> cp;

    PokerRiver(std::shared_ptr<CardPool> _cp, Cards cards=Cards()):
        Cards(cards), cp(_cp)
    {
        cp->pickout(cards);
    }

    PokerRiver(Cards cards):
        Cards(cards), cp(std::make_shared<CardPool>())
    {
        cp->pickout(cards);
    }

    inline bool hasCard(Card a, int showedCard=-1) const {
        if(cp->notAvailable(a)) return true;
        if(showedCard == -1) showedCard = cardNum;
        for(int i=0; i<showedCard; i++)
            if(cards[i] == a) return true;
        return false;
    }

    bool genRiver(std::function<bool(PokerRiver*)> callback, int maxDepth=5, Card st={Card::Suit::DIAMOND, 2}) {
        if(cardNum == maxDepth) {
            if(callback(this)) return true;
            return false;
        }
        for(Card a: SeqGenerator(st)) {
            if(hasCard(a)) continue;
            cards[cardNum++] = a;
            bool ret = genRiver(callback, maxDepth, ++a);
            cardNum--;
            if(ret) return true;
        }
        return false;
    }

    void genRandomRiver(std::function<bool(PokerRiver*)> callback, int times=100000, int maxDepth=5) {
        if(cardNum == maxDepth) {
            callback(this);
            return;
        }
        for(int i=0; i<times; i++) {
            cards[cardNum++] = cp->pick();
            genRandomRiver(callback, 1, maxDepth);
            cardNum--;
            cp->put_back();
        }
    }

    void push(Cards &&newCards) {
        for(int i=0; i<newCards.getCardNum(); i++)
            cards[cardNum++] = newCards.cards[i];
    }
};


#endif // POKERRIVER_HPP
