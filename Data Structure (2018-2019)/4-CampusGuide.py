# coding=utf-8

import math
from pprint import pprint


class Place(object):
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
    def __del__(self):
        pass


class Net(object):
    placeholder = math.inf
    # 用♾来表示那些不存在的边的权值

    def __init__(self, n=0, nodes: list=None, arcs: list=None):
        # nodes用以储存节点信息（Place对象）
        # arcs即为邻接矩阵
        self.n = n
        self.nodes = nodes if nodes else [None] * n

        if n != 0:
            self.arcs = arcs if arcs else [[self.placeholder] * n for i in range(n)]
            # 默认创建节点间互不相连的网
        else:
            self.arcs = [[]]

    def __del__(self):
        pass

    def _is_valid(self, x):
        return x != self.placeholder
        # 判断特定边是否存在（权值不是♾）

    def locate_vertex(self, x):
        for n, i in enumerate(self.nodes):
            if i == x:
                return n
                # 如果与x相等则返回其下标
        return None

    def get_vertex(self, i):
        if i > len(self.nodes) or i < 0:
            # 边界检查
            return None
        return self.nodes[i]

    def first_adj_vertex(self, root):
        i = root
        for j in range(len(self.arcs)):
            if self._is_valid(self.arcs[i][j]):
                return j
            # 依次检查root与各个节点的边的权值，如果不是♾则立即返回对应节点的下标
        return None

    def next_adj_vertex(self, root, current):
        i = root
        for j in range(len(self.arcs)):
            if self._is_valid(self.arcs[i][j]) and current == j:
                # 找到current对应的邻接节点下标
                # 然后从此处继续往后找
                for k in range(j+1, len(self.arcs)):
                    if self._is_valid(self.arcs[i][k]):
                        return k
                return None
        return None

    def get_adj_vertexs(self, root):
        vertexs = []
        at = self.first_adj_vertex(root)
        while at is not None:
            vertexs.append(at)
            at = self.next_adj_vertex(root, at)
        # 对first_adj_vertex与next_adj_vertex的功能做一个简单的拼接
        return vertexs

    def insert_vertex(self, x):
        self.nodes.append(x)
        self.arcs = [i + [self.placeholder] for i in self.arcs]
        self.arcs += [[self.placeholder] * len(self.arcs[0])]
        # 相应地，邻接矩阵也要扩大

    def insert_arc(self, i, j, w):
        self.arcs[i][j] = self.arcs[j][i] = w

    def _get_min(self, D: dict):
        # 辅助函数，用于dijkstra算法中
        min_value = self.placeholder
        min_key = None

        for key in D:
            val = D[key]
            if val < min_value:
                min_key = key
                min_value = val

        D.pop(min_key)
        return min_value, min_key

    def dijkstra(self, start, end=None):
        # 实现带路径回溯的dijkstra算法
        D, P, Q = {}, {}, {}
        # D 是已经找到最短路径的节点
        # P 用于记录路径
        # Q 是还在处理中的节点
        Q[start] = 0

        while True:
            if len(Q) == 0:
                break

            dist, v = self._get_min(Q)
            D[v] = dist
            if v == end:
                break

            for w in self.get_adj_vertexs(v):
                vwLength = D[v] + self.arcs[v][w]
                if w in D:
                    continue

                if self.arcs[v][w] >= math.inf:
                    continue
                    # 这条边不存在则直接跳过

                if w not in Q or vwLength < Q[w]:
                    # 如果找到了更短的路径
                    Q[w] = vwLength
                    P[w] = v

        return (D, P)

    def get_shortest_route(self, start, to):
        distances, paths = self.dijkstra(start)
        # 根据带路径回溯的dijkstra算法所返回的结果
        # 生成最短路径并返回之
        path = []
        while to != start:
            path.append(to)
            to = paths[to]
        path.append(start)
        path.reverse()
        # 因为paths的key->value与实际上的路径方向相异，所以要先做一个逆置
        return path


