# coding=utf-8
from __future__ import annotations

import os
from html import escape
from typing import Optional, Dict, Tuple, List, Any, Set, NoReturn, Union

from FA import DFA, DFAState
from tokens import TokenType
from utils import End, MatchAny

__all__ = ('Nonterminal', 'LL1', 'SLR1')

context = None  # type: Optional[LL1]


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

    def __str__(self):
        return "NT:{}".format(self.name)

    def __repr__(self):
        return "<Non-Terminal:{}>".format(self.name)

    @context_checker
    def to(self, *args) -> Nonterminal:
        context.add_production(self, args)
        return self


class Language(object):
    def __init__(self, args):
        self.args = args
        self.startfrom = None  # type: Optional[Nonterminal]
        self.nonterminals = []  # type: List[Nonterminal]
        self.productions = {}  # type: Dict[Nonterminal, List[Tuple[Any]]]

        self.possible_components = set()  # type: Set[Any]

        self.first_set = None
        self.follow_set = None  # type: Optional[Dict[Nonterminal, Set[Any]]]

    def add_nonterminal(self, nt: Nonterminal) -> NoReturn:
        if self.startfrom is None:
            self.startfrom = nt
        self.nonterminals.append(nt)
        self.possible_components.add(nt)

    def add_production(self, frm: Nonterminal, seq: Tuple[Any]) -> NoReturn:
        if self.productions.get(frm) is None:
            self.productions[frm] = [seq]
        else:
            self.productions[frm].append(seq)

        for component in seq:
            self.possible_components.add(component)

    @context_checker
    def remove_left_recursion(self) -> None:
        def remove_direct_left_recursion(nt: Nonterminal) -> NoReturn:
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

    def first(self, seq: Union[Tuple[Any], Any]) -> Set[Any]:
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

    def __enter__(self):
        global context
        assert context is None, \
            "only one Language(LL1, SLR1) instance is allowed at a time"

        context = self
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        global context
        assert context is not None

        context = None


class LL1(Language):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.select_set: Dict[Tuple[Nonterminal, Tuple[Any]], Set[Any]] = {}
        self.parsing_table: Optional[
            Dict[Nonterminal,
                 Dict[Any,
                      Tuple[Nonterminal, List[Tuple]]]]] = None

    def preprocess(self) -> NoReturn:
        """A series of operations, assembled for convenience"""
        self.remove_left_recursion()
        self.do_left_factoring()
        self.calc_follow()
        self.calc_select()
        if self.is_LL1():
            self.calc_parsing_table()
            self.show_parsing_table()

    def calc_select(self) -> NoReturn:
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
                    # 判断是否有交集
                    if self.select(nt, production1).intersection(
                            self.select(nt, production2)):
                        return False
        return True

    def calc_parsing_table(self) -> NoReturn:
        assert self.is_LL1()

        self.parsing_table = {nt: {} for nt in self.nonterminals}
        for nt, productions in self.productions.items():
            for production in productions:
                select_set = self.select(nt, production)
                for i in select_set:
                    self.parsing_table[nt][i] = (nt, production)

    def show_parsing_table(self) -> NoReturn:
        assert self.parsing_table is not None, "call calc_parsing_table() first"
        components = list(filter(
            lambda x: not isinstance(x, Nonterminal),
            self.possible_components))
        components.append(End)

        def save_as_html():
            filename = "parsing_table.html"
            with open(os.path.join(self.args.out, filename), "w") as fw:
                fw.write("<style>th,td{border: 1px solid black;}</style>\n")
                lines = ['<table>', '<tr><th></th>']

                for comp in components:
                    lines.append(f'<th>{comp!s}</th>')
                lines.append('</tr>')

                for nt in self.nonterminals:
                    lines.append(f'<tr><td>{nt!s}</td>')
                    for comp in components:
                        got = self.parsing_table[nt].get(MatchAny, '') \
                              or self.parsing_table[nt].get(comp, '')
                        if got:
                            got = '{!s} => {}'.format(
                                got[0],
                                ' '.join(map(str, got[1])))
                        lines.append(f'<td>{escape(got)}</td>')
                    lines.append('</tr>')

                lines.append('</tr></table>')
                fw.writelines(lines)

            print(f'visiting file://{os.path.abspath(filename)}'
                  f' to see the parsing table')

        def save_as_latex():
            filename = "parsing_table.tex"
            with open(os.path.join(self.args.out, filename), "w") as fw:
                fw.write("\\resizebox{\\textwidth}{!}{\n")
                fw.write("\\begin{tabular}{")
                cs = ['c']
                for _ in components:
                    cs.append('c')
                fw.write(f"|{'|'.join(cs)}|}}\n")
                fw.write('\\hline\n')

                lines = []
                comps = ['']
                for comp in components:
                    comps.append(str(comp))
                lines.append('&'.join(comps))

                for nt in self.nonterminals:
                    comps = [str(nt)]
                    for comp in components:
                        got = self.parsing_table[nt].get(MatchAny, '') \
                              or self.parsing_table[nt].get(comp, '')
                        if got:
                            got = '{} => {}'.format(
                                str(got[0]),
                                ' '.join(map(str, got[1])))
                        comps.append(f"{got}")
                    lines.append('&'.join(comps))

                lines.append('')
                fw.write('\\\\\n\\hline\n'.join(lines)
                         .replace('_', r'\_')
                         .replace('$', r'\$'))
                fw.write('\\end{tabular}}\n')

        save_as_html()
        save_as_latex()

    def analyze(self, seq: List[Any]) -> bool:
        assert self.parsing_table is not None

        stack = [End, self.startfrom]
        seq = seq.copy()
        seq.append(End)
        at = 0

        print(f'{"-"*70}-+-{"-"*70}-+-{"-"*70}')
        print(f'{"STACK":^70} | {"SEQUENCE":^70} | {"PRODUCTION":^25}')
        print(f'{"-"*70}-+-{"-"*70}-+-{"-"*70}')

        def report_status(at, nt=None, production=None):
            _stack = ' '.join(map(str, reversed(stack)))
            _seq = ' '.join(map(str, seq[at:]))

            if nt:
                _production = nt.name + ' => ' + ' '.join(map(str, production))
            else:
                _production = ''

            print(f'{_stack:>70} | {_seq:<70} | {_production:<25}')

        nt = None
        production = None
        while not (stack[-1] == End and seq[at] == End):
            report_status(at, nt, production)
            if isinstance(stack[-1], Nonterminal):
                result = self.parsing_table[stack[-1]].get(seq[at])
                if result is None:
                    result = self.parsing_table[stack[-1]].get(seq[at].tp)
                    if result is None:
                        print('=' * 171)
                        print('Analysis Error: unexpected symbol: {}'
                              .format(repr(seq[at])))
                        return False
                nt, production = result
                stack.pop()
                for component in reversed(production):
                    stack.append(component)
            else:
                nt = None
                production = None
                if stack[-1] == seq[at] \
                        or (isinstance(stack[-1], TokenType)
                            and seq[at].tp == stack[-1]):
                    stack.pop()
                    at += 1
                else:
                    print('=' * 171)
                    print(f'Analysis Error: unexpected symbol: {seq[at]!r}')
                    return False

        report_status(at)
        return True


