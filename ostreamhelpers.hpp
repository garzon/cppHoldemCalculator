#ifndef OSTREAMHELPERS_HPP
#define OSTREAMHELPERS_HPP

#include <iostream>

#include "card.hpp"
#include "pokercombination.hpp"
#include "pokersuite.hpp"
#include "pokerriver.hpp"

static const char *typeName[] = {
    "unknown", "high", "pair", "two_pairs", "3_of_a_kind",
    "straight", "flush", "fullhouse", "4_of_a_kind", "straight_flush"
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

std::ostream & operator << (std::ostream &os, const PokerSuite &ps) {
    for(int i=0; i<ps.getCardNum(); i++)
        os << ps.cards[i] << " ";
    return os << std::endl;
}

std::ostream & operator << (std::ostream &os, const PokerRiver &pr) {
    for(int i=0; i<pr.getCardNum(); i++)
        os << pr.cards[i] << " ";
    return os << std::endl;
}

#endif // OSTREAMHELPERS_HPP
