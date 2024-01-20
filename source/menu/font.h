#include <nds.h>




#ifndef FONT_H
#define FONT_H

enum
{
    space = 0,
    pal0 = 0,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    zero,
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    nine,
    colon,
    x,
    caret,
    period,
    dash,
    underscore,
    lparen,
    rparen,
    pal1,
    pal2,
    pal3,
    pal4,
    pal5,
    pal6,
    pal7,
    pal8,
    pal9,
    pal10,
    pal11,
    pal12,
    pal13,
    pal14,
    pal15,
    max,
};

const u8 lookup[256] =
{
[' '] = space,
['\xC0'] = pal0,
['a'] = A,
['A'] = A,
['b'] = B,
['B'] = B,
['c'] = C,
['C'] = C,
['d'] = D,
['D'] = D,
['e'] = E,
['E'] = E,
['f'] = F,
['F'] = F,
['g'] = G,
['G'] = G,
['h'] = H,
['H'] = H,
['i'] = I,
['I'] = I,
['j'] = J,
['J'] = J,
['k'] = K,
['K'] = K,
['l'] = L,
['L'] = L,
['m'] = M,
['M'] = M,
['n'] = N,
['N'] = N,
['o'] = O,
['O'] = O,
['p'] = P,
['P'] = P,
['q'] = Q,
['Q'] = Q,
['r'] = R,
['R'] = R,
['s'] = S,
['S'] = S,
['t'] = T,
['T'] = T,
['u'] = U,
['U'] = U,
['v'] = V,
['V'] = V,
['w'] = W,
['W'] = W,
['x'] = X,
['X'] = X,
['y'] = Y,
['Y'] = Y,
['z'] = Z,
['Z'] = Z,
['0'] = zero,
['1'] = one,
['2'] = two,
['3'] = three,
['4'] = four,
['5'] = five,
['6'] = six,
['7'] = seven,
['8'] = eight,
['9'] = nine,
[':'] = colon,
['\x80'] = x,
['>'] = caret,
['.'] = period,
['-'] = dash,
['_'] = underscore,
['('] = lparen,
[')'] = rparen,
['\xC1'] = pal1,
['\xC2'] = pal2,
['\xC3'] = pal3,
['\xC4'] = pal4,
['\xC5'] = pal5,
['\xC6'] = pal6,
['\xC7'] = pal7,
['\xC8'] = pal8,
['\xC9'] = pal9,
['\xCA'] = pal10,
['\xCB'] = pal11,
['\xCC'] = pal12,
['\xCD'] = pal13,
['\xCE'] = pal14,
['\xCF'] = pal15,

};

