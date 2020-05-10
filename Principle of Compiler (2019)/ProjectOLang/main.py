# coding=utf-8
import sys
import argparse

from utils import ShowFA
from pprint import pprint


# 定义命令行选项
parser = argparse.ArgumentParser()
parser.add_argument('--debug-off', action='store_false')
parser.add_argument('--demo-fa', action='store_true')
parser.add_argument('--demo-load', type=str)

subparsers = parser.add_subparsers(dest='mode')

parser_lexical = subparsers.add_parser('lexical')
parser_syntax = subparsers.add_parser('syntax')

parser_lexical.add_argument('--in', type=argparse.FileType('r'), required=True)
parser_lexical.add_argument('--out', type=str, required=True)

parser_syntax.add_argument('--lexical-load-from', type=str, required=True)
parser_syntax.add_argument('--production-load-from', type=str, required=True)
parser_syntax.add_argument('--lang', type=str, choices=['LL1', 'SLR1'],
                           required=True)
parser_syntax.add_argument('--out', type=str, required=True)

parsed = parser.parse_args()


def demo_fa(args):
    from regex import Concat, AnyTimes, Alter, Match

    pattern = Concat(
        AnyTimes(Alter(Match("a"), Match("b"))),
        Match("abb"))

    pattern.compile()
    pattern.show_nfa()
    pattern.show_dfa()


def demo_load(args):
    from utils import Loader

    pprint(Loader(args.demo_load, tp='idtable'))
    pprint(Loader(args.demo_load, tp='token_sequence'))


def mode_lexical(args):
    from utils import Saver
    from lexical import Lexical
    infile = args.__dict__['in']

    analyser = Lexical()
    try:
        idtable, tokens = analyser.analyse(infile.readlines())
    except SyntaxError:
        sys.exit(-1)

    if not args.debug_off:
        pprint(idtable)
        pprint(tokens)

    Saver(idtable, to=args.out)
    Saver(tokens, to=args.out)


def mode_syntax(args):
    from utils import Loader
    from syntax import LL1, SLR1

    lang_dict = dict(LL1=LL1, SLR1=SLR1)
    assert args.lang.upper() in lang_dict, "`lang` must be either LL1 or SLR1"

    with lang_dict.get(args.lang.upper())(args) as lang:
        __import__(args.production_load_from)
        lang.preprocess()

    if args.lang.upper() == 'LL1':
        result = lang.is_LL1()
        print('The language `{}` {} LL(1)'.format(
            args.production_load_from,
            'is' if result else "isn't"))
        if not result:
            exit(0)
    elif args.lang.upper() == 'SLR1':
        result = lang.is_SLR1()
        print('The language `{}` {} SLR(1)'.format(
            args.production_load_from,
            'is' if result else "isn't"))
        if not result:
            exit(0)

    if args.lang.upper() == 'SLR1':
        ShowFA(lang.dfa, 'SLR1-DFA')

    tokens = Loader(args.lexical_load_from, tp='token_sequence')
    pprint(lang.analyze(tokens))


if __name__ == '__main__':
    if not parsed.debug_off:
        pprint(parsed.__dict__)

    if parsed.demo_fa:
        demo_fa(parsed)

    elif parsed.demo_load:
        demo_load(parsed)

    elif parsed.mode == 'lexical':
        mode_lexical(parsed)

    elif parsed.mode == 'syntax':
        mode_syntax(parsed)

    else:
        print('No action has been taken, exiting.')
