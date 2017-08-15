#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <random>
#include <utility>
#include <memory>
#include <functional>

using namespace std;

struct Card {
    enum class Suit: char { DIAMOND = 1, CLUB = 2, HEART = 3, SPADE = 4, ANY = 0 } type;
    char num;

    Card(): num(0), type(Suit::ANY) {}
    Card(Suit _type, char _num) : type(_type), num(_num) {}

#define GEN_REAL_NUM(x) (((x)+11)%13)
// A, 2, 3 .. Q, K -> 12, 0, 1 .. 10, 11

    bool operator < (Card c) const {
        char realNum = GEN_REAL_NUM(num), cRealNum = GEN_REAL_NUM(c.num);
        if(realNum > cRealNum) return false;
        if(realNum < cRealNum) return true;
        if(type > c.type) return false;
        if(type < c.type) return true;
        return false;
    }

    bool operator == (Card c) const {
        return c.num == num && c.type == type;
    }
};

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

        void operator ++() {
            p.type = (Suit)(((char)(p.type)+1) % 5);
            if(p.type == Suit::ANY) {
                p.num += 1;
                p.type = Suit::DIAMOND;
                if(p.num == 14) p.num = 1;
                if(p.num == 2) {
                    p.num = 0;
                    p.type = Suit::ANY;
                }
            }
        }

        bool operator ==(const iterator &i) const {
            return p == i.p;
        }
    };
    SeqGenerator::iterator begin() const {
        return SeqGenerator::iterator(st);
    }
    SeqGenerator::iterator end() const {
        return e;
    }
};

Card operator "" _card(const char *cardName) {
    Card::Suit type; char num;
    switch(cardName[0]) {
    case 'j': case 'J':
        num = 11;
        break;
    case 'q': case 'Q':
        num = 12;
        break;
    case 'K': case 'k':
        num = 13;
        break;
    case '*':
        num = 0;
        break;
    case 'T': case 't':
        num = 10;
        break;
    case 'a': case 'A':
        num = 1;
        break;
    default:
        if(cardName[0] > '1' && cardName[0] <= '9') {
            num = cardName[0] - '0';
            break;
        }
        throw "Invalid number";
    }
    switch(cardName[1]) {
    case '1': case 'd': case 'D':
        type = Card::Suit::DIAMOND;
        break;
    case '2': case 'c': case 'C':
        type = Card::Suit::CLUB;
        break;
    case '3': case 'h': case 'H':
        type = Card::Suit::HEART;
        break;
    case '4': case 'S': case 's':
        type = Card::Suit::SPADE;
        break;
    default:
        throw "Invalid suit";
    }
    return Card{type, num};
}

random_device rd;
mt19937 mt;

class CardPool {
    int realUpperBound;
    Card cards[52];

    void swap_back(Card *p) {
        Card tmp = *p;
        *p = cards[realUpperBound];
        cards[realUpperBound] = tmp;
        realUpperBound--;
    }

public:
    CardPool(): realUpperBound(51) {
        int ind = 0;
        for(Card a: SeqGenerator()) {
            cards[ind++] = a;
        }
        assert(ind == realUpperBound + 1);
    }

    void pickout(Card card) {
        Card *p = find(cards, cards + realUpperBound + 1, card);
        if(p == cards + realUpperBound + 1) throw "card not found";
        swap_back(p);
    }

    void pickout(vector<Card> &&suite) {
        for(const Card &c: suite) {
            pickout(c);
        }
    }

    void put_back(int num=1) {
        realUpperBound += num;
    }

    void recover() {
        realUpperBound = 51;
    }

    Card pick() {
        auto p = cards + uniform_int_distribution<>(0, realUpperBound)(mt);
        Card ret = *p;
        swap_back(p);
        return ret;
    }
};

class CardRiver {
    inline bool hasCard(Card a, int showedCard=numShowed) {
        for(int i=0; i<showedCard; i++)
            if(cards[i] == a) return true;
        return false;
    }

