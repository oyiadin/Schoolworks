# coding=utf-8
import enum
import os
import struct
from typing import overload, List

from idtable import IdentifierTable
from tokens import *


class Saver(object):
    @overload
    def __init__(self, obj: List[Token]):
        ...
    @overload
    def __init__(self, obj: IdentifierTable):
        ...
    def __init__(self, obj):
        self.obj = obj

    def saveto(self, path: str):
        assert os.path.exists(path)

        if isinstance(self.obj, IdentifierTable):
            with open(os.path.join(path, 'idtable'), 'wb') as f:
                for name in self.obj.table:
                    ID = self.obj.table[name]
                    f.write(struct.pack('IH', ID, len(name)))
                    f.write(struct.pack('{}s'.format(len(name)), name.encode()))

        elif isinstance(self.obj, list):  # and isinstance(self.obj[0], Token):
            with open(os.path.join(path, 'token_sequence'), 'wb') as f:
                for token in self.obj:
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

    def loadfrom(self, path: str):
        assert os.path.exists(path)

        if isinstance(self.obj, IdentifierTable):
            with open(os.path.join(path, 'idtable'), 'rb') as f:
                size = struct.calcsize('IH')
                while True:
                    head = f.read(size)
                    if not head:
                        break
                    ID, name_length = struct.unpack('IH', head)
                    name = f.read(name_length)
                    self.obj.table[name.decode()] = ID

        elif isinstance(self.obj, list):  # and isinstance(self.obj[0], Token):
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

                    self.obj.append(Token(tp, value))
