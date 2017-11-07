#ifndef POKERRIVER_HPP
#define POKERRIVER_HPP

#include <functional>
#include <memory>

#include "cards.hpp"
#include "seqgenerator.hpp"
#include "cardpool.hpp"

struct PokerRiver: public Cards {
    std::shared_ptr<CardPool> card_pool;

    PokerRiver(std::shared_ptr<CardPool> _card_pool, Cards cards=Cards()):
        Cards(cards), card_pool(_card_pool)
    {
        card_pool->pickout(cards);
    }

    PokerRiver(Cards cards=Cards()):
        Cards(cards), card_pool(std::make_shared<CardPool>())
    {
        card_pool->pickout(cards);
    }

    inline bool hasCard(Card a, int showedCard=-1) const {
        if(card_pool->notAvailable(a)) return true;
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
            cards[cardNum++] = card_pool->pick();
            genRandomRiver(callback, 1, maxDepth);
            cardNum--;
            card_pool->put_back();
        }
    }

    void push(Cards &&newCards) {
        for(int i=0; i<newCards.getCardNum(); i++)
            cards[cardNum++] = newCards.cards[i];
    }
};


#endif // POKERRIVER_HPP
