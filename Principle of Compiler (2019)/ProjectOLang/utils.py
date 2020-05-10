# coding=utf-8
import enum
import os
import struct
from typing import List, Union

from tokens import *


__all__ = ('SpecialSymbol', 'End', 'Epsilon', 'MatchAny', 'ShowFA',
           'IdentifierTable', 'Saver', 'Loader')


class SpecialSymbol(object):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return self.value


End = SpecialSymbol('$')
Epsilon = SpecialSymbol('ε')
MatchAny = SpecialSymbol('*')


def ShowFA(fa, filename=None):
    from graphviz import Digraph
    from FA import NFA, DFA

    assert isinstance(fa, (DFA, NFA))
    dot = Digraph(comment='The Automaton Illustration')

    for q in fa.states:  # 绘制顶点
        node = 'S{}'.format(q.id)
        if q in fa.accept_states:
            dot.node(node, shape='box')
        else:
            dot.node(node)

    # 绘制边
    if isinstance(fa, NFA):
        for frm in fa.states:
            for ch in frm.map:
                for to in frm.map[ch]:
                    dot.edge('S{}'.format(frm.id),
                             'S{}'.format(to.id),
                             label=str(ch) if ch is not Epsilon else 'ε')
    elif isinstance(fa, DFA):
        for frm in fa.states:
            for ch in frm.map:
                dot.edge('S{}'.format(frm.id),
                         'S{}'.format(frm.map[ch].id),
                         label=str(ch) if ch is not Epsilon else 'ε')

    dot.view(filename=filename)


class IdentifierTable(object):
    def __init__(self):
        self.count = 0
        self.table = {}

    def new_ID(self, x: str) -> int:
        assert self.table.get(x) is None
        self.table[x] = self.count
        self.count += 1

        return self.table[x]

    def get_ID(self, x: str) -> int:
        result = self.table.get(x)
        if result is None:
            return self.new_ID(x)
        else:
            return result

    def __repr__(self):
        ret = ["<IdentifierTable"]
        for k, v in sorted(self.table.items(), key=lambda x: x[1]):
            ret.append("  {} => {},".format(v, k))
        ret.append(">")

        return os.linesep.join(ret)


def Saver(obj: Union[List[Token], IdentifierTable], to: str):
    assert os.path.exists(to)

    if isinstance(obj, IdentifierTable):
        with open(os.path.join(to, 'idtable'), 'wb') as f:
            for name in obj.table:
                ID = obj.table[name]
                f.write(struct.pack('IH', ID, len(name)))
                f.write(struct.pack('{}s'.format(len(name)), name.encode()))

    elif isinstance(obj, list):  # and isinstance(self.obj[0], Token):
        with open(os.path.join(to, 'token_sequence'), 'wb') as f:
            for token in obj:
                f.write(struct.pack('H', token.tp.value))
                if isinstance(token.attr, int):
                    f.write(b'i')
                    f.write(struct.pack('q', token.attr))
                elif isinstance(token.attr, str):
                    f.write(b's')
                    f.write(struct.pack('H', len(token.attr)))
                    f.write(struct.pack(
                        '{}s'.format(len(token.attr)), token.attr.encode()))
                elif isinstance(token.attr, enum.Enum):
                    f.write(b'e')
                    f.write(struct.pack('h', token.attr.value))

    else:
        raise ValueError


def Loader(path: str, tp: str) -> Union[IdentifierTable, List[Token]]:
    tp = tp.lower()
    assert tp in ('idtable', 'token_sequence')
    assert os.path.exists(path)

    if tp == 'idtable':
        obj = IdentifierTable()
        with open(os.path.join(path, 'idtable'), 'rb') as f:
            size = struct.calcsize('IH')
            while True:
                head = f.read(size)
                if not head:
                    break
                ID, name_length = struct.unpack('IH', head)
                name = f.read(name_length)
                obj.table[name.decode()] = ID

    elif tp == 'token_sequence':
        obj = []
        with open(os.path.join(path, 'token_sequence'), 'rb') as f:
            size = struct.calcsize('H')
            size_int = struct.calcsize('q')
            size_strlen = struct.calcsize('H')
            size_enum = struct.calcsize('h')
            while True:
                head = f.read(size)
                if not head:
                    break
                tp = TokenType(struct.unpack('H', head)[0])
                value_tp = f.read(1)

                if value_tp == b'i':
                    value = struct.unpack('q', f.read(size_int))[0]
                elif value_tp == b's':
                    strlen = struct.unpack('H', f.read(size_strlen))[0]
                    value = struct.unpack('{}s'.format(strlen),
                                          f.read(strlen))[0].decode()
                elif value_tp == b'e':
                    value = struct.unpack('h', f.read(size_enum))[0]
                    if tp == TokenType.KEYWORD:
                        value = Keyword(value)
                    elif tp == TokenType.OP:
                        value = OP(value)

                obj.append(Token(tp, value))

    else:
        raise ValueError(f"Unknown type {tp}")

    return obj
