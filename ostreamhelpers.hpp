#ifndef OSTREAMHELPERS_HPP
#define OSTREAMHELPERS_HPP

#include <iostream>

#include "card.hpp"
#include "pokercombination.hpp"
#include "pokerriver.hpp"

static const char *typeName[] = {
    "unknown", "high", "pair", "two_pairs", "one_kind_of_3",
    "straight", "flush", "fullhouse", "one_kind_of_4", "straight_flush"
};

std::ostream & operator << (std::ostream &os, const Card &c) {
    return os << c.suitToChr() << c.numToChr();
}

std::ostream & operator << (std::ostream &os, const PokerCombination &pc) {
    os << "Type: " << typeName[(int)pc.comb] << "(";
    char ret = numChars[RECOVER_FROM_REAL_NUM((pc.numOfParts/NUM_COUNT/NUM_COUNT/NUM_COUNT/NUM_COUNT) % NUM_COUNT)];
    os << ret;
    if(ret != ' ') ret = numChars[RECOVER_FROM_REAL_NUM((pc.numOfParts/NUM_COUNT/NUM_COUNT/NUM_COUNT) % NUM_COUNT)];
    if(ret != ' ') os << ", " << ret;
    if(ret != ' ') ret = numChars[RECOVER_FROM_REAL_NUM((pc.numOfParts/NUM_COUNT/NUM_COUNT) % NUM_COUNT)];
    if(ret != ' ') os << ", " << ret;
    if(ret != ' ') ret = numChars[RECOVER_FROM_REAL_NUM((pc.numOfParts/NUM_COUNT) % NUM_COUNT)];
    if(ret != ' ') os << ", " << ret;
    if(ret != ' ') ret = numChars[RECOVER_FROM_REAL_NUM((pc.numOfParts) % NUM_COUNT)];
    if(ret != ' ') os << ", " << ret;
    return os << ")." << std::endl;
}

std::ostream & operator << (std::ostream &os, const PokerRiver &pr) {
    for(int i=0; i<5; i++)
        os << pr.cards[i] << " ";
    return os << std::endl;
}

#endif // OSTREAMHELPERS_HPP
