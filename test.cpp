#include <cstdio>

#include "automat.h"

/*
void test1()
{
    Automat nfa;
    State s0 = 0;
    State s1 = 1;
    State s2 = 2;
    State s3 = 3;

    nfa.addStart(s0);

    nfa.addTransistion(s0, s1, 'a');
    nfa.addTransistion(s2, s3, 'b');
    nfa.addTransistion(s1, s2);

    nfa.addAccepting(s3);

    Automat dfa = Automat::ntod(nfa);

    bool n = nfa.simulateDFA("ab");
    printf("nfa: %d\n", n);
    bool d = dfa.simulateDFA("ab");
    printf("dfa: %d\n", d);
}
*/

void testDot()
{
    bool bo;

    Automat a = Automat::symbol('a');
    Automat b = Automat::symbol('b');
    Automat x = Automat::symbol('x');
    Automat y = Automat::symbol('y');
    
    bo = a.simulateDFA("a");
    printf("%d\n", bo);
    bo = b.simulateDFA("b");
    printf("%d\n", bo);
    Automat ab = Automat::dot(a, b);
    Automat abS = Automat::star(ab);
    Automat abSx = Automat::dot(abS, x);
    Automat abSxIy = Automat::bar(abSx, y);

    Automat dfa = Automat::ntod(abSxIy);
    bo = dfa.simulateDFA("y");
    printf("accept?: %d\n", bo);

    abSxIy.print();
}

int main()
{
    //test1();
    testDot();
}
