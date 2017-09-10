#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    Print, Lparen, Rparen, Plus, Minus, Multi, Divi,
    Assign, VarName, IntNum, EofTkn, Others
} Kind;

typedef struct {
    Kind kind; // a kind of token
    int val;
} Token;

int ch; // current character
int stkct; // for stack
int errF; // for error
char buf[80], *bufp; // for input
int var[26];
int errF;
#define STK_SIZ 20
int stack[STK_SIZ+1];

Token token;
void input(void);
void statement(void);
Token nextTkn(void);
int nextCh(void);
void checkTkn(Kind);
void expression(void);
int pop(void);
void push(int);
void term(void);
void operate(Kind);
void factor(void);

int main() {
    while (1) {
        input();
        token = nextTkn();
        if (token.kind == EofTkn) {
            exit(1);
        }
        statement();
        if (errF) {
            puts("  --err--");
        }
    }
}

int pop(void) {
    if (errF) {
        return 1;
    }
    if (stkct < 1) {
        puts("stack underflow");
        exit(1);
    }
    return stack[stkct--];
}

void push(int n) {
    if (errF) {
        return;
    }
    if (stkct + 1 > STK_SIZ) {
        puts("stack overflow");
        exit(1);
    }

    stack[++stkct] = n;
}

void input(void)
{
    errF = 0;
    stkct = 0;

    buf[0] = '\0';

    fgets(buf, 80, stdin);
    bufp = buf;
    ch = nextCh();
}

void statement(void) {
    int vNbr;

    switch (token.kind) {
        case VarName:
            vNbr = token.val;
            token = nextTkn();
            checkTkn(Assign); // like an assert
            if (errF) {
                break;
            }
            token = nextTkn();
            expression();
            var[vNbr] = pop();
            break;
        case Print:
            token = nextTkn();
            expression();
            checkTkn(EofTkn);
            if (errF) {
                break;
            }
            printf(" %d\n", pop());
            return;
        default:
            errF = 1;
    }

    checkTkn(EofTkn);
}

void expression(void) {
    Kind op;
    term();

    while (token.kind == Plus || token.kind == Minus) {
        op = token.kind;
        token = nextTkn();
        term();
        operate(op);
    }
}

void term(void) {
    Kind op;

    factor();
    while (token.kind == Multi || token.kind == Divi)  {
        op = token.kind;
        token = nextTkn();
        factor();
        operate(op);
    }
}

void factor(void) {
    switch (token.kind) {
        case VarName:
            push(var[token.val]);
            break;
        case IntNum:
            push(token.val);
            break;
        case Lparen:
            token = nextTkn();
            expression();
            checkTkn(Rparen);
            break;
        default:
            errF = 1;
    }
    token = nextTkn();
}

void operate(Kind op) {
    int d2 = pop(); int d1 = pop();

    if (op == Divi && d2 == 0) {
        puts("division by 0");
        errF = 1;
    }
    if (errF) {
        return;
    }

    switch (op) {
        case Plus: push(d1 + d2); break;
        case Minus: push(d1 - d2); break;
        case Multi: push (d1 * d2); break;
        case Divi: push(d1 / d2); break;
    }
}

int nextCh(void) // move next char
{
    if (*bufp == '\0') {
      return '\0';
    }

    return *bufp++;
}
void checkTkn(Kind kd) {
    if (token.kind != kd) {
        errF = 1;
    }
}

Token nextTkn(void) {
    int num;
    Token tk = {Others, 0};

    while (isspace(ch)) { // ignore a space
        ch = nextCh();
    }

    if (isdigit(ch)) { // int
        for (num = 0; isdigit(ch); ch = nextCh()) {
            num = num * 10 + (ch - '0');
        }
        tk.kind = IntNum;
        tk.val = num;
    } else if (islower(ch)) { // var name
        tk.kind = VarName;
        tk.val = ch - 'a';
        ch = nextCh();
    } else {
        switch (ch) {
            case '(': tk.kind = Lparen; break;
            case ')': tk.kind = Rparen; break;
            case '+': tk.kind = Plus; break;
            case '-': tk.kind = Minus; break;
            case '*': tk.kind = Multi; break;
            case '/': tk.kind = Divi; break;
            case '=': tk.kind = Assign; break;
            case '?': tk.kind = Print; break;
            case '\0': tk.kind = EofTkn; break;
        }
        ch = nextCh();
    }

    return tk;
}