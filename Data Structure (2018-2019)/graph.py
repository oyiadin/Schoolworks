# coding=utf-8

import math
import numpy as np


class AbstractGraph(object):
    placeholder = None

    def __init__(self, n=0, nodes: list=None, arcs: list=None):
        self.nodes = nodes if nodes else [None] * n
        if n != 0:
            self.arcs = arcs if arcs \
                             else [[self.placeholder] * n for i in range(n)]
        else:
            self.arcs = [[]]
        

class Graph(object):
    placeholder = 0

    def __init__(self, n=0, nodes: list=None, arcs: list=None):
        super().__init__(n, nodes, arcs)
        @@@@@@@@@@@@@@@prepare()


    def _is_valid(self, x):
        return x != self.placeholder

    def locate_vertex(self, x):
        for n, i in enumerate(self.nodes):
            if i == x:
                return n
        return None

    def get_vertex(self, i):
        if i > len(self.nodes) or i < 0:
            return None
        return self.nodes[i]

    def first_adj_vertex(self, x):
        i = self.locate_vertex(x)
        if i is None:
            return None
        for j in range(len(self.arcs)):
            if self._is_valid(self.arcs[i][j]):
                return self.nodes[j]
        return None

    def insert_vertex(self, x):
        self.nodes.append(x)
        self.arcs = [i + [self.placeholder] for i in self.arcs]
        self.arcs += [[self.placeholder] * len(self.arcs[0])]

    def insert_arc(self, a, b):
        i, j = self.locate_vertex(a), self.locate_vertex(b)
        if i is None or j is None:
            return None
        self.arcs[i][j] = 1

    def dfs(self, root, f=lambda x: True):
        root_index = self.get_vertex(root)
        if root_index is None:
            return False


    def bfs(self, root, f=lambda x: True):
        root_index = self.get_vertex(root)
        if root_index is None:
            return False

class Net(Graph):
    placeholder = math.inf

    def __init__(self, n=0, nodes: list=None, arcs: list=None):
        super().__init__(n, nodes, arcs)
        self.arcs = arcs if arcs else [[self.placeholder] * n for i in range(n)]

    def insert_arc(self, a, b, w):
        i, j = self.locate_vertex(a), self.locate_vertex(b)
        if i is None or j is None:
            return None
        self.arcs[i][j] = w


nodes = list(map(str, [1, 2, 3, 4, 5, 6]))
arcs = [[Net.placeholder] * 6 for i in range(6)]
arcs[0][2] = 1
arcs[3][5] = 2
arcs[1][4] = 3
arcs[2][5] = 4
arcs[0][3] = 5
arcs[1][2] = 5
arcs[2][3] = 5
arcs[0][1] = 6
arcs[2][4] = 6
arcs[4][5] = 6

net = Net(nodes=nodes, arcs=arcs)
net.kruskal()   v