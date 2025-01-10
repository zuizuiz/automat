#include <algorithm>
#include <set>
#include <stack>

#include "automat.h"

Automat::~Automat()
{
    std::set<State*> states;
    for(auto value: transistions)
    {
        states.insert(value.second);
    }
    for(State* state: states)
    {
        delete state;
    }
}

void Automat::addStart(State* s0)
{
    start = s0;
}

void Automat::addAccepting(State* s0)
{
    accepting.insert(s0);
}

void Automat::addTransistion(State* s0, State* s1, char symbol)
{
    transistions.insert({std::make_pair(s0, symbol), s1});
    alphabet.insert(symbol);
}

/* ignores epsilon transistions */
bool Automat::simulateDFA(const char *str) const
{
    State* s = start;
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

State* Automat::move(State* s, char symbol) const
{
    auto search = transistions.find(std::make_pair(s, symbol));
    if(search != transistions.end()) return search->second;
    return nullptr;
}

std::set<State*> Automat::move(const std::set<State*>& T, char symbol) const
{
    std::set<State*> set;
    for(State* x: T)
    {
        auto range = transistions.equal_range(std::make_pair(x, symbol));
        for(auto it = range.first; it != range.second; it++)
        {
            set.insert(it->second);
        }
    }
    return set;
}

std::set<State*> Automat::closure(State* state) const
{
    std::set<State*> s = { state };
    return closure(s);
}

std::set<State*> Automat::closure(const std::set<State*>& T) const
{
    std::set<State*> set(T);
    std::stack<State*> stack;
    for(State* x: T)
    {
        stack.push(x);
    }
    while(!stack.empty()) 
    {
        State* t = stack.top();
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
    Automat dfa;
    std::set<std::set<State*>> marked;
    std::set<std::set<State*>> unmarked;
    std::map<std::set<State*>, State*> map;
    map.insert(std::make_pair(nfa.closure(nfa.start), new State));
    unmarked.insert(map.begin()->first);
    dfa.start = map.begin()->second;

    while(!unmarked.empty())
    {
        std::set<State*> T = *unmarked.begin();
        unmarked.erase(T);
        marked.insert(T);

        for(char symbol: nfa.alphabet)
        {
            std::set<State*> U = nfa.closure(nfa.move(T, symbol));
            if(U.empty()) continue;
            if(!marked.count(U) && !unmarked.count(U))
            {
                unmarked.insert(U);
                map.insert(std::make_pair(U, new State));
            }
            dfa.addTransistion(map[T], map[U], symbol);
        }
    }
    for(std::set<State*> set: marked)
    {
        std::set<State*> intersection;
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
