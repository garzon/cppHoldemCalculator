#ifndef POKERRIVER_HPP
#define POKERRIVER_HPP

#include <functional>
#include <memory>

#include "seqgenerator.hpp"
#include "cardpool.hpp"

struct PokerRiver {
    Card cards[5];
    int numShowed = 0;

    inline bool hasCard(Card a, int showedCard=-1) {
        if(showedCard == -1) showedCard = numShowed;
        for(int i=0; i<showedCard; i++)
            if(cards[i] == a) return true;
        return false;
    }

    bool genRiver(std::function<bool(PokerRiver *)> f, int maxDepth=5, Card st={Card::Suit::DIAMOND, 2}) {
        if(numShowed == maxDepth) {
            if(f(this)) return true;
            return false;
        }
        for(Card a: SeqGenerator(st)) {
            if(hasCard(a)) continue;
            cards[numShowed++] = a;
            bool ret = genRiver(f, maxDepth, ++a);
            numShowed--;
            if(ret) return true;
        }
        return false;
    }

    void genRandomRiver(std::function<bool(PokerRiver *)> f, std::shared_ptr<CardPool> cp, int times=100000, int maxDepth=5) {
        if(numShowed == maxDepth) {
            f(this);
            return;
        }
        for(int i=0; i<times; i++) {
            cards[numShowed++] = cp->pick();
            genRandomRiver(f, cp, 1, maxDepth);
            numShowed--;
            cp->put_back();
        }
    }
};


#endif // POKERRIVER_HPP
