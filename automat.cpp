#include <algorithm>
#include <set>
#include <stack>
#include <cstdio>

#include "automat.h"

int Automat::getStates()
{
    return numstates;
}

State Automat::addState()
{
    return numstates++;
}

void Automat::addStart(State s0)
{
    start = s0;
}

void Automat::addAccepting(State s0)
{
    accepting.insert(s0);
}

void Automat::addTransistion(State s0, State s1, char symbol)
{
    transistions.insert({std::make_pair(s0, symbol), s1});
    alphabet.insert(symbol);
}

/* ignores epsilon transistions */
bool Automat::simulateDFA(const char *str) const
{
    State s = start;
    char c = *str;
    while(c)
    {
        s = move(s, c);
        if(!s) return false;
        c = *++str;
    }
    if(accepting.count(s)) return true;
    return false;
}

State Automat::move(State s, char symbol) const
{
    auto search = transistions.find(std::make_pair(s, symbol));
    if(search != transistions.end()) return search->second;
    return -1;
}

std::set<State> Automat::move(const std::set<State>& T, char symbol) const
{
    std::set<State> set;
    for(State x: T)
    {
        auto range = transistions.equal_range(std::make_pair(x, symbol));
        for(auto it = range.first; it != range.second; it++)
        {
            set.insert(it->second);
        }
    }
    return set;
}

std::set<State> Automat::closure(State state) const
{
    std::set<State> s = { state };
    return closure(s);
}

std::set<State> Automat::closure(const std::set<State>& T) const
{
    std::set<State> set(T);
    std::stack<State> stack;
    for(State x: T)
    {
        stack.push(x);
    }
    while(!stack.empty()) 
    {
        State t = stack.top();
        stack.pop();
        auto range = transistions.equal_range(std::make_pair(t, epsilon));
        for(auto i = range.first; i != range.second; i++)
        {
            if(!set.count(i->second))
            {
                set.insert(i->second);
                stack.push(i->second);
            }
        }
    }
    return set;
}

Automat Automat::ntod(const Automat& nfa)
{
    char counter = 0;
    Automat dfa;
    std::set<std::set<State>> marked;
    std::set<std::set<State>> unmarked;
    std::map<std::set<State>, State> map;
    map.insert(std::make_pair(nfa.closure(nfa.start), counter++));
    unmarked.insert(map.begin()->first);
    dfa.start = map.begin()->second;

    while(!unmarked.empty())
    {
        std::set<State> T = *unmarked.begin();
        unmarked.erase(T);
        marked.insert(T);

        for(char symbol: nfa.alphabet)
        {
            std::set<State> U = nfa.closure(nfa.move(T, symbol));
            if(U.empty()) continue;
            if(!marked.count(U) && !unmarked.count(U))
            {
                unmarked.insert(U);
                map.insert(std::make_pair(U, counter++));
            }
            dfa.addTransistion(map[T], map[U], symbol);
        }
    }
    for(std::set<State> set: marked)
    {
        std::set<State> intersection;
        std::set_intersection(
                nfa.accepting.begin(), nfa.accepting.end(),
                set.begin(), set.end(),
                std::inserter(intersection, intersection.begin()));
        if(intersection.size())
        {
            dfa.accepting.insert(map[set]);
        }
    }
    return dfa;
}

void Automat::print()
{
    printf("-----------------\n");
    printf("start: %d\n", start);
    for(auto value: transistions)
    {
        printf("%d -> %c -> %d\n", value.first.first, value.first.second, value.second);
    }
    printf("-----------------\n");
    printf("accepting: %d\n", *accepting.begin());
}

Automat Automat::symbol(char symbol)
{
    Automat a;
    State s0 = a.addState();
    State s1 = a.addState();
    a.addStart(s0);
    a.addAccepting(s1);
    a.addTransistion(s0, s1, symbol);

    return a;
}

void offsetInsert(
        std::multimap<std::pair<State, char>, State>& to, 
        const std::multimap<std::pair<State, char>, State>& from, 
        int offset)
{
    for(auto value: from)
    {
        State s0 = value.first.first + offset;
        State s1 = value.second + offset;
        char symbol = value.first.second;
        to.insert({std::make_pair(s0, symbol), s1});
    }
}

Automat Automat::dot(const Automat& a, const Automat& b)
{
    Automat c;
    c.alphabet.insert(a.alphabet.begin(), a.alphabet.end());
    c.alphabet.insert(b.alphabet.begin(), b.alphabet.end());
    offsetInsert(c.transistions, a.transistions, 0);
    offsetInsert(c.transistions, b.transistions, a.numstates);
    //dot
    c.addTransistion(*a.accepting.begin(), b.start + a.numstates);

    c.numstates = a.numstates + b.numstates;
    c.start = a.start;  
    for(auto value: b.accepting)
    {
        c.accepting.insert(value + a.numstates);
    }
    return c;
}



Automat Automat::bar(const Automat &a, const Automat &b)
{
    Automat c;
    c.alphabet.insert(a.alphabet.begin(), a.alphabet.end());
    c.alphabet.insert(b.alphabet.begin(), b.alphabet.end());
    offsetInsert(c.transistions, a.transistions, 1);
    offsetInsert(c.transistions, b.transistions, 1 + a.numstates);
    c.numstates = a.numstates + b.numstates + 2;
    //bar
    c.addTransistion(0, 1);
    c.addTransistion(0, 1 + a.numstates);
    c.addTransistion(a.numstates, c.numstates - 1);
    c.addTransistion(c.numstates - 2, c.numstates - 1);
    c.accepting.insert(c.numstates - 1);
    c.start = 0;

    return c;
}

Automat Automat::star(const Automat &a)
{
    Automat c;
    c.alphabet.insert(a.alphabet.begin(), a.alphabet.end());
    offsetInsert(c.transistions, a.transistions, 1);
    c.numstates = a.numstates + 2;
    //star
    c.addTransistion(0, 1);
    c.addTransistion(0, c.numstates - 1);
    c.addTransistion(c.numstates - 2, 1);
    c.addTransistion(c.numstates - 2, c.numstates - 1);
    c.accepting.insert(c.numstates - 1);
    c.start = 0;

    return c;
}
