# coding=utf-8
from __future__ import annotations
from typing import Optional, Dict, Tuple, List, Any, overload, Set

from tokens import TokenType

__all__ = ('Nonterminal', 'End', 'Language',)

context = None  # type: Optional[Language]


def context_checker(func):
    def wrapper(self, *args, **kwargs):
        assert context is not None, \
            "call me inside a `with Language()` statement"
        return func(self, *args, **kwargs)
    return wrapper


class Nonterminal(object):
    @context_checker
    def __init__(self, name: str):
        self.name = name
        context.add_nonterminal(self)

    def __repr__(self):
        return "<Nonterminal:{}>".format(self.name)

    @context_checker
    def to(self, *args) -> Nonterminal:
        context.add_production(self, args)
        return self


class SpecialSymbol(object):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return '<SpecialSymbol:{}>'.format(self.value)

    def __str__(self):
        return str(self.value)

End = SpecialSymbol('$')


class Language(object):
    def __init__(self):
        self.startfrom = None
        self.nonterminals = []
        self.productions = {}  # type: Dict[Nonterminal, List[Tuple]]

        self.first_set = None
        self.follow_set = None  # type: Optional[Dict[Nonterminal, Set[Any]]]
        self.parsing_table: Optional[
            Dict[Nonterminal,
                 Dict[Any,
                      Tuple[Nonterminal, List[Tuple]]
                 ]
            ]
        ] = None

    def add_nonterminal(self, nt: Nonterminal) -> None:
        if not len(self.nonterminals):
            self.startfrom = nt
        self.nonterminals.append(nt)

    def add_production(self, frm: Nonterminal, seq: tuple) -> None:
        list = self.productions.get(frm)
        if list is None:
            self.productions[frm] = [seq]
        else:
            self.productions[frm].append(seq)

    def preprocess(self) -> None:
        """A series of operations, assembled for convenience"""
        self.remove_left_recursion()
        self.do_left_factoring()
        self.calc_follow()
        self.calc_select()
        if self.is_LL1():
            self.calc_parsing_table()

    @context_checker
    def remove_left_recursion(self) -> None:
        def remove_direct_left_recursion(nt: Nonterminal) -> None:
            recursions = []
            nonrecursions = []

            for production in self.productions[nt]:
                if len(production) and production[0] == nt:
                    recursions.append(production)
                else:
                    nonrecursions.append(production)

            if not len(recursions):
                return

            new_nt = Nonterminal(nt.name + "'")
            self.productions[nt] = list(map(lambda x: x + (new_nt,),
                                            nonrecursions))

            self.productions[new_nt] = [()]  # 先把空集加上
            for production in recursions:
                self.productions[new_nt].append(production[1:] + (new_nt,))

        for i, i_nt in enumerate(self.nonterminals):
            for j in range(i):
                j_nt = self.nonterminals[j]
                assert self.productions.get(i_nt) is not None, \
                    "undefined nonterminal {}".format(i_nt)
                new_productions = []
                for production in self.productions[i_nt]:
                    if len(production) and production[0] == j_nt:
                        for inner_production in self.productions[j_nt]:
                            new_productions.append(
                                inner_production + production[1:])
                    else:
                        new_productions.append(production)
                self.productions[i_nt] = new_productions

            remove_direct_left_recursion(i_nt)

    @context_checker
    def do_left_factoring(self) -> None:
        for nt in self.nonterminals:
            catalogues = {}
            new_productions = []
            for production in self.productions[nt]:
                if len(production):
                    if production[0] in catalogues:
                        catalogues[production[0]].append(production)
                    else:
                        catalogues[production[0]] = [production]
                else:
                    new_productions.append(production)

            for key in catalogues:
                if len(catalogues[key]) == 1:
                    new_productions.append(catalogues[key][0])
                else:
                    new_nt = Nonterminal('{}_startwith_{}'.format(nt.name, key))
                    new_productions.append((key, new_nt))
                    self.productions[new_nt] = list(map(lambda x: x[1:],
                                                        catalogues[key]))

            self.productions[nt] = new_productions

    @overload
    def first(self, seq: Tuple[Any]) -> Set[Any]:
        ...
    @overload
    def first(self, seq: Any) -> Set[Any]:
        ...
    def first(self, seq):
        if not isinstance(seq, tuple):
            seq = (seq,)
        ret = set()

        for component in seq:
            if not isinstance(component, Nonterminal):
                ret.add(component)
                break

            for production in self.productions[component]:
                if not len(production):
                    ret.add(())
                else:
                    for part in production:
                        sub_first_set = self.first(part)
                        ret.update(sub_first_set - {()})
                        if () not in sub_first_set:
                            break

            if () not in self.productions[component]:
                break

        return ret

    def calc_follow(self) -> None:
        self.follow_set = {k: set() for k in self.nonterminals}
        self.follow_set[self.startfrom].add(End)

        is_modified = True
        while is_modified:
            is_modified = False
            for nt in self.nonterminals:
                for production in self.productions[nt]:
                    for n, component in enumerate(production):
                        if isinstance(component, Nonterminal):
                            temp_first = self.first(production[n + 1:])
                            if not self.follow_set[component].issuperset(
                                    temp_first - {()}):
                                self.follow_set[component].update(
                                    temp_first - {()})
                                is_modified = True
                            if len(temp_first) == 0 or () in temp_first:
                                self.follow_set[component].update(
                                    self.follow_set[nt])

    def follow(self, target_nt: Nonterminal) -> Set[Any]:
        if self.follow_set is not None:
            return self.follow_set[target_nt]

        self.calc_follow()
        return self.follow_set[target_nt]

    def calc_select(self) -> None:
        self.select_set = {}
        for nt in self.nonterminals:
            for production in self.productions[nt]:
                key = (nt, production)
                temp_first = self.first(production)
                self.select_set[key] = temp_first - {()}
                if len(temp_first) == 0 or () in temp_first:
                    self.select_set[key].update(self.follow(nt))

    def select(self, frm: Nonterminal, to: Tuple[Any]) -> Set[Any]:
        return self.select_set[(frm, to)]

    def is_LL1(self) -> bool:
        for nt, productions in self.productions.items():
            for production1 in productions:
                for production2 in productions:
                    if production1 == production2:
                        continue
                    if self.select(nt, production1).intersection(
                            self.select(nt, production2)):
                        return False
        return True

    def calc_parsing_table(self) -> None:
        assert self.is_LL1()

        self.parsing_table = {nt: {} for nt in self.nonterminals}
        for nt, productions in self.productions.items():
            for production in productions:
                select_set = self.select(nt, production)
                for i in select_set:
                    self.parsing_table[nt][i] = (nt, production)

    def analyze(self, seq: List[Any]) -> bool:
        assert self.parsing_table is not None
        table = self.parsing_table

        stack = [End, self.startfrom]
        seq = seq.copy()
        seq.append(End)
        at = 0

        print('{:^70} | {:^70} | {:^25}'  \
              .format('STACK', 'SEQUENCE', 'PRODUCTION'))

        def report_status(at, nt=None, production=None):
            _stack = []
            for component in reversed(stack):
                if isinstance(component, Nonterminal):
                    _stack.append(component.name)
                else:
                    _stack.append(str(component))
            _stack = ' '.join(_stack)

            _seq = []
            for component in seq[at:]:
                if isinstance(component, Nonterminal):
                    _seq.append(component.name)
                else:
                    _seq.append(str(component))
            _seq = ' '.join(_seq)

            if nt:
                _production = []
                for component in production:
                    if isinstance(component, Nonterminal):
                        _production.append(component.name)
                    else:
                        _production.append(str(component))
                _production = nt.name + ' -> ' + ' '.join(_production)
            else:
                _production = ''

            print('{:>70} | {:<70} | {:<25}'.format(_stack, _seq, _production))

        nt = None
        production = None
        while not (stack[-1] == End and seq[at] == End):
            report_status(at, nt, production)
            if isinstance(stack[-1], Nonterminal):
                result = self.parsing_table[stack[-1]].get(seq[at])
                if result is None:
                    result = self.parsing_table[stack[-1]].get(seq[at].tp)
                    if result is None:
                        print('=' * 96)
                        print('Analysis Error: unexpected symbol: {}' \
                              .format(seq[at]))
                        return False
                nt, production = result
                stack.pop()
                for component in reversed(production):
                    stack.append(component)
            else:
                nt = None
                production = None
                if stack[-1] == seq[at]  \
                        or (isinstance(stack[-1], TokenType)
                            and seq[at].tp == stack[-1]):
                    stack.pop()
                    at += 1
                else:
                    print('=' * 96)
                    print('Analysis Error: unexpected symbol: {}' \
                          .format(seq[at]))
                    return False

        report_status(at)
        return True

    def __enter__(self):
        global context
        assert context is None, \
            "only one Language instance is allowed at a time"

        context = self
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        global context
        assert context is not None

        context = None