class SLR1Item(object):
    def __init__(self, nt: Nonterminal, seq: Tuple[Any], at: int):
        self.nt = nt
        self.seq = seq
        self.at = at

    def __eq__(self, other):
        if isinstance(other, SLR1Item):
            return self.nt == other.nt \
                   and self.seq == other.seq \
                   and self.at == other.at
        return False

    def __hash__(self):
        return hash(self.__repr__())

    def __repr__(self):
        return "<Item at {}, {!s} => {}>".format(
            self.at,
            self.nt,
            ', '.join(map(str, self.seq)))


class LRAction(object):
    def __init__(self, tp: str, value: Any = None):
        tp = tp.lower()
        assert tp in ('shift', 'reduce', 'accept'), \
            "argument `tp` must be one of: 'shift', 'reduce' and 'accept'"
        self.tp = tp
        self.value = value

    def is_shift(self):
        return self.tp == 'shift'

    def is_reduce(self):
        return self.tp == 'reduce'

    def is_accept(self):
        return self.tp == 'accept'

    def __repr__(self):
        if self.is_shift():
            return "<Shift S{}>".format(self.value.id)
        elif self.is_reduce():
            return "<Reduce {!s} => {}>".format(
                self.value['frm'],
                ', '.join(map(str, self.value['to'])))
        elif self.is_accept():
            return "<Accept>"
        else:
            return "<ERROR>"


