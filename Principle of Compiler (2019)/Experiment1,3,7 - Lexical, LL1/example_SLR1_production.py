# coding=utf-8
from tokens import *
from language import *


# SS = Nonterminal("S'")
# S = Nonterminal('S')
# E = Nonterminal('E')
# T = Nonterminal('T')
# F = Nonterminal('F')
#
# SS.to(S)
# S.to(TokenType.ID, Token(TokenType.OP, OP.ASSIGN), E)
# E.to(E, Token(TokenType.OP, OP.ADD), T)
# E.to(E, Token(TokenType.OP, OP.MINUS), T)
# E.to(T)
# T.to(T, Token(TokenType.OP, OP.MUL), F)
# T.to(T, Token(TokenType.OP, OP.DIV), F)
# T.to(F)
# F.to(Token(TokenType.OP, OP.L_PAR), E, Token(TokenType.OP, OP.R_PAR))
# F.to(TokenType.NUM)
# F.to(TokenType.ID)

SS = Nonterminal("S'")
E = Nonterminal("E")
T = Nonterminal("T")
F = Nonterminal("F")

SS.to(E)
E.to(E, Token(TokenType.OP, OP.ADD), T)
E.to(T)
T.to(T, Token(TokenType.OP, OP.MUL), F)
T.to(F)
F.to(Token(TokenType.OP, OP.L_PAR), E, Token(TokenType.OP, OP.R_PAR))
F.to(TokenType.ID)
