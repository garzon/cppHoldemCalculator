#include <iostream>
#include <assert.h>

#include "pokerriver.hpp"
#include "pokersuite.hpp"

#include "ostreamhelpers.hpp"

using namespace std;

class PokerSuiteProbabilityComparer {
    PokerSuite mySuite, othersSuite;
public:
    int myWinings = 0, myLoses = 0, tot = 0;

    PokerSuiteProbabilityComparer() {}

    PokerSuiteProbabilityComparer(PokerSuite _mySuite, PokerSuite _othersSuite):
        mySuite(_mySuite), othersSuite(_othersSuite)
    {}

    void reset() {
        myWinings = 0, myLoses = 0, tot = 0;
    }

    bool compare(PokerRiver *pr) {
        tot += 1;
        PokerCombination &&myCC = mySuite.findCombination(*pr);
        PokerCombination &&othersCC = othersSuite.findCombination(*pr);
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
    cout << "ok" << endl;
}

PokerSuite emptyPokerSuite;

bool printRiverTester(PokerRiver *pr) {
    cout << *pr << " " << emptyPokerSuite.findCombination(*pr) << endl;
    return false;
}

int main() {
    runTestCases();

    shared_ptr<CardPool> cp = make_shared<CardPool>();
    PokerRiver pr;
    //pr.genRandomRiver(printRiverTester, cp, 20);
    //return 0;

    PokerSuite mySuite("AsQh", cp);
    PokerSuite othersSuite("JdJc", cp);
    PokerSuiteProbabilityComparer pspc(mySuite, othersSuite);

    pr.genRiver([&pspc](PokerRiver *pr) { return pspc.compare(pr); });
    pspc.printResult();
    pspc.reset();

    pspc = {{"AsAd"},{"KhKd"}};
    pr.genRiver([&pspc](PokerRiver *pr) { return pspc.compare(pr); });
    pspc.printResult();
    pspc.reset();

    //pr.genRandomRiver([&pspc](PokerRiver *pr) { return pspc.compare(pr); }, cp);
    //pspc.printResult();

    return 0;
}
