# coding=utf-8

import argparse
import os
import sys
from pprint import pprint

from analyzer import Analyser
from idtable import IdentifierTable
from regex import *
from saver import Saver
from language import *


parser = argparse.ArgumentParser()
parser.add_argument('--debug-off', action='store_false')
parser.add_argument('--demo-fa', action='store_true')
parser.add_argument('--demo-load', type=str)

subparsers = parser.add_subparsers(dest='mode')

parser_lexical = subparsers.add_parser('lexical')
parser_syntax = subparsers.add_parser('syntax')
parser_semantic = subparsers.add_parser('semantic')

parser_lexical.add_argument('--in', type=argparse.FileType('r'), required=True)
parser_lexical.add_argument('--out', type=str, required=True)

parser_syntax.add_argument('--lexical-load-from', type=str, required=True)
parser_syntax.add_argument('--production-load-from', type=str, required=True)
parser_syntax.add_argument('--lang', type=str, choices=['LL1', 'SLR1'],
                           required=True)

args = parser.parse_args()


def demo_fa(args):
    pattern = Concat(
        AnyTimes(Alter(Match("a"), Match("b"))),
        Match("abb")
    )

    pattern.compile()
    pattern.show_nfa()
    pattern.show_dfa()


def demo_load(args):
    path = args.demo_load

    idtable = IdentifierTable()
    tokens = []

    Saver(idtable).loadfrom(path)
    pprint(idtable)

    Saver(tokens).loadfrom(path)
    pprint(tokens)


def mode_lexical(args):
    infile = args.__dict__['in']

    analyser = Analyser()
    try:
        idtable, tokens = analyser.analyse(infile.readlines())
    except SyntaxError:
        sys.exit(-1)

    if not args.debug_off:
        pprint(idtable)
        pprint(tokens)

    Saver(idtable).saveto(args.out)
    Saver(tokens).saveto(args.out)


def mode_syntax(args):
    assert args.lexical_load_from and args.lang and args.production_load_from

    lang_dict = dict(
        LL1=LL1,
        SLR1=SLR1
    )

    assert args.lang.upper() in lang_dict

    with lang_dict.get(args.lang.upper())() as lang:
        __import__(args.production_load_from)
        lang.preprocess()

    if args.lang.upper() == 'LL1':
        print('The language `{}` {} LL(1)'  \
              .format(args.production_load_from,
                      'is' if lang.is_LL1() else "isn't"))
    elif args.lang.upper() == 'SLR1':
        print('The language `{}` {} SLR(1)' \
              .format(args.production_load_from,
                      'is' if lang.is_SLR1() else "isn't"))

    tokens = []
    Saver(tokens).loadfrom(args.lexical_load_from)

    pprint(lang.analyze(tokens))

    if args.lang.upper() == 'SLR1':
        lang.nfa.show()
        input('Showing NFA, press Enter to continue >>> ')
        lang.dfa.show()
        input('Showing DFA, press Enter to continue >>> ')


def mode_semantic(args):
    pass


if __name__ == '__main__':
    if not args.debug_off:
        print(args)

    if args.demo_fa:
        demo_fa(args)

    elif args.demo_load:
        demo_load(args)

    elif args.mode == 'lexical':
        mode_lexical(args)

    elif args.mode == 'syntax':
        mode_syntax(args)

    elif args.mode == 'semantic':
        mode_semantic(args)
