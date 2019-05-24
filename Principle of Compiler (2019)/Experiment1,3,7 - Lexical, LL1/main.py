# coding=utf-8

import argparse
import os
import sys
from pprint import pprint

from analyzer import Analyser
from idtable import IdentifierTable
from tokens import *
from regex import *
from saver import Saver

from LL1 import *

parser = argparse.ArgumentParser()

parser.add_argument('file', nargs='?', type=argparse.FileType('r'),
                    default=sys.stdin)

parser.add_argument('-o', '--output', type=str)
parser.add_argument('--demo', type=str)

args = parser.parse_args()


def demo_show_fa():
    pattern = Concat(
        AnyTimes(Alter(Match("a"), Match("b"))),
        Match("abb")
    )

    regex = pattern
    regex.compile()
    regex.show_nfa()
    regex.show_dfa()


def demo_load(args):
    assert args.output and os.path.isdir(args.output)

    idtable = IdentifierTable()
    tokens = []

    Saver(idtable).loadfrom(args.output)
    pprint(idtable)

    Saver(tokens).loadfrom(args.output)
    pprint(tokens)


def main(args):
    assert args.file
    assert args.output and os.path.isdir(args.output)

    analyser = Analyser()
    try:
        idtable, tokens = analyser.analyse(args.file.readlines())
    except SyntaxError:
        sys.exit(-1)

    with Language() as lang:
        E = Nonterminal('E')
        EE = Nonterminal("E'")
        T = Nonterminal('T')
        TT = Nonterminal("T'")
        F = Nonterminal('F')
        A = Nonterminal('A')
        M = Nonterminal('M')

        E.to(T, EE)
        EE.to(A, T, EE).to()
        T.to(F, TT)
        TT.to(M, F, TT).to()
        F.to(
            Token(TokenType.OP, OP.LEFT_PAR),
            E,
            Token(TokenType.OP, OP.RIGHT_PAR))  \
         .to(TokenType.NUM)
        A.to(Token(TokenType.OP, OP.ADD)).to(Token(TokenType.OP, OP.MINUS))
        M.to(Token(TokenType.OP, OP.MUL)).to(Token(TokenType.OP, OP.DIV))

        lang.preprocess()

    print(lang.analyze(tokens))

    # pprint(idtable)
    # Saver(idtable).saveto(args.output)
    #
    # pprint(tokens)
    # Saver(tokens).saveto(args.output)


if __name__ == '__main__':
    if args.demo:
        demo_mode = args.demo.lower().strip()

        if demo_mode == 'fa':
            demo_show_fa()

        elif demo_mode == 'load':
            demo_load(args)

        else:
            main(args)

    else:
        main(args)