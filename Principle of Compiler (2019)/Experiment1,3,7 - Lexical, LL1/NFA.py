# coding=utf-8
from __future__ import annotations
import string
from typing import NoReturn, Set

from draw import DotShow

__all__ = ('NFA_State', 'NFA')

Epsilon = ...


class NFA_State(object):
    count = 0
    def __init__(self):
        self.id = NFA_State.count
        self.map = {}
        NFA_State.count += 1

    def map_to(self, to: NFA_State, ch: str) -> NoReturn:
        if self.map.get(ch) is None:
            self.map[ch] = {to}
        else:
            self.map[ch].add(to)

    def transfer(self, ch) -> Set[NFA_State]:
        return self.map[ch]

    def __repr__(self):
        return "<NFA_State S{}>".format(self.id)

    def __hash__(self):
        return hash(id(self))


class NFA(DotShow):
    def __init__(self):
        self.q0 = None
        self.states = set()
        self.accept_states = set()
        self.chars = string.printable

    def new_state(self) -> NFA_State:
        q = NFA_State()
        self.states.add(q)

        if self.q0 is None:
            self.q0 = q

        return q

    def accept(self, q: NFA_State) -> NoReturn:
        self.accept_states.add(q)