const u8 charset[][32] =
{
// Null / Space / pal 0
{

},
// A
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
},
// B
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x00,
},
// C
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x00,
},
// D
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x00,
},
// E
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x11, 0x01, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x11, 0x11, 0x00,
},
// F
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x11, 0x01, 0x00,
    0x10, 0x11, 0x01, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
},
// G
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x11, 0x11, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x11, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x01,
},
// H
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x01,
    0x10, 0x11, 0x11, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
},
// I
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x01,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x10, 0x11, 0x11, 0x01,
},
// J
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x10, 0x10, 0x01, 0x00,
    0x10, 0x11, 0x01, 0x00,
},
// K
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x11, 0x01,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x11, 0x01, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x11, 0x01,
    0x10, 0x01, 0x10, 0x01,

},
// L
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x01,
}, 
// M
{
    0x00, 0x00, 0x00, 0x00,
    0x01, 0100, 0x00, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x01, 0x11, 0x01,
    0x11, 0x11, 0x11, 0x01,
    0x01, 0x11, 0x01, 0x01,
    0x01, 0x10, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x01,
},
// N
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x01,
    0x11, 0x01, 0x00, 0x01,
    0x11, 0x11, 0x00, 0x01,
    0x01, 0x11, 0x01, 0x01,
    0x01, 0x10, 0x11, 0x01,
    0x01, 0x00, 0x11, 0x01,
    0x01, 0x00, 0x10, 0x01,
},
// O
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x00,
},
// P
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x00,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x11, 0x11, 0x00,
    0x11, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00,
},
// Q
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x10, 0x10, 0x01,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x00, 0x10, 0x11,
    0x10, 0x11, 0x11, 0x10,
},
// R
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x00,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x11, 0x11, 0x00,
    0x11, 0x10, 0x11, 0x00,
    0x11, 0x00, 0x11, 0x01,
},
// S
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x01,
    0x11, 0x01, 0x10, 0x01,
    0x11, 0x01, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x01,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x11, 0x11, 0x00,
},
// T
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x01,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
},
// U
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x00,
},
// V
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x10, 0x01, 0x11, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x00, 0x11, 0x01, 0x00,
},
// W
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x10, 0x10, 0x01,
    0x11, 0x10, 0x10, 0x01,
    0x11, 0x11, 0x11, 0x01,
    0x10, 0x01, 0x11, 0x00,
},
// X
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x00, 0x10, 0x01,
    0x11, 0x01, 0x11, 0x01,
    0x10, 0x11, 0x11, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x11, 0x01, 0x11, 0x01,
    0x11, 0x00, 0x10, 0x01,
},
// Y
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x11, 0x11, 0x01,
    0x00, 0x11, 0x11, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
},
// Z
{
    0x00, 0x00, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x01,
    0x00, 0x00, 0x11, 0x01,
    0x00, 0x10, 0x11, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x11, 0x00, 0x00,
    0x11, 0x01, 0x00, 0x00,
    0x11, 0x11, 0x11, 0x01,
},
// zero
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x00, 0x10, 0x00,
    0x11, 0x00, 0x11, 0x01,
    0x11, 0x10, 0x10, 0x01,
    0x11, 0x01, 0x10, 0x01,
    0x10, 0x00, 0x10, 0x00,
    0x00, 0x11, 0x01, 0x00,
},
// one
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x10, 0x11, 0x11, 0x01,
},
// two
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x11, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x01,
},
// three
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x10, 0x00, 0x11, 0x00,
    0x00, 0x11, 0x01, 0x00,
},
// four
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x11, 0x00,
    0x10, 0x01, 0x11, 0x00,
    0x10, 0x01, 0x11, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
},
// five
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x00, 0x00, 0x00,
    0x10, 0x10, 0x01, 0x00,
    0x10, 0x01, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x10, 0x00, 0x11, 0x00,
    0x10, 0x11, 0x01, 0x00,
},
// six
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x00,
},
// seven
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x01,
    0x00, 0x00, 0x10, 0x01,
    0x00, 0x00, 0x11, 0x01,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x10, 0x11, 0x00,
    0x00, 0x10, 0x01, 0x00,
    0x00, 0x11, 0x01, 0x00,
},
// eight
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x00,
},
// nine
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x10, 0x01, 0x10, 0x01,
    0x10, 0x01, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x01,
    0x00, 0x00, 0x10, 0x01,
    0x10, 0x00, 0x10, 0x01,
    0x00, 0x11, 0x11, 0x00,
},
// colon
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
},
// x
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x00,
    0x00, 0x10, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00,
},
// caret
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x00, 0x11, 0x01, 0x00,
    0x00, 0x10, 0x11, 0x01,
    0x00, 0x11, 0x01, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
},
// period
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
},
// dash
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x00, 0x11, 0x11, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
},
// underscore
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x11, 0x11, 0x00,
},
// left parenthesis
{
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x01,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x11, 0x00,
    0x00, 0x00, 0x10, 0x01,
},
// right parenthesis
{
    0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x00, 0x11, 0x00, 0x00,
    0x10, 0x01, 0x00, 0x00,
},
// pal 1
{
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
    0x11, 0x11, 0x11, 0x11, 
},
//pal 2
{
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
    0x22, 0x22, 0x22, 0x22, 
},
// pal 3
{
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33,
},
// pal 4
{
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
},
// pal 5
{
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55,
},
// pal 6
{
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66,
},
// pal 7
{
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77,
},
// pal 8
{
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x88,
},
// pal 9
{
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
    0x99, 0x99, 0x99, 0x99,
},
// pal 10
{
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA,
},
// pal 11
{
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
    0xBB, 0xBB, 0xBB, 0xBB,
},
// pal 12
{
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
    0xCC, 0xCC, 0xCC, 0xCC,
},
// pal 13
{
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
    0xDD, 0xDD, 0xDD, 0xDD,
},
// pal 14
{
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
    0xEE, 0xEE, 0xEE, 0xEE, 
},
// pal 15
{
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
},
};
#endif
