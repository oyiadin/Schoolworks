# coding=utf-8
from __future__ import annotations
from typing import List, Set, overload, NoReturn, Any

from NFA import *
from draw import DotShow

__all__ = ('DFA_State', 'DFA')

Epsilon = ...


class DFA_State(object):
    count = 0
    def __init__(self):
        self.id = DFA_State.count
        self.map = {}
        DFA_State.count += 1

    def map_to(self, to: DFA_State, ch: str) -> NoReturn:
        assert self.map.get(ch) is None
        self.map[ch] = to

    def transfer(self, ch) -> DFA_State:
        return self.map[ch]

    def __repr__(self):
        return "<DFA_State S{}>".format(self.id)

    def __hash__(self):
        return hash(id(self))


class DFA(DotShow):
    def __init__(self, frm: NFA = None):
        self.q0 = None
        self.states = set()  # type: Set[DFA_State]
        self.accept_states = set()  # type: Set[DFA_State]

        if frm is not None:
            self.build_from_NFA(frm)

    def possible_next_chars(self, states: Set[NFA_State]) -> Set[Any]:
        ret = set()
        for state in states:
            ret.update(state.map.keys())
        ret.discard(Epsilon)
        return ret

    def build_from_NFA(self, nfa: NFA) -> NoReturn:
        self.q0 = self.new_state()
        self.states.add(self.q0)

        states = [self.epsilon_closure({nfa.q0})]
        dfa_states = [self.q0]

        total = 1
        current = 0

        while current < total:
            for ch in self.possible_next_chars(states[current]):
                new_states = self.edge(states[current], ch)  # type: Set[NFA_State]
                if new_states == set():
                    continue  # 如果是空集，直接跳过
                for i in range(total):
                    if new_states == states[i]:  # 如果该集合已存在
                        dfa_states[current].map_to(dfa_states[i], ch)
                        break
                else:
                    states.append(new_states)
                    new_dfa_state = self.new_state()
                    dfa_states.append(new_dfa_state)
                    dfa_states[current].map_to(new_dfa_state, ch)
                    total += 1
                    # 更新 DFA 的接受状态集
                    for nfa_state in new_states:
                        if nfa_state in nfa.accept_states:
                            self.accept(new_dfa_state)
                            break
            current += 1

    @overload
    def epsilon_closure(self, states: List[NFA_State]) -> Set[NFA_State]:
        ...
    @overload
    def epsilon_closure(self, states: Set[NFA_State]) -> Set[NFA_State]:
        ...
    def epsilon_closure(self, states):
        closure = set()
        for state in states:
            closure.add(state)
            temp_closure = state.map.get(Epsilon, None)
            if temp_closure is not None:
                closure.update(temp_closure)
                closure.update(self.epsilon_closure(temp_closure))
        return closure

    def edge(self, frm: Set[NFA_State], ch: str) -> Set[NFA_State]:
        to_states = set()
        for state in frm:
            temp_states = state.map.get(ch)  # type: Set[NFA_State]
            if temp_states is not None:
                to_states.update(temp_states)

        return self.epsilon_closure(to_states)


    def new_state(self) -> DFA_State:
        q = DFA_State()
        self.states.add(q)
        return q

    def accept(self, q: DFA_State) -> NoReturn:
        self.accept_states.add(q)

    def _show__draw_connections(self, dot) -> NoReturn:
        for frm in self.states:
            for ch in frm.map:
                dot.edge('S{}'.format(frm.id),
                         'S{}'.format(frm.map[ch].id),
                         label=str(ch) if ch is not Epsilon else 'ε')
