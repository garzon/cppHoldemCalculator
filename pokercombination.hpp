#ifndef POKERCOMBINATION_HPP
#define POKERCOMBINATION_HPP

struct PokerCombination {
    enum class Combination: char {
        STRAIGHT_FLUSH = 9,
        KIND_OF_4 = 8,
        FULL_HOUSE = 7,
        FLUSH = 6,
        STRAIGHT = 5,
        KIND_OF_3 = 4,
        TWO_PAIRS = 3,
        PAIR = 2,
        HIGH = 1,
        UNKNOWN = 0
    } comb;
    int numOfParts;

    PokerCombination(Combination _comb, int mostSig, int second = 0, int third = 0, int fourth = 0, int fifth = 0) {
        comb = _comb;
        numOfParts =
            (char)(comb) * NUM_COUNT * NUM_COUNT * NUM_COUNT * NUM_COUNT * NUM_COUNT +
            GEN_REAL_NUM(mostSig) * NUM_COUNT * NUM_COUNT * NUM_COUNT * NUM_COUNT +
            GEN_REAL_NUM(second) * NUM_COUNT * NUM_COUNT * NUM_COUNT +
            GEN_REAL_NUM(third) * NUM_COUNT * NUM_COUNT +
            GEN_REAL_NUM(fourth) * NUM_COUNT + GEN_REAL_NUM(fifth);
    }

    bool operator < (const PokerCombination &o) const {
        //if(comb > o.comb) return false;
        //if(comb < o.comb) return true;
        return numOfParts < o.numOfParts;
    }

    bool operator == (const PokerCombination &o) const {
        return numOfParts == o.numOfParts;
    }
};


#endif // POKERCOMBINATION_HPP
