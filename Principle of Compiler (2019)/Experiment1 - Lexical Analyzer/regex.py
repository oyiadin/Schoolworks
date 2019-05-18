# coding=utf-8
from typing import List, Tuple, overload

from NFA import *
from DFA import *


__all__ = (
    'RegEx',
    'Match', 'Concat', 'Alter', 'AnyTimes', 'AtLeastOnce', 'Epsilon',
    'MatchAny', 'MatchAnyBut'
)

Epsilon = ...


def must_compiled(func):
    def wrapper(self, *args, **kwargs):
        assert self.is_compiled, "Must compile first"
        return func(self, *args, **kwargs)
    return wrapper


class RegEx(object):
    def __init__(self):
        self.nfa = None
        self.dfa = None
        self.is_compiled = False

    def _compile(self, frm: NFA_State, nfa: NFA):
        raise NotImplementedError

    def compile(self):
        # build NFA
        self.nfa = NFA()
        to = self._compile(frm=self.nfa.q0, nfa=self.nfa)
        self.nfa.accept(to)
        # construct a DFA from NFA
        self.dfa = DFA(self.nfa)

        self.is_compiled = True

        return self  # makes `pattern = Match(xxx).compile()` to be legal

    @must_compiled
    def show_nfa(self):
        self.nfa.show()
        input('Showing NFA, press Enter to continue. >>> ')

    @must_compiled
    def show_dfa(self):
        self.dfa.show()
        input('Showing DFA, press Enter to continue. >>> ')

    @must_compiled
    def matchstart(self, content: str) -> int:
        q = _q = self.dfa.q0

        print('Start from {}'.format(q))
        n = -1
        ch = None
        try:
            for n, ch in enumerate(content):
                q = _q.transfer(ch)
                _q = q
                print("`{}` ==> {}".format(ch, q))
        except:
            print("No proper way to handle '{}', checking if I can stop now" \
                  .format(ch))
            n -= 1

        n += 1
        if q in self.dfa.accept_states:
            print("{} is one of the acceptable states, stopped.".format(q))
            return n
        print("{} is unacceptable, failed".format(q))
        return -1

    @must_compiled
    def match(self, content: str) -> bool:
        q = _q = self.dfa.q0

        print('Start from {}'.format(q))
        ch = None
        try:
            for ch in content:
                q = _q.transfer(ch)
                _q = q
                print("`{}` ==> {}".format(ch, q))
        except:
            print("No proper way to handle '{}', stopped.".format(ch))
            return False

        if q in self.dfa.accept_states:
            print("{} is one of the acceptable states, stopped.".format(q))
            return True
        return False


class Match(RegEx):
    def __init__(self, exp: str):
        super().__init__()
        self.exp = exp

    def _compile(self, frm: NFA_State, nfa: NFA):
        q = _q = frm
        for ch in self.exp:
            q = nfa.new_state()
            _q.map_to(q, ch)
            _q = q
        to = q

        return to


class Concat(RegEx):
    def __init__(self, *exps: RegEx):
        super().__init__()
        self.exps = exps

    def _compile(self, frm: NFA_State, nfa: NFA):
        to = None
        for exp in self.exps:
            to = exp._compile(frm=frm, nfa=nfa)
            frm = to

        return to


class Alter(RegEx):
    def __init__(self, *exps: RegEx):
        super().__init__()
        self.exps = exps

    def _compile(self, frm: NFA_State, nfa: NFA):
        to = nfa.new_state()
        for inner_exp in self.exps:
            inner_frm = nfa.new_state()
            inner_to = inner_exp._compile(frm=inner_frm, nfa=nfa)
            frm.map_to(inner_frm, Epsilon)
            inner_to.map_to(to, Epsilon)

        return to


class AtLeastOnce(RegEx):
    def __init__(self, exp: RegEx):
        super().__init__()
        self.exp = exp

    def _compile(self, frm: NFA_State, nfa: NFA):
        frm_inner = nfa.new_state()
        to_inner = self.exp._compile(frm_inner, nfa=nfa)
        to = nfa.new_state()

        frm.map_to(frm_inner, Epsilon)
        to_inner.map_to(to, Epsilon)
        to_inner.map_to(frm_inner, Epsilon)

        return to


class AnyTimes(AtLeastOnce):
    def __init__(self, exp: RegEx):
        super().__init__(exp)

    def _compile(self, frm: NFA_State, nfa: NFA):
        to = super()._compile(frm=frm, nfa=nfa)
        frm.map_to(to, Epsilon)

        return to


class MatchAny(RegEx):
    def __init__(self):
        super().__init__()

    def _compile(self, frm: NFA_State, nfa: NFA):
        to = nfa.new_state()
        for ch in nfa.chars:
            frm.map_to(to, ch)

        return to


class MatchAnyBut(RegEx):
    def __init__(self, but: str):
        super().__init__()
        self.but = but

    def _compile(self, frm: NFA_State, nfa: NFA):
        to = nfa.new_state()
        for ch in nfa.chars:
            if ch != self.but:
                frm.map_to(to, ch)

        return to