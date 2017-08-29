#include <iostream>
#include <string>
#include <assert.h>
#include <map>

#include "pokerriver.hpp"
#include "pokersuite.hpp"

#include "ostreamhelpers.hpp"

using namespace std;

class PokerSuiteProbabilityComparer {
    PokerSuite mySuite;
    shared_ptr<vector<PokerSuite>> othersSuite = nullptr;
public:
    int myWinings = 0, myLoses = 0, tot = 0;

    PokerSuiteProbabilityComparer() {}

    PokerSuiteProbabilityComparer(const PokerSuite &_mySuite, const PokerSuite &_othersSuite):
        mySuite(_mySuite)
    {
        othersSuite = make_shared<vector<PokerSuite>>();
        othersSuite->emplace_back(_othersSuite);
    }

    PokerSuiteProbabilityComparer(const PokerSuite &_mySuite, shared_ptr<vector<PokerSuite>> _othersSuite=nullptr):
        mySuite(_mySuite), othersSuite(_othersSuite)
    {
        if(othersSuite.get() == nullptr) othersSuite = make_shared<vector<PokerSuite>>();
    }

    void reset() {
        myWinings = 0, myLoses = 0, tot = 0;
    }

    bool callback(PokerRiver *pr) {
        tot += 1;
        PokerCombination myCC = mySuite.findCombinationWithRiver(*pr);
        bool isWin = true;
        for(PokerSuite &ps: *othersSuite) {
            PokerCombination othersCC = ps.findCombinationWithRiver(*pr);
            if(myCC < othersCC) {
                myLoses++;
            }
            if(!(othersCC < myCC)) {
                isWin = false;
                break;
            }
        }
        if(isWin) myWinings++;
        return false;
    }

    void printResult() {
        double p_win = myWinings;
        double p_lose = myLoses;
        p_win /= tot; p_lose /= tot;
        cout << "total(" << tot << "): P(win)=" << p_win << ", P(lose)=" << p_lose << endl;
    }
};

class CombinationProbabilityCalculator {
    PokerSuite mySuite;
    int probabilities[10], tot = 0;
public:
    CombinationProbabilityCalculator(const PokerSuite &_mySuite):
        mySuite(_mySuite)
    {
        memset(probabilities, 0, sizeof(probabilities));
    }

    bool callback(PokerRiver *pr) {
        tot += 1;
        mySuite.addRiver(*pr);
        if(mySuite.hasStraightFlush()) probabilities[(char)PokerCombination::Combination::STRAIGHT_FLUSH]++;
        if(mySuite.has4OfAKind()) probabilities[(char)PokerCombination::Combination::KIND_OF_4]++;
        if(mySuite.hasFullHouse()) probabilities[(char)PokerCombination::Combination::FULL_HOUSE]++;
        if(mySuite.hasFlush()) probabilities[(char)PokerCombination::Combination::FLUSH]++;
        if(mySuite.hasStraight()) probabilities[(char)PokerCombination::Combination::STRAIGHT]++;
        if(mySuite.has3OfAKind()) probabilities[(char)PokerCombination::Combination::KIND_OF_3]++;
        if(mySuite.hasTwoPairs()) probabilities[(char)PokerCombination::Combination::TWO_PAIRS]++;
        if(mySuite.hasPair()) probabilities[(char)PokerCombination::Combination::PAIR]++;
        mySuite.dropRiver(pr->getCardNum());
        return false;
    }

    void printResult() {
        for(char i=(char)PokerCombination::Combination::PAIR; i<=(char)PokerCombination::Combination::STRAIGHT_FLUSH; i++) {
            cout << "P(" << typeName[i] << ")=" << probabilities[i]*1.0/tot << " ";
        }
        cout << endl;
    }
};

void runTestCases() {
    for(int i=0; i<NUM_COUNT; i++) assert(RECOVER_FROM_REAL_NUM(GEN_REAL_NUM(i)) == i);

    assert(PokerSuite("AdAcAh2c3c").findCombination() == PokerCombination(PokerCombination::Combination::KIND_OF_3, 1, 3, 2));
    assert(PokerSuite("2c3h4sAdKdTd5h").findCombination() == PokerCombination(PokerCombination::Combination::STRAIGHT, 5));
    assert(PokerSuite("2d3d4sAdKdTd5d").findCombination() == PokerCombination(PokerCombination::Combination::FLUSH, 1, 13, 10, 5, 3));
    assert(PokerSuite("2c3h4s2d3d4d").findCombination() == PokerCombination(PokerCombination::Combination::TWO_PAIRS, 4, 3, 2));

    assert(PokerSuite("2d3d4dAdKdTd5d").findCombination() == PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, 5));
    assert(PokerSuite("2d3d4dAdKdTd6d").findCombination() == PokerCombination(PokerCombination::Combination::FLUSH, 1, 13, 10, 6, 4));
    assert(PokerSuite("2d3d4dAdKd6d5d").findCombination() == PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, 6));
    assert(PokerSuite("TdJd4dAdKd6dQd").findCombination() == PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, 1));

    assert(PokerSuite("AdAcAh2d2c2h3s").findCombination() == PokerCombination(PokerCombination::Combination::FULL_HOUSE, 1, 2));
    assert(PokerSuite("AdAcAh2d2c2h2s").findCombination() == PokerCombination(PokerCombination::Combination::KIND_OF_4, 2, 1));
    cout << "ok" << endl;
}

