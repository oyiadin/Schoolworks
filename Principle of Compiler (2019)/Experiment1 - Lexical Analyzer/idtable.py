# coding=utf-8
import os
import struct


class IdentifierTable(object):
    def __init__(self):
        self.count = 0
        self.table = {}

    def new_ID(self, x: str):
        assert self.table.get(x) is None
        self.table[x] = self.count
        self.count += 1

        return self.table[x]

    def get_ID(self, x: str):
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
