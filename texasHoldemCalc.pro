TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

DISTFILES += \
    .gitignore

HEADERS += \
    card.hpp \
    ostreamhelpers.hpp \
    seqgenerator.hpp \
    cardpool.hpp \
    pokerriver.hpp \
    pokersuite.hpp \
    pokercombination.hpp
