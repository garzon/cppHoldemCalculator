#ifndef CARD_HPP
#define CARD_HPP

const char *suitChars[] = {"","d","c","h","s","\xe2\x99\xa6","\xe2\x99\xa3","\xe2\x99\xa5","\xe2\x99\xa0"};
const char *numChars = " A23456789TJQK";
constexpr const int NUM_COUNT = 14;

struct Card {
    enum class Suit: char { DIAMOND = 1, CLUB = 2, HEART = 3, SPADE = 4, ANY = 0 } type;
    char num;

    Card(): num(0), type(Suit::ANY) {}
    Card(Suit _type, char _num) : type(_type), num(_num) {}
    Card(const char *cardName) {
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
            if(cardName[0] >= '1' && cardName[0] <= '9') {
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
    }

#define GEN_REAL_NUM(x) (((x) > 0) ? ((((x)+NUM_COUNT-3)%(NUM_COUNT-1))+1) : 0)
// A, 2, 3 .. Q, K -> 13, 1, 2 .. 11, 12. or 0
#define RECOVER_FROM_REAL_NUM(x) (((x) > 0) ? (((x)%(NUM_COUNT-1))+1) : 0)

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

    Card operator ++() {
        type = (Suit)(((char)(type)+1) % 5);
        if(type == Suit::ANY) {
            num += 1;
            type = Suit::DIAMOND;
            if(num == 14) num = 1;
            if(num == 2) {
                num = 0;
                type = Suit::ANY;
            }
        }
        return *this;
    }

    static inline int toInt(Suit type, char num) {
        return GEN_REAL_NUM(num) * 5 + (char)type;
    }

    inline int toInt() const {
        return toInt(type, num);
    }

    inline const char *suitToChr() const {
        return suitChars[(char)type];
    }

    inline char numToChr() const {
        return numChars[num];
    }
};

inline Card operator "" _card(const char *cardName) {
    return Card(cardName);
}

#endif // CARD_HPP