def do_inspect(net):
    while True:
        print('')
        print('[*] Input Q to quit')
        print('Input a ID to insepct the information of it: ')
        ID = input(' >> ')
        # 做一些输入合法性检查
        if not ID.isdecimal():
            if ID.strip().lower() == 'q':
                return
            print('[!] Illegal input, please try again.')
            continue
        ID = int(ID)
        if ID > net.n:
            print('[!] Index overflow, please try again.')
            continue
        place = net.nodes[ID]
        # 直接从网net的节点集中取出对应Place对象
        print('  * ID = {}, Name = {}'.format(ID, place.name))
        print('  * Description = {}'.format(place.description))

def do_query(net):
    while True:
        print('')
        print('[*] Input Q to quit')
        while True:
            print('Where are you right now?')
            ID1 = input(' >> ')
            # 做一些输入合法性检查
            if not ID1.isdecimal():
                if ID1.strip().lower() == 'q':
                    return
                print('[!] Illegal input, please try again.')
                continue
            ID1 = int(ID1)
            if ID1 > net.n:
                print('[!] Index overflow, please try again.')
                continue
            break

        while True:
            print('Where are you going?')
            ID2 = input(' >> ')
            # 做一些输入合法性检查
            if not ID2.isdecimal():
                if ID2.strip().lower() == 'q':
                    return
                print('[!] Illegal input, please try again.')
                continue
            ID2 = int(ID2)
            if ID2 > net.n:
                print('[!] Index overflow, please try again.')
                continue
            break

        name1, name2 = net.nodes[ID1].name, net.nodes[ID2].name
        print('From = {}, Dest = {}'.format(name1, name2))
        # 打印对应节点的简略信息
        path = net.get_shortest_route(ID1, ID2)
        # 调用函数获得最短路径
        print('The shortest path is:')
        print('[', ' -> '.join([str(net.nodes[i].name) for i in path]), ']')


if __name__ == '__main__':
    nodes = [
        Place('西北田径场', '在生活区里，偶尔去跑步'),
        Place('10号宿舍楼', '寝室楼'),
        Place('体育馆', '也叫飞碟'),
        Place('五餐', '吃饭的地方'),
        Place('八教', 'ACM大佬集聚的神秘地方'),
        Place('六教', '上课的地方'),
        Place('学活', '学生活动中心'),
        Place('七教', '离宿舍最近的教学楼'),
        Place('图书馆', '借书的地方，书很全'),
        Place('三教', '遥远的一栋教学楼'),
        Place('二教', '没去过'),
        Place('一教', '计院大本营'),
        Place('南大门', '超级大')
    ]
    net = Net(n=13, nodes=nodes)

    net.insert_arc(0, 1, 5)
    net.insert_arc(0, 2, 3)
    net.insert_arc(2, 3, 4)
    net.insert_arc(1, 3, 3)
    net.insert_arc(2, 4, 1)
    net.insert_arc(4, 5, 1)
    net.insert_arc(5, 8, 2)
    net.insert_arc(3, 6, 1)
    net.insert_arc(6, 7, 1)
    net.insert_arc(6, 8, 2)
    net.insert_arc(7, 8, 2)
    net.insert_arc(5, 10, 4)
    net.insert_arc(10, 12, 2)
    net.insert_arc(7, 9, 2)
    net.insert_arc(9, 11, 2)
    net.insert_arc(11, 12, 2)

    while True:
        print('Please choose the functionality you wanna use: ')
        print('  * (I)nspect the information of a specific site.')
        print('  * (Q)uery the shortest path between two sites.')
        choice = input(' >> ')
        if choice.strip().lower() in ("i", "inspect"):
            do_inspect(net)
        elif choice.strip().lower() in ("q", "query"):
            do_query(net)
        else:
            print('[!] Unknown option, please try again.')
        print('')
