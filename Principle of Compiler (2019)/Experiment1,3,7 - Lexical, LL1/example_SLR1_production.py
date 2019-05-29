# coding=utf-8
from tokens import *
from language import *


SS = Nonterminal("S'")
S = Nonterminal('S')
E = Nonterminal('E')
T = Nonterminal('T')
F = Nonterminal('F')

SS.to(S)
S.to(TokenType.NUM, OP.ASSIGN, E)
E.to(E, OP.ADD, T)
E.to(E, OP.MINUS, T)
E.to(T)
T.to(T, OP.MUL, F)
T.to(T, OP.DIV, F)
T.to(F)
F.to(OP.L_PAR, E, OP.R_PAR)
F.to(TokenType.NUM)
