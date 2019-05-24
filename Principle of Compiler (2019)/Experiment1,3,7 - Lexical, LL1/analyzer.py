# coding=utf-8
import string
from typing import List, overload, Tuple

from idtable import IdentifierTable
from regex import *
from tokens import *

__all__ = ('Analyser',)


class Analyser(object):
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
                part = content[idx1:idx1 + idx2]
                print('===> Encountered with some blanks, ignored.\n')
                idx1 += idx2
                continue

            idx2 = self.ID.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1:idx1 + idx2]
                if part in self.keyword:
                    result.append(
                        Token(TokenType.KEYWORD, Keyword[part.upper()]))
                    print(
                        '===> Encountered with a KEYWORD `{}`!\n'.format(part))
                else:
                    ID = self.idtable.get_ID(part)
                    result.append(Token(TokenType.ID, ID))
                    print('===> Encountered with an ID `{}`, id={}!\n'.format(
                        part, result[-1].attr))

                idx1 += idx2
                continue

            idx2 = self.numbers.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1:idx1 + idx2]
                result.append(Token(TokenType.NUM, int(part)))
                print('===> Encountered with a NUMBER `{}`!\n'.format(part))

                idx1 += idx2
                continue

            idx2 = self.string.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[
                       idx1 + 1:idx1 + idx2 - 1]  # strip off the wrapper
                result.append(Token(TokenType.STR, part))
                print('===> Encountered with a STR `{}`!\n'.format(part))

                idx1 += idx2
                continue

            idx2 = self.ops.matchstart(content[idx1:])
            if idx2 != -1:
                part = content[idx1:idx1 + idx2]

                if part == '#':
                    print(
                        '===> Oops, seems like this is a comment, skipping.\n')
                    return result

                else:
                    result.append(Token(TokenType.OP, op2enum[part]))
                    print('===> Encountered with an OP `{}`!\n'.format(part))
                    idx1 += idx2
                    continue

            # sth error...
            print('\n================ ERROR ================')
            print(content)
            print(' ' * idx1 + '^')
            print('Error: unexpected character')
            print('=======================================')

            raise SyntaxError

        return result

    @overload
    def analyse(self, content: str) -> Tuple[IdentifierTable, List[Token]]:
        ...
    @overload
    def analyse(self, content: List[str]) -> Tuple[
        IdentifierTable, List[Token]]:
        ...
    def analyse(self, content):
        if isinstance(content, str):
            return self.idtable, self._analyse(content)
        elif isinstance(content, list):
            ret = []
            for line in content:
                ret.extend(self._analyse(line))
            return self.idtable, ret