class SLR1(Language):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.items = None  # type: Optional[Dict[Nonterminal, List[SLR1Item]]]
        # The elements of the list must be `at` ascending
        self.dfa = None  # type: Optional[DFA]

        self.groups = None
        self.states = None

        self.action_table: Dict[DFAState, Dict[Any, LRAction]] = {}
        self.goto_table: Dict[DFAState, Dict[Any, DFAState]] = {}

    def is_SLR1(self):
        for group in self.groups:
            reduce_follow = []
            shift_follow = set()
            for item in group:
                if item.at == len(item.seq):
                    reduce_follow.append(self.follow(item.nt))
                else:
                    shift_follow.add(item.seq[item.at])

            follow = reduce_follow + [shift_follow]

            for subset1 in follow:
                for subset2 in follow:
                    if subset1 is not subset2:
                        if subset1.intersection(subset2):
                            # 如果归约、移进时 Follow 集有交集，则不是 SLR1 文法
                            return False

        return True

    def preprocess(self) -> NoReturn:
        # self.remove_left_recursion()
        # self.do_left_factoring()
        self.init_items()
        self.calc_follow()
        self.build()

    def init_items(self) -> NoReturn:
        self.items = {nt: [] for nt in self.nonterminals}
        for nt in self.nonterminals:
            for production in self.productions[nt]:
                for at in range(0, len(production) + 1):
                    self.items[nt].append(SLR1Item(nt, production, at))

    def closure(self, items: Union[Set[SLR1Item], List[SLR1Item]]) \
            -> Set[SLR1Item]:
        ret = list(items)
        at = 0
        while at < len(ret):
            item = ret[at]
            if item.at < len(item.seq):
                next_component = item.seq[item.at]
                if isinstance(next_component, Nonterminal):
                    for inner_item in self.items[next_component]:
                        if inner_item.at == 0 and inner_item not in ret:
                            ret.append(inner_item)
            at += 1
        return set(ret)

    def go(self, items: Set[SLR1Item], via: Any) -> Set[SLR1Item]:
        ret = set()
        for item in items:
            if item.at < len(item.seq) and item.seq[item.at] == via:
                ret.add(SLR1Item(item.nt, item.seq, item.at + 1))

        return self.closure(ret)

    def build(self):
        self.action_table: Dict[DFAState, Dict[Any, LRAction]] = {}
        self.goto_table: Dict[DFAState, Dict[Any, DFAState]] = {}

        self.dfa = DFA()
        item0 = self.items[self.nonterminals[0]][0]
        item_acc = SLR1Item(item0.nt, item0.seq, len(item0.seq))
        I0 = self.closure({item0})
        groups = [I0]
        self.dfa.q0 = self.dfa.new_state()
        states = [self.dfa.q0]
        at = 0

        while at < len(groups):
            if states[at] not in self.action_table:
                self.action_table[states[at]] = {}
            if states[at] not in self.goto_table:
                self.goto_table[states[at]] = {}

            for item in groups[at]:
                if item.at == len(item.seq):
                    for via in self.follow(item.nt):
                        self.action_table[states[at]][via] = \
                            LRAction('reduce', dict(frm=item.nt, to=item.seq))

            # 把 accept 放 reduce 后边，不然 accept 会被覆盖了
            if item_acc in groups[at]:
                self.action_table[states[at]][End] = LRAction('accept')
                self.dfa.accept(states[at])

            for via in self.possible_components:
                result = self.go(groups[at], via)
                if result:
                    if result not in groups:
                        groups.append(result)
                        to_state = self.dfa.new_state()
                        states.append(to_state)
                    else:
                        to_state = states[groups.index(result)]

                    states[at].map_to(to_state, via)
                    if isinstance(via, Nonterminal):
                        self.goto_table[states[at]][via] = to_state
                    else:
                        self.action_table[states[at]][via] = \
                            LRAction('shift', to_state)
            at += 1

        self.groups = groups
        self.states = states

    def analyze(self, seq: List[Any]) -> bool:
        seq = seq + [End]
        states = [self.dfa.q0]
        symbols = [End]

        print('{}-+-{}-+-{}-+-{}-+-{}'.format(
            '-' * 25, '-' * 50, '-' * 45, '-' * 35, '-' * 10))
        print('{:^25} | {:^50} | {:^45} | {:^35} | {:^10}'.format(
            'STATES STACK', 'SYMBOLS STACK', 'SEQUENCE', 'ACTION', 'GOTO'))
        print('{}-+-{}-+-{}-+-{}-+-{}'.format(
            '-' * 25, '-' * 50, '-' * 45, '-' * 35, '-' * 10))

        def report_status(at, action=None, goto=None):
            _states = ' '.join([str(i.id) for i in states])
            _symbols = ' '.join([str(i) for i in symbols])
            _seq = ' '.join([str(i) for i in seq[at:]])
            _action = str(action) if action is not None else ''
            _goto = 'S{}'.format(str(goto.id)) if goto is not None else ''
            print('{:<25} | {:<50} | {:>45} | {:<35} | {:<10}'.format(
                _states, _symbols, _seq, _action, _goto))

        at = -1
        while True:
            at += 1
            if at >= len(seq):
                print('=' * 177)
                print('Unexpected EOF.')
                return False

            try:
                candidates = self.action_table[states[-1]]
                for key in candidates:
                    if key == seq[at] \
                            or (isinstance(key, TokenType)
                                and seq[at] is not End and seq[at].tp == key):
                        action = candidates[key]
                        break
                else:
                    raise KeyError
            except KeyError:
                action = self.action_table[states[-1]].get(MatchAny, None)
                if action is None:
                    print('=' * 177)
                    if seq[at] is End:
                        print('Unexpected EOF.')
                    else:
                        print('Unexpected symbol: {}'.format(seq[at]))
                    return False

            if action.is_shift():
                report_status(at, action)
                states.append(action.value)
                symbols.append(seq[at])

            elif action.is_reduce():
                idx = -(len(action.value['to']) + 1)
                report_status(at, action,
                              self.goto_table[states[idx]].get(
                                  action.value['frm']))
                if action.value['to']:  # 防止误处理 A => Epsilon 这种情况
                    del states[-len(action.value['to']):]
                    del symbols[-len(action.value['to']):]
                symbols.append(action.value['frm'])
                states.append(self.goto_table[states[-1]][symbols[-1]])

                at -= 1

            elif action.is_accept():
                report_status(at, action)
                return True
