#ifndef POKERSUITE_HPP
#define POKERSUITE_HPP

#include <memory>
#include <utility>

#include "cardpool.hpp"
#include "pokerriver.hpp"
#include "pokercombination.hpp"

class PokerSuite {
    char cardNum = 0;

    char suitCounter[5];
    char numCounter[15];

    void calcCommon() {
        memset(suitCounter, 0, sizeof(suitCounter));
        memset(numCounter, 0, sizeof(numCounter));
        for(int i=0; i<cardNum; i++) {
            suitCounter[(char)cards[i].type]++;
            numCounter[cards[i].num]++;
            if(cards[i].num == 1)
                numCounter[0]++;
        }
    }

public:
    std::shared_ptr<CardPool> cp;
    Card cards[7];

    PokerSuite(std::shared_ptr<CardPool> _cp=nullptr): cp(_cp) {
        if(cp.get() == nullptr) cp = std::make_shared<CardPool>();
        //randomize();
    }

    PokerSuite(const char *suiteStr, std::shared_ptr<CardPool> _cp=nullptr): cp(_cp) {
        // at least 2 cards in suite anytime
        cards[0] = Card(suiteStr);
        cards[1] = Card(suiteStr+2);
        cardNum = 2;
        bool doPickout = cp.get() != nullptr;
        if(doPickout) {
            cp->pickout(cards[0]);
            cp->pickout(cards[1]);
        }
        for(int i=4; suiteStr[i]; i+=2) {
            cards[i >> 1] = Card(suiteStr+i);
            if(doPickout) cp->pickout(cards[i >> 1]);
            cardNum++;
        }
    }

    PokerSuite(Card card1, Card card2, std::shared_ptr<CardPool> _cp=nullptr): cp(_cp) {
        cards[0] = card1;
        cards[1] = card2;
        cardNum = 2;
        if(cp.get() != nullptr) {
            cp->pickout(card1);
            cp->pickout(card2);
        }
    }

    void randomize() {
        assert(cp.get() != nullptr);
        cards[0] = cp->pick();
        cards[1] = cp->pick();
    }

