# coding=utf-8
import string
from typing import List, Tuple, Union

from utils import IdentifierTable
from regex import *
from tokens import *

__all__ = ('Lexical',)


class Lexical(object):
    def __init__(self):
        self.idtable = IdentifierTable()

        letter = [Match(ch) for ch in string.ascii_letters]
        letter = Alter(*letter)

        digit = [Match(ch) for ch in string.digits]
        digit = Alter(*digit)

        self.keyword = [
            "begin", "end", "if", "then", "else", "for",
            "while", "do", "and", "or", "not"]

        self.ID = Concat(
            Alter(Match("_"), letter),
            AnyTimes(Alter(letter, digit, Match("_")))
        ).compile()
        self.numbers = AtLeastOnce(digit).compile()
        self.ops = Alter(*[Match(x) for x in [
            "+", "-", "*", "/", ">", "<", "=", ":=", ">=", "<=",
            "!=", "++", "--", "(", ")", ";", "#"]]).compile()
        self.string = Concat(Match('"'),
                             AnyTimes(MatchAnyBut('"')),
                             Match('"')).compile()
        self.blank = AtLeastOnce(Alter(*[Match(x) for x in [
            " ", "\t", "\n", "\r"]])).compile()

    def _analyse(self, content: str) -> List[Token]:
        result = []
        idx1 = 0
        while idx1 < len(content):
            idx2 = self.blank.matchstart(content[idx1:])
            if idx2 != -1:
                # part = content[idx1:idx1 + idx2]
                print('[==>] Result: Blanks\n')
                idx1 += idx2
                continue

            idx2 = self.ID.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1:idx1 + idx2]
                if part in self.keyword:
                    result.append(
                        Token(TokenType.KEYWORD, Keyword[part.upper()]))
                    print(
                        '[==>] Result: Keyword `{}`\n'.format(part))
                else:
                    ID = self.idtable.get_ID(part)
                    result.append(Token(TokenType.ID, ID))
                    print('[==>] Result: ID `{}` (id = {})\n'.format(
                        part, result[-1].attr))

                idx1 += idx2
                continue

            idx2 = self.numbers.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1:idx1 + idx2]
                result.append(Token(TokenType.NUM, int(part)))
                print('[==>] Result: Number `{}`\n'.format(part))

                idx1 += idx2
                continue

            idx2 = self.string.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1 + 1:idx1 + idx2 - 1]
                # strip off the wrapper
                result.append(Token(TokenType.STR, part))
                print('[==>] Result: String `{}`\n'.format(part))

                idx1 += idx2
                continue

            idx2 = self.ops.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1:idx1 + idx2]

                if part == '#':
                    print('[==>] Result: Comments\n')
                    return result

                else:
                    result.append(Token(TokenType.OP, op2enum[part]))
                    print('[==>] Result: Operator `{}`\n'.format(part))
                    idx1 += idx2
                    continue

            # sth error...
            print('\n')
            print('[ ! ] Unexpected symbol:')
            print('[ > ]', content)
            print('[ @ ]', ' ' * idx1 + '^')

            raise SyntaxError

        return result

    def analyse(self, content: Union[str, List[str]])  \
            -> Tuple[IdentifierTable, List[Token]]:
        if isinstance(content, str):
            return self.idtable, self._analyse(content)

        elif isinstance(content, list):
            ret = []
            for line in content:
                ret.extend(self._analyse(line))
            return self.idtable, ret
