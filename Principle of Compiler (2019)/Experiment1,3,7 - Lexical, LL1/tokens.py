# coding=utf-8
from typing import Any
import enum

__all__ = (
    'TokenType', 'Keyword', 'OP', 'op2enum', 'Token'
)


class MyEnum(enum.Enum):
    def __str__(self):
        return self._name_


class TokenType(MyEnum):
    ID = 0
    KEYWORD = 1
    NUM = 2
    OP = 3
    STR = 4


class Keyword(MyEnum):
    BEGIN = 0
    END = 1
    IF = 2
    THEN = 3
    ELSE = 4
    FOR = 5
    WHILE = 6
    DO = 7
    AND = 8
    OR = 9
    NOT = 10


class OP(MyEnum):
    ADD = 0
    MINUS = 1
    MUL = 2
    DIV = 3
    LT = 4
    GT = 5
    EQ = 6
    ASSIGNMENT = 7
    LE = 8
    GE = 9
    NEQ = 10
    INCR = 11
    DECR = 12
    LEFT_PAR = 13
    RIGHT_PAR = 14
    SEMICOLON = 15
    COMMENT = 16


op2enum = {
    '+': OP.ADD,
    '-': OP.MINUS,
    '*': OP.MUL,
    '/': OP.DIV,
    '>': OP.LT,
    '<': OP.GT,
    '=': OP.EQ,
    ':=': OP.ASSIGNMENT,
    '>=': OP.LE,
    '<=': OP.GE,
    '!=': OP.NEQ,
    '++': OP.INCR,
    '--': OP.DECR,
    '(': OP.LEFT_PAR,
    ')': OP.RIGHT_PAR,
    ';': OP.SEMICOLON,
    '#': OP.COMMENT,
}


class Token(object):
    def __init__(self, tp: TokenType, attr: Any):
        assert isinstance(tp, TokenType)
        self.tp = tp
        self.attr = attr

    def __repr__(self):
        return "<Token: ({}, {})>".format(self.tp, self.attr)

    def __str__(self):
        return str(self.attr)

    def __eq__(self, other):
        if isinstance(other, Token):
            return self.tp == other.tp and self.attr == other.attr
        return False

    def __hash__(self):
        return hash(self.__repr__())
