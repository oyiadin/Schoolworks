# coding=utf-8

import math
import numpy as np


class Graph(object):
    placeholder = 0

    def __init__(self, n=0, nodes: list=None, arcs: list=None):
        self.nodes = nodes if nodes else [None] * n
        if n != 0:
            self.arcs = arcs if arcs else [[self.placeholder] * n] * n
        else:
            self.arcs = [[]]

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

    def kruskal(self):
        subgroups = [i for i in range(len(self.nodes))]
        subgroups_remains = len(self.nodes)
        # give every members of a subgroup a single ID
        weights = []
        for i in range(len(self.arcs)):
            for j in range(len(self.arcs)):
                if self._is_valid(self.arcs[i][j]):
                    weights.append(((i, j), self.arcs[i][j]))
        weights.sort(key=lambda x: x[1])

        selected_arcs = []

        while subgroups_remains > 1:
            while True:
                if not weights:
                    return None
                arc = weights.pop(0)
                if subgroups[arc[0][0]] == subgroups[arc[0][1]]:
                    continue
                from_group_id = subgroups[arc[0][1]]
                to_group_id = subgroups[arc[0][0]]
                for i in range(len(subgroups)):
                    if subgroups[i] == from_group_id:
                        subgroups[i] = to_group_id
                selected_arcs.append((arc[0][0], arc[0][1]))
                subgroups_remains -= 1
                break

        return selected_arcs


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
net.kruskal()