# coding=utf-8
# author：dzzhyk
# date：2020年10月25日16:33:48
# title：lab1.py

import pulp

CHILD_NUMBER = 4  # 最大子节点数量
NODE_LIST = []  # 节点列表
ORIGIN_NODE_GRAPH = {}  # 原始图
NEW_NODE_GRAPH = {}  # 结果图


# 子节点类
class ChildNode:
    def __init__(self, node_id, edge_type):
        self.node_id = node_id
        self.edge_type = edge_type

    def __str__(self):
        return f"[ChildNode] node_id={self.node_id}, edge_type={self.edge_type}"


# 节点类
class Node:
    def __init__(self, _id, child_list, t_start, t_end, node_type, has_parent):
        self.now_id = _id  # 现节点编号
        self.origin_id = _id  # 原节点编号
        self.child_list = child_list  # 子节点列表
        self.t_start = t_start  # 开始
        self.t_end = t_end  # 结束
        self.node_type = node_type  # 节点类型
        self.has_parent = has_parent  # 是否有父节点
        self.t_step = t_start - 1  # 时间步初始化为t_start-1

    def __str__(self):
        return f"[Node] now_id={self.now_id}, origin_id={self.origin_id}, child_list=[" + ",".join(
            str(c) for c in self.child_list) + \
               f"], t_start={self.t_start}, t_end={self.t_end}, node_type={self.node_type}, has_parent={self.has_parent}\n"

    def toFile(self):
        return f"{self.now_id},{','.join([str(c.node_id) for c in self.child_list])},{self.node_type},{self.origin_id}\n"


# 读入函数
def read(filename):
    with open(filename, "r", encoding="utf-8") as fin:
        print("<=== 读入文件：" + filename)
        temp = fin.readline()
        while temp != "":
            temp_ls = temp.split(",")
            temp_id = temp_ls[0]
            temp_child = [ChildNode(int(temp_ls[i * 2 + 1]), int(temp_ls[i * 2 + 2])) for i in range(CHILD_NUMBER)]
            temp_start, temp_end, temp_type, temp_parent = [int(a) for a in
                                                            temp_ls[CHILD_NUMBER * 2 + 1:CHILD_NUMBER * 2 + 5]]
            NODE_LIST.append(Node(temp_id, temp_child, temp_start, temp_end, temp_type, temp_parent))
            temp = fin.readline()


# 输出函数
def write(filename):
    with open(filename, "w", encoding="utf-8") as fout:
        print("===> 输出目标：" + filename)
        for node in NODE_LIST:
            fout.write(str(node))
        for node in NODE_LIST:
            fout.write(node.toFile())


# 预处理输入数据得到原始图
def pre():
    for node in NODE_LIST:
        ORIGIN_NODE_GRAPH[str(node.origin_id)] = [str(c.node_id) for c in node.child_list if c.node_id!=0]


# 输出精简图信息
def printG(G):
    for dic in G:
        print(dic, end=": ")
        print(G[dic])


# 图拓扑排序
# 返回结果节点列表 [2, 1, 3, 4]
def topological(G):
    in_degrees = dict((u, 0) for u in G)
    for u in G:
        for v in G[u]:
            if v == '0': continue
            in_degrees[v] += 1
    Q = ['1']
    res = []
    while Q:
        u = Q.pop()
        res.append(u)
        for v in G[u]:
            in_degrees[v] -= 1
            if in_degrees[v] == 0 and v!='1':
                Q.append(v)
    return res


# 图逆拓扑排序
# 返回结果节点列表 [4, 3, 1, 2]
def reverse_topological(G):
    # 首先反向建图
    RG = {}
    for u in G:
        RG[u] = []
    for u in G:
        for v in G[u]:
            if v == '0': continue
            RG[v].append(u)
    for u in RG:
        RG[u] = sorted(list(set(RG[u])))  # 去重
    return topological(RG)


# ASAP调度
def asap():
    pass


# ALAP
def alap():
    pass


if __name__ == "__main__":
    read("./in")
    # write("./out")
    pre()  # 预处理得到精简图
    printG(ORIGIN_NODE_GRAPH)
    ans = topological(ORIGIN_NODE_GRAPH)
    ans_reversed = reverse_topological(ORIGIN_NODE_GRAPH)
    print(ans)
    print(ans_reversed)
