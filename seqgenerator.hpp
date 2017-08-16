#ifndef SEQGENERATOR_HPP
#define SEQGENERATOR_HPP

#include "card.hpp"

static const Card ANY_CARD;

class SeqGenerator {
    Card e, st;
public:
    typedef Card::Suit Suit;

    SeqGenerator(): st(Suit::DIAMOND, 2), e(ANY_CARD) {}
    SeqGenerator(Card _begin, Card _end=ANY_CARD): st(_begin), e(_end) {}

    class iterator {
        Card p;
    public:
        iterator(Card _p) : p(_p) {}

        iterator operator ++() {
            ++p;
            return *this;
        }

        bool operator !=(const iterator &i) const {
            return !(p == i.p);
        }

        inline Card operator *() const {
            return p;
        }
    };
    SeqGenerator::iterator begin() const {
        return SeqGenerator::iterator(st);
    }
    SeqGenerator::iterator end() const {
        return e;
    }
};

#endif // SEQGENERATOR_HPP
