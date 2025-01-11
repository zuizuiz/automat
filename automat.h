#ifndef AUTOMAT_H
#define AUTOMAT_H

#include <cstdint>
#include <set>
#include <map>

typedef char State;

/*
Modells both DFA and NFA
Converts NFAs to DFAs
Simulates DFAs
*/
class Automat
{
    public:
    static Automat symbol(char symbol);
    static Automat ntod(const Automat& nfa);
    static Automat star(const Automat& a);
    static Automat dot(const Automat& a, const Automat& b);
    static Automat bar(const Automat& a, const Automat& b);

    void print();
    int getStates();
    State addState();
    void addStart(State s0);
    void addAccepting(State s0);
    void addTransistion(State s0, State s1, char symbol = epsilon);
    bool simulateDFA(const char* str) const;

    private:
    static constexpr char epsilon = -1;

    int numstates = 0;
    State start = 0;
    std::set<char> alphabet;
    std::set<State> accepting;
    std::multimap<std::pair<State, char>, State> transistions;

    State move(State s, char c) const;
    std::set<State> move(const std::set<State>& T, char symbol) const;
    std::set<State> closure(State state) const;
    std::set<State> closure(const std::set<State>& T) const;
};

#endif
