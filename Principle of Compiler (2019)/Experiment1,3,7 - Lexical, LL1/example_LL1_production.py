# coding=utf-8
from tokens import *
from language import *


E = Nonterminal('E')
EE = Nonterminal("E'")
T = Nonterminal('T')
TT = Nonterminal("T'")
F = Nonterminal('F')
A = Nonterminal('A')
M = Nonterminal('M')

E.to(T, EE)
EE.to(A, T, EE)  \
    .to()
T.to(F, TT)
TT.to(M, F, TT)  \
    .to()
F.to(Token(TokenType.OP, OP.L_PAR), E, Token(TokenType.OP, OP.R_PAR))  \
    .to(TokenType.NUM)  \
    .to(TokenType.ID)
A.to(Token(TokenType.OP, OP.ADD))  \
    .to(Token(TokenType.OP, OP.MINUS))
M.to(Token(TokenType.OP, OP.MUL))  \
    .to(Token(TokenType.OP, OP.DIV))
