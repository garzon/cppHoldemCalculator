#ifndef POKERRIVER_HPP
#define POKERRIVER_HPP

#include <functional>
#include <memory>

#include "cards.hpp"
#include "seqgenerator.hpp"
#include "cardpool.hpp"


struct PokerRiver: public Cards {

    inline bool hasCard(Card a, int showedCard=-1) const {
        if(showedCard == -1) showedCard = cardNum;
        for(int i=0; i<showedCard; i++)
            if(cards[i] == a) return true;
        return false;
    }

    bool genRiver(std::function<bool(PokerRiver *)> f, int maxDepth=5, Card st={Card::Suit::DIAMOND, 2}) {
        if(cardNum == maxDepth) {
            if(f(this)) return true;
            return false;
        }
        for(Card a: SeqGenerator(st)) {
            if(hasCard(a)) continue;
            cards[cardNum++] = a;
            bool ret = genRiver(f, maxDepth, ++a);
            cardNum--;
            if(ret) return true;
        }
        return false;
    }

    void genRandomRiver(std::function<bool(PokerRiver *)> f, std::shared_ptr<CardPool> cp, int times=100000, int maxDepth=5) {
        if(cardNum == maxDepth) {
            f(this);
            return;
        }
        for(int i=0; i<times; i++) {
            cards[cardNum++] = cp->pick();
            genRandomRiver(f, cp, 1, maxDepth);
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
