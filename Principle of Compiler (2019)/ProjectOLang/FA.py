# coding=utf-8
from __future__ import annotations
from typing import List, Set, Any, Union, NoReturn
import string

from utils import Epsilon

__all__ = ('NFAState', 'NFA', 'DFAState', 'DFA')


class NFAState(object):
    count = 0

    def __init__(self):
        self.id = NFAState.count
        self.map = {}
        NFAState.count += 1

    def map_to(self, to: NFAState, ch: str) -> NoReturn:
        if self.map.get(ch) is None:
            self.map[ch] = {to}
        else:
            self.map[ch].add(to)

    def transfer(self, ch) -> Set[NFAState]:
        return self.map[ch]

    def __repr__(self):
        return f"<NFAState S{self.id}>"

    def __str__(self):
        return f"S{self.id}"

    def __hash__(self):
        return hash(id(self))


class NFA(object):
    def __init__(self):
        self.q0 = None
        self.states = set()
        self.accept_states = set()
        self.chars = string.printable

    def new_state(self) -> NFAState:
        q = NFAState()
        self.states.add(q)

        if self.q0 is None:
            self.q0 = q

        return q

    def accept(self, q: NFAState) -> NoReturn:
        self.accept_states.add(q)


class DFAState(object):
    count = 0

    def __init__(self):
        self.id = DFAState.count
        self.map = {}
        DFAState.count += 1

    def map_to(self, to: DFAState, ch: str) -> NoReturn:
        assert self.map.get(ch) is None
        self.map[ch] = to

    def transfer(self, ch) -> DFAState:
        return self.map[ch]

    def __repr__(self):
        return f"<DFAState S{self.id}>"

    def __str__(self):
        return f"S{self.id}"

    def __hash__(self):
        return hash(id(self))


class DFA(object):
    def __init__(self, frm: NFA = None):
        self.q0 = None
        self.states = set()  # type: Set[DFAState]
        self.accept_states = set()  # type: Set[DFAState]

        if frm is not None:
            self.build_from_NFA(frm)

    @staticmethod
    def possible_next_chars(states: Set[NFAState]) -> Set[Any]:
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
                new_states: Set[NFAState] = self.edge(states[current], ch)
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

    def epsilon_closure(self, states: Union[List[NFAState], Set[NFAState]])  \
            -> Set[NFAState]:
        closure = set()
        for state in states:
            closure.add(state)
            temp_closure = state.map.get(Epsilon, None)
            if temp_closure is not None:
                closure.update(temp_closure)
                closure.update(self.epsilon_closure(temp_closure))
        return closure

    def edge(self, frm: Set[NFAState], ch: str) -> Set[NFAState]:
        to_states = set()
        for state in frm:
            temp_states = state.map.get(ch)  # type: Set[NFAState]
            if temp_states is not None:
                to_states.update(temp_states)

        return self.epsilon_closure(to_states)

    def new_state(self) -> DFAState:
        q = DFAState()
        self.states.add(q)
        return q

    def accept(self, q: DFAState) -> NoReturn:
        self.accept_states.add(q)
