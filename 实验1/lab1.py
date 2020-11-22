# coding=utf-8
# author：dzzhyk
# date：2020年10月25日16:33:48
# title：lab1.py

from pulp import *

CHILD_NUMBER = 4  # 最大子节点数量
NODE_LIST = []  # 节点列表
ORIGIN_NODE_GRAPH = {}  # 简化过后的原始图，只有节点关系数据，用于拓扑排序
AFTER_TOPO = []
AFTER_RETOPO = []
AFTER_ASAP_ALAP = []  # 使用ASAP_ALAP调度过后的图节点列表
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
        self.t_step = t_start - 1  # 节点step

    def __str__(self):
        return f"[Node] now_id={self.now_id}, origin_id={self.origin_id}, child_list=[" + ",".join(
            str(c) for c in self.child_list) + \
               f"], t_start={self.t_start}, t_end={self.t_end}, node_type={self.node_type}, has_parent={self.has_parent}\n"

    def toFile(self):
        return f"{self.now_id},{self.t_step},{','.join([str(c.node_id) for c in self.child_list])},{self.node_type},{self.origin_id}\n"


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
            AFTER_ASAP_ALAP.append(Node(temp_id, temp_child, temp_start, temp_end, temp_type, temp_parent))
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
        ORIGIN_NODE_GRAPH[str(node.origin_id)] = [str(c.node_id) for c in node.child_list if c.node_id != 0]


# 输出精简图信息
def printG(G):
    for dic in G:
        print(dic, end=": ")
        print(G[dic])


# 图拓扑排序
# 返回结果节点列表 [2, 1, 3, 4]
def topological(G):
    in_degree = dict((u, 0) for u in G)
    for u in G:
        for v in G[u]:
            if v == '0': continue
            in_degree[v] += 1

    Q = []

    for key, value in in_degree.items():
        if key != '0' and value == 0:
            Q.append(key)

    res = []
    while len(Q) > 0:
        u = Q.pop()
        res.append(u)
        for v in G[u]:
            in_degree[v] -= 1
            if in_degree[v] == 0:
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
    # 求解PIs
    PIs = set()
    for node in ORIGIN_NODE_GRAPH.values():
        for i in node:
            PIs.add(i)

    for v in AFTER_TOPO:
        act_v = None
        # 首先找到节点原有对象
        for node in AFTER_ASAP_ALAP:
            if node.origin_id == eval(v):
                act_v = node
                break
        if v in PIs:
            act_v.new_id = 1
        else:
            # 找到最前面的节点
            pass


# ALAP调度
def alap():
    pass


# 求解整数规划
def solve_ilp(objective, constraints):
    prob = pulp.LpProblem('LP1', pulp.LpMinimize)
    prob += objective
    for cons in constraints:
        prob += cons
    print(prob)
    # 判断是否求解成功
    status = prob.solve()
    if status != 1:
        return None
    else:
        # 返回整数解
        return [v.varValue.real for v in prob.variables()]


if __name__ == "__main__":
    read("./in")
    # write("./out")
    pre()  # 预处理得到精简图
    printG(ORIGIN_NODE_GRAPH)
    AFTER_TOPO = topological(ORIGIN_NODE_GRAPH)
    AFTER_RETOPO = reverse_topological(ORIGIN_NODE_GRAPH)

    # 变量，直接设置下限，并设置cat类型为整数解
    X1 =
    X2 = pulp.LpVariable('X2', lowBound=3, cat=pulp.LpInteger)
    X3 = pulp.LpVariable('X3', lowBound=0, cat=pulp.LpInteger)

    N = 10
    temp = [k for k in range(1, N+1)]
    vars = ["x"+str(i) for i in temp]
    touse = []

    for v in vars:
        for i in range(1, N+1):
            touse.append(v+str(i))

    # 变量
    variables = []
    variables = [LpVariable(x, lowBound=0, upBound=1) for x in touse]

    cons = []
    cons.append(sum([a1[i] * variables[i] for i in range(0, V_NUM)]) <= 1000000)

    res = solve_ilp()
    print(res)