    bool genRiver(function<bool(CardRiver *)> f, int maxDepth=5) {
        if(numShowed == maxDepth) {
            if(f(this)) return true;
            return false;
        }
        for(Card a: SeqGenerator()) {
            if(hasCard(a)) continue;
            cards[numShowed++] = a;
            bool ret = genRiver(f, maxDepth);
            numShowed--;
            if(ret) return true;
        }
        return false;
    }
public:
    Card cards[5];
    int numShowed = 0;
};

CardRiver emptyCardRiver;

struct CardCombination {
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
    };
    Card cards[5];
    Combination comb;
    int numOfParts;

    bool operator < (const CardCombination &o) {
        if(comb > o.comb) return false;
        if(comb < o.comb) return true;
        return numOfParts < o.numOfParts;
    }

    static inline int genNumOfParts(int mostSig, int second = 0, int third = 0, int fourth = 0, int fifth = 0) {
        return GEN_REAL_NUM(mostSig) * 13 * 13 * 13 * 13 +
               GEN_REAL_NUM(second) * 13 * 13 * 13 +
               GEN_REAL_NUM(third) * 13 * 13 +
               GEN_REAL_NUM(fourth) * 13 + GEN_REAL_NUM(fifth);
    }
};

class CardSuite {
    CardPool cp;
    Card cards[7];
    int cardNum = 2;

    int suitCounter[4];
    int numCounter[15];

    void calcCommon() {
        memset(suitCounter, 0, sizeof(suitCounter));
        memset(numCounter, 0, sizeof(numCounter));
        for(int i=0; i<cardNum; i++) {
            suitCounter[(char)cards[i].type]++;
            numCounter[(char)cards[i].num]++;
        }
    }

public:
    CardSuite() {
        randomize();
    }

    CardSuite(Card card1, Card card2) {
        cards[0] = card1;
        cards[1] = card2;
    }

    void randomize() {
        cp.recover();
        cards[0] = cp.pick();
        cards[1] = cp.pick();
    }

    CardCombination findCombination(const CardRiver &cr) {
        for(int i=0; i<cr.numShowed; i++) {
            cards[i+2] = cr.cards[i];
        }
        cardNum += cr.numShowed;
        calcCommon();
        // TODO
        cardNum = 2;
    }

    inline int hasFlush() const {
        return suitCounter[0] >= 5 || suitCounter[1] >= 5 || suitCounter[2] >= 5 || suitCounter[3] >= 5;
    }

    int hasStraightFlush() const {
        if(!hasStraight()) return false;
        if(!hasFlush()) return false;

    }
};

CardSuite operator "" _cards(const char *cardName) {
    CardSuite ret(operator "" _card(cardName), operator "" _card(cardName+2));
    for(int i=4; cardName[i]; i+=2) {
        ret[i >> 1] = operator "" _card(cardName+i);
    }
    return ret;
}

class SuiteProbabilityComparer {
    CardSuite mySuite, othersSuite;
public:
    int myWinings, myLoses, tot;

    SuiteProbabilityComparer(CardSuite _mySuite, CardSuite _othersSuite):
        mySuite(_mySuite), othersSuite(_othersSuite)
    {}

    bool compare(CardRiver *cr) {
        tot += 1;
        CardCombination &&myCC = mySuite.findCombination(*cr);
        CardCombination &&othersCC = othersSuite.findCombination(*cr);
        if(myCC < othersCC) myLoses++;
        else {
            if(othersCC < myCC) myWinings++;
        }
        return false;
    }

    void printResult() {
        double p_win = myWinings;
        double p_lose = myLoses;
        p_win /= tot; p_lose /= tot;
        cout << "total(" << tot << "): P(win)=" << p_win << ", P(lose)=" << p_lose << endl;
    }
};

int main() {
    SuiteProbabilityComparer spc("AsQh"_cards, "QdQc"_cards);
    emptyCardRiver.genRiver(spc.compare);
    spc.printResult();
    return 0;
}
