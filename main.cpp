#include <iostream>
#include <string>
#include <assert.h>

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

    bool compare(PokerRiver *pr) {
        tot += 1;
        PokerCombination myCC = mySuite.findCombination(*pr);
        bool isWin = true;
        for(PokerSuite &ps: *othersSuite) {
            PokerCombination othersCC = ps.findCombination(*pr);
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

void runTestCases() {
    for(int i=0; i<NUM_COUNT; i++) assert(RECOVER_FROM_REAL_NUM(GEN_REAL_NUM(i)) == i);

    PokerRiver emptyPokerRiver;
    assert(PokerSuite("AdAcAh2c3c").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::KIND_OF_3, 1, 3, 2));
    assert(PokerSuite("2c3h4sAdKdTd5h").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::STRAIGHT, 5));
    assert(PokerSuite("2d3d4sAdKdTd5d").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::FLUSH, 1, 13, 10, 5, 3));
    assert(PokerSuite("2c3h4s2d3d4d").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::TWO_PAIRS, 4, 3, 2));

    assert(PokerSuite("2d3d4dAdKdTd5d").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, 5));
    assert(PokerSuite("2d3d4dAdKdTd6d").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::FLUSH, 1, 13, 10, 6, 4));
    assert(PokerSuite("2d3d4dAdKd6d5d").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, 6));
    assert(PokerSuite("TdJd4dAdKd6dQd").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::STRAIGHT_FLUSH, 1));

    assert(PokerSuite("AdAcAh2d2c2h3s").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::FULL_HOUSE, 1, 2));
    assert(PokerSuite("AdAcAh2d2c2h2s").findCombination(emptyPokerRiver) == PokerCombination(PokerCombination::Combination::KIND_OF_4, 2, 1));
    cout << "ok" << endl;
}

PokerSuite emptyPokerSuite;

bool printRiverTester(PokerRiver *pr) {
    cout << *pr << " " << emptyPokerSuite.findCombination(*pr) << endl;
    return false;
}

void printRandomRiver() {
    PokerRiver pr;
    shared_ptr<CardPool> cp = make_shared<CardPool>();
    pr.genRandomRiver(printRiverTester, cp, 20);
}

void printCompareResult(PokerSuite mySuite, PokerSuite othersSuite) {
    PokerRiver pr;
    PokerSuiteProbabilityComparer pspc(mySuite, othersSuite);
    pr.genRiver([&pspc](PokerRiver *pr) { return pspc.compare(pr); });
    pspc.printResult();
}

void printCompareResult(PokerRiver pr, PokerSuite mySuite, vector<PokerSuite> othersSuite) {
    PokerSuiteProbabilityComparer pspc(mySuite, shared_ptr<vector<PokerSuite>>(&othersSuite));
    pr.genRiver([&pspc](PokerRiver *pr) { return pspc.compare(pr); });
    pspc.printResult();
}

void calc_a_round(PokerSuite &mySuite, PokerRiver &pr, int &wins, int &loses, int &tot, int winNTimes = 1) {
    auto othersSuites = make_shared<vector<PokerSuite>>();
    for(int i=0; i<winNTimes; i++)
        othersSuites->emplace_back(mySuite.cp);
    for(int i=0; i<=1000; i++) {
        for(PokerSuite &ps: *othersSuites) ps.randomize();
        PokerSuiteProbabilityComparer pspc(mySuite, othersSuites);
        pr.genRandomRiver([&pspc](PokerRiver *pr) { return pspc.compare(pr); }, mySuite.cp, 500);
        wins += pspc.myWinings;
        loses += pspc.myLoses;
        tot += pspc.tot;
        mySuite.cp->put_back(2 * winNTimes);
    }
}

void print_and_reset_result(int &wins, int &loses, int &tot) {
    cout << wins*1.0/tot << " " << loses*1.0/tot << endl;
    wins = loses = tot = 0;
}

void interact() {
    string inp; int people;
    cout << "#people: ";
    cin >> people;
    people--;

    cout << "Init: ";
    cin >> inp;
    shared_ptr<CardPool> cp = make_shared<CardPool>();
    PokerSuite mySuite(inp.c_str(), cp);
    PokerRiver pr;
    int wins, loses, tot;
#define DO_A_ROUND wins = loses = tot = 0; calc_a_round(mySuite, pr, wins, loses, tot, people); print_and_reset_result(wins, loses, tot)
    DO_A_ROUND;

#define ROUND(x) cout << x << ": "; cin >> inp; pr.push(Cards(inp.c_str())); DO_A_ROUND
    ROUND("Flop");
    ROUND("Turn");
    ROUND("Final");
}


int main() {
    runTestCases();
    //printRandomRiver();

    /*
    printCompareResult({"AsQh"},{{"QcQd"}});
    printCompareResult({"AsQh"},{{"QcQs"}});
    printCompareResult({"AsAd"},{{"KhKd"}});
    printCompareResult({"AcQd"},{{"3s7s"}});
    printCompareResult({"AcQd"},{{"3s7h"}});
    */


    while(1) {
        interact();
        cout << endl;
    }

    return 0;
}