void printRandomRiver() {
    shared_ptr<CardPool> cp = make_shared<CardPool>();
    PokerRiver pr(cp);
    PokerSuite emptyPokerSuite;
    pr.genRandomRiver([&emptyPokerSuite](PokerRiver *pr) {
        cout << *pr << " " << emptyPokerSuite.findCombinationWithRiver(*pr) << endl;
        return false;
    }, 20);
}

#define CALLBACK_EXP(riverCallable) [&riverCallable](PokerRiver *pr) { return riverCallable.callback(pr); }

void printCompareResult(PokerSuite mySuite, PokerSuite othersSuite, shared_ptr<PokerRiver> pr=nullptr) {
    if(pr.get() == nullptr) {
        pr = make_shared<PokerRiver>(make_shared<CardPool>());
    }
    pr->cp->pickout(mySuite);
    pr->cp->pickout(othersSuite);
    PokerSuiteProbabilityComparer pspc(mySuite, {othersSuite});
    pr->genRiver(CALLBACK_EXP(pspc));
    pr->cp->put_back(mySuite.getCardNum());
    pr->cp->put_back(othersSuite.getCardNum());
    pspc.printResult();
}

void calc_a_round(PokerSuite &mySuite, PokerRiver &pr, int &wins, int &loses, int &tot, int winNTimes = 1) {
    auto othersSuites = make_shared<vector<PokerSuite>>();
    for(int i=0; i<winNTimes; i++)
        othersSuites->emplace_back(pr.cp);
    for(int i=0; i<=400; i++) {
        for(PokerSuite &ps: *othersSuites) ps.randomize();
        PokerSuiteProbabilityComparer pspc(mySuite, othersSuites);
        pr.genRandomRiver(CALLBACK_EXP(pspc), 400);
        wins += pspc.myWinings;
        loses += pspc.myLoses;
        tot += pspc.tot;
        pr.cp->put_back(2 * winNTimes);
    }
    /*CombinationProbabilityCalculator cpc(mySuite);
    pr.genRandomRiver(CALLBACK_EXP(cpc), 1000000);
    cpc.printResult();*/
}

void interact() {
    string inp;

    cout << "Init: ";
    cin >> inp;
    shared_ptr<CardPool> cp = make_shared<CardPool>();
    PokerSuite mySuite(inp.c_str(), cp);
    PokerRiver pr(cp);
    int wins, loses, tot;
#define DO_A_ROUND wins = loses = tot = 0; calc_a_round(mySuite, pr, wins, loses, tot); cout << wins*1.0/tot << " " << loses*1.0/tot << endl
    DO_A_ROUND;

#define ROUND(x) cout << x << ": "; cin >> inp; if(inp == "x") return; pr.push(Cards(inp.c_str())); DO_A_ROUND
    ROUND("Flop");
    ROUND("Turn");
    ROUND("Final");
}

pair<double, double> printInitProbability(Card card1, Card card2) {
    cout << "Calculating " << card1 << card2 << endl;
    shared_ptr<CardPool> cp = make_shared<CardPool>();
    PokerSuite mySuite(card1, card2, cp);
    PokerRiver pr(cp);
    int wins, loses, tot;
    DO_A_ROUND;
    return make_pair(wins*1.0/tot, loses*1.0/tot);
}

map<double, pair<pair<Card, Card>, pair<double, double>>> initProb;
void printAllInitProb() {
    for(int i=1; i<=13; i++) {
        Card card1(Card::Suit::DIAMOND, RECOVER_FROM_REAL_NUM(i));
        for(int j=i; j<=13; j++) {
            Card card2(Card::Suit::CLUB, RECOVER_FROM_REAL_NUM(j));
            pair<double, double> res = printInitProbability(card1, card2);
            initProb[res.first - res.second] = make_pair(make_pair(card1, card2), res);
            if(j == i) continue;
            Card card2_sameSuit(Card::Suit::DIAMOND, RECOVER_FROM_REAL_NUM(j));
            res = printInitProbability(card1, card2_sameSuit);
            initProb[res.first - res.second] = make_pair(make_pair(card1, card2_sameSuit), res);
        }
    }

    for(const auto &p: initProb) {
        cout << p.second.first.first << p.second.first.second << ": " << p.first << "(" << p.second.second.first << " - " << p.second.second.second << ")" << endl;
    }
}


int main() {
    //runTestCases();
    //printRandomRiver();

    printCompareResult({"KsJs"},{{"QhTd"}}, make_shared<PokerRiver>(Cards{"3s5sQc"}));
    /*
    printCompareResult({"AsQh"},{{"QcQd"}});
    printCompareResult({"AsQh"},{{"QcQs"}});
    printCompareResult({"AsAd"},{{"KhKd"}});
    printCompareResult({"AcQd"},{{"3s7s"}});
    printCompareResult({"AcQd"},{{"3s7h"}});
*/

    /*printAllInitProb();
    return 0;*/

    while(1) {
        interact();
        cout << endl;
    }

    return 0;
}