    PokerCombination findCombination(const PokerRiver &pr) {
        for(int i=0; i<pr.numShowed; i++) {
            cards[cardNum+i] = pr.cards[i];
        }
        cardNum += pr.numShowed;
        calcCommon();

        int highest, flushType;
        if(highest = hasStraightFlush()) {
            cardNum -= pr.numShowed;
            return PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, highest);
        }
        if(highest = hasKindOfN(4)) {
            int second = 0;
            if(cardNum > 4) {
                second = hasKindOfN(3);
                if(!second) second = hasKindOfN(2);
                if(!second) second = hasKindOfN(1);
            }
            cardNum -= pr.numShowed;
            return PokerCombination(PokerCombination::Combination::KIND_OF_4, highest, second);
        }
        if(highest = hasKindOfN(3)) {
            int second;
            if((second = hasKindOfN(3, GEN_REAL_NUM(highest)-1)) || (second = hasKindOfN(2))) {
                cardNum -= pr.numShowed;
                return PokerCombination(PokerCombination::Combination::FULL_HOUSE, highest, second);
            }
        }
        if(flushType = hasFlush()) {
            int nums[5], curr = 0;
            bool ifExists[15];
            memset(ifExists, 0, sizeof(ifExists));
            for(int i=0; i<cardNum; i++)
                if((char)cards[i].type == flushType)
                    ifExists[cards[i].num] = true;
            for(int i=GEN_REAL_NUM(1); i>0 && curr != 5; i--) {
                int pNum = RECOVER_FROM_REAL_NUM(i);
                if(ifExists[pNum]) nums[curr++] = pNum;
            }
            cardNum -= pr.numShowed;
            return PokerCombination(PokerCombination::Combination::FLUSH, nums[0], nums[1], nums[2], nums[3], nums[4]);
        }
        if(highest = hasStraight()) {
            cardNum -= pr.numShowed;
            return PokerCombination(PokerCombination::Combination::STRAIGHT, highest);
        }
        if(highest = hasKindOfN(3)) {
            int second = 0, third = 0;
            if(cardNum >= 4)
                second = hasKindOfN(1);
            if(cardNum >= 5)
                third = hasKindOfN(1, GEN_REAL_NUM(second)-1);
            cardNum -= pr.numShowed;
            return PokerCombination(PokerCombination::Combination::KIND_OF_3, highest, second, third);
        }
        if(highest = hasKindOfN(2)) {
            int second = 0, third = 0, forth = 0;
            if(cardNum >= 4 && (second = hasKindOfN(2, GEN_REAL_NUM(highest)-1))) {
                // two pairs
                if(cardNum >= 5) {
                    if(!(third = hasKindOfN(2, GEN_REAL_NUM(second)-1))) third = hasKindOfN(1);
                }
                cardNum -= pr.numShowed;
                return PokerCombination(PokerCombination::Combination::TWO_PAIRS, highest, second, third);
            } else {
                // one pair
                if(cardNum >= 3) second = hasKindOfN(1);
                if(cardNum >= 4) third = hasKindOfN(1, GEN_REAL_NUM(second)-1);
                if(cardNum >= 5) forth = hasKindOfN(1, GEN_REAL_NUM(third)-1);
                cardNum -= pr.numShowed;
                return PokerCombination(PokerCombination::Combination::PAIR, highest, second, third, forth);
            }
        }
        int second = 0, third = 0, forth = 0, fifth = 0;
        if(cardNum >= 1) highest = hasKindOfN(1);
        if(cardNum >= 2) second = hasKindOfN(1, GEN_REAL_NUM(highest)-1);
        if(cardNum >= 3) third = hasKindOfN(1, GEN_REAL_NUM(second)-1);
        if(cardNum >= 4) forth = hasKindOfN(1, GEN_REAL_NUM(third)-1);
        if(cardNum >= 5) fifth = hasKindOfN(1, GEN_REAL_NUM(forth)-1);
        cardNum -= pr.numShowed;
        return PokerCombination(PokerCombination::Combination::HIGH, highest, second, third, forth, fifth);
    }

    inline int hasKindOfN(int num, int startFrom=GEN_REAL_NUM(1)) const {
        for(int i=startFrom; i>0; i--) {
            int pNum = RECOVER_FROM_REAL_NUM(i);
            if(numCounter[pNum] == num) return pNum;
        }
        return 0;
    }

    inline int hasFlush() const {
        return suitCounter[4] >= 5 ? 4 : (
                suitCounter[1] >= 5 ? 1 : (
                 suitCounter[2] >= 5 ? 2 : (
                   suitCounter[3] >= 5 ? 3 : 0)));
    }

    static int checkStraight(const char *numCount) {
        for(int i=GEN_REAL_NUM(1); i>=GEN_REAL_NUM(5); i--) {
            int pNum = RECOVER_FROM_REAL_NUM(i);
            if(!numCount[pNum]) continue;
            int step = 0;
            while(step != 5 && i >= 0 && numCount[RECOVER_FROM_REAL_NUM(i)]) {
                step++;
                i--;
            }
            if(step == 5) return RECOVER_FROM_REAL_NUM(i + 5);
        }
        return 0;
    }

    int hasStraight() const {
        return checkStraight(numCounter);
    }

    int hasStraightFlush() const {
        int flushType, highest;
        if(!(flushType = hasFlush())) return false;
        if(!(highest = hasStraight())) return false;

        char ifExists[15];
        memset(ifExists, 0, sizeof(ifExists));
        for(int i=0; i<cardNum; i++) {
            if((char)cards[i].type != flushType) continue;
            ifExists[cards[i].num] += 1;
            if(cards[i].num == 1) ifExists[0] += 1;
        }

        return checkStraight(ifExists);
    }
};

#endif // POKERSUITE_HPP
