# coding=utf-8
from graphviz import Digraph

Epsilon = ...

__all__ = ('DotShow',)


class DotShow(object):
    def _show__create_nodes(self, dot):
        for q in self.states:
            node = 'S{}'.format(q.id)
            if q in self.accept_states:
                dot.node(node, shape='box')
            else:
                dot.node(node)

    def _show__draw_connections(self, dot):
        for frm in self.states:
            for ch in frm.map:
                for to in frm.map[ch]:
                    dot.edge('S{}'.format(frm.id),
                             'S{}'.format(to.id),
                             label=ch if ch is not Epsilon else 'ε')

    def show(self):
        dot = Digraph(comment='The Illustration')
        self._show__create_nodes(dot)
        self._show__draw_connections(dot)

        dot.view()
