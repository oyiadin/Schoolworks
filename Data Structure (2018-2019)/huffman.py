# coding=utf-8
# 课上随便写的，代码比较乱，懒得改了=。=

import math


class BinTree(object):
    def __init__(self, value=None, left=None, right=None):
        self.value = value
        self.left = left
        self.right = right


class Huffman(object):
    def __init__(self):
        self.btree = None
        self.table = None

    def cons_btree(self, x: list):
        def calc_freq(x: list):
            freq = {}
            for i in x:
                freq[i] = freq.get(i, 0) + 1
            return freq

        freq = calc_freq(x)
        trees = list(map(lambda x: (BinTree(x[0]), x[1]), freq.items()))

        while len(trees) >= 2:
            trees = sorted(trees, key=lambda x: x[1], reverse=True)
            a, b = trees.pop(), trees.pop()
            trees.append((BinTree(None, a[0], b[0]), a[1] + b[1]))

        self.btree = trees[0][0]
        return self.btree

    def gen_table(self, btree: BinTree = None):
        assert btree or self.btree, "call cons_btree() first"
        if not btree:
            btree = self.btree

        table = {}

        def f(x, code=''):
            if x is None:
                return
            elif x.left is None and x.right is None:
                table[x.value] = code
            f(x.left, code + '0')
            f(x.right, code + '1')

        f(btree)
        self.table = table
        return table

    def encode(self, x: list) -> str:
        assert self.table, "call gen_table() first"
        return ''.join([self.table[i] for i in x])

    def decode(self, cipher: str) -> list:
        assert self.btree, "call cons_btree() first"
        if not cipher:
            return ''

        decoded = []
        at = self.btree
        i = 0

        while i < len(cipher):
            bit = cipher[i]
            if bit == '0':
                if not at.left:
                    decoded.append(at.value)
                    at = self.btree
                else:
                    at = at.left
                    i += 1
            else:
                if not at.right:
                    decoded.append(at.value)
                    at = self.btree
                else:
                    at = at.right
                    i += 1
        decoded.append(at.value)
        return decoded


if __name__ == '__main__':
    huff = Huffman()

    while True:
        raw = list(input('>> '))
        huff.cons_btree(raw)
        huff.gen_table()

        for i in huff.table:
            print('t>', i, huff.table[i])
        encoded = huff.encode(raw)
        print('e>', encoded)
        decoded = ''.join(huff.decode(encoded))
        print('d>', decoded)

        original_bits_per_character = math.ceil(math.log(len(raw), 2))
        huffman_bits_per_character = len(encoded) / len(raw)
        print('====> {}% <===='.format(
            100 * huffman_bits_per_character / original_bits_per_character))
