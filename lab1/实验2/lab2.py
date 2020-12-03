from pulp import *
import numpy as np

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
        ORIGIN_NODE_GRAPH[str(node.origin_id)] = [str(c.node_id) for c in node.child_list if c.node_id != 0]


# 输出精简图信息
def printG(G):
    for dic in G:
        print(dic, end=": ")
        print(G[dic])


def ILP_plus(pe, points, times, G, nodeList):  # peMAX， 节点数量，最长时间步，图，

    resMii = np.ceil(points / pe)

    # 遍历，假定都在最早时间不执行

    recMii = np.ceil(0)  # 到时候再写

    MII = max(resMii, recMii)

    II = int(MII)

    prob = LpProblem("myProblem", LpMinimize)

    PE = LpVariable("PE", lowBound=0, upBound=pe, cat="Integer")
    prob += PE

    vars = [[LpVariable("x" + str(j) + "_" + str(i) + "_" + str(k),
                        lowBound=0, upBound=1, cat="Integer")
             for i in range(times)
             for k in range(times)]
            for j in range(points)]

    varsY = [[LpVariable("y" + str(j) + "_" + str(i) + "_" + str(k),
                         lowBound=0, upBound=1, cat="Integer")
              for i in range(times)
              for k in range(times)]
             for j in range(points)]

    varsZ = [[LpVariable("z" + str(j) + "_" + str(i) + "_" + str(k),
                         lowBound=0, upBound=1, cat="Integer")
              for i in range(times)
              for k in range(times)]
             for j in range(points)]

    print(vars[1])

    node_sn_ln_dict = {}

    for i in NODE_LIST:
        node_sn_ln_dict[int(i.now_id)] = [i.t_start, i.t_end]

    print("node dict")
    print(node_sn_ln_dict)

    # 唯一性：算子调度时间步是唯一的
    for j in range(points):
        list = []
        count = 0
        for k in range(times):
            list.append(vars[j][k * times + count])
            count += 1

        prob += sum(list) == 1

    # 排他性：
    for n in range(points):
        for i1 in range(times):
            for i2 in range(times):
                if i1 == i2:
                    continue
                else:
                    for j2 in range(times):
                        prob += vars[n][i1 * times + i1] + vars[n][i2 * times + j2] <= 1
                        prob += vars[n][i1 * times + i1] + varsY[n][i2 * times + j2] <= 1
                        prob += vars[n][i1 * times + i1] + varsZ[n][i2 * times + j2] <= 1

    # PE路由与memory路由互斥：
    for n in range(points):
        for i in range(times):
            for j1 in range(times):
                for j2 in range(times):
                    prob += varsY[n][i * times + j1] + vars[n][i * times + j2] <= 1

    # load和store要同时存在：
    for n in range(points):
        for i in range(times):
            for j1 in range(times):
                for j2 in range(times):
                    prob += varsY[n][i * times + j1] - varsZ[n][i * times + j2] == 0

    # 依赖约束：算子节点调度时间必须满足早于其子节点
    for n1 in G:
        for n2 in G[n1]:
            for i1 in range(times):
                childPointList = []
                for i2 in range(node_sn_ln_dict[int(n2)][0], node_sn_ln_dict[int(n2)][1] + 1):
                    childPointList.append(i2 * vars[int(n2)-1][i2*times + i2])
                prob += i1 * vars[int(n1)-1][i1*times + i1] <= lpSum(childPointList)+1

    # 依赖约束2
    # 算子执行时间步必须位于最早最晚时间步之间
    for n1 in range(points):
        for i1 in range(times):
            for j1 in range(times):
                for i2 in range(times):

                    tempNode = 0
                    Max = 0
                    for n2 in G[str(n1+1)]:
                        Max = max(node_sn_ln_dict[int(n2)][1], Max)
                        if Max == node_sn_ln_dict[int(n2)][1]:
                            tempNode = int(n2)-1

                        prob += j1*vars[n1][j1*times + j1] <= i2*vars[tempNode][i2*times + i2] + 1


    II = 3
    # PE约束
    maxListNum = np.math.ceil(times / II)  # 循环次数
    print("max")
    print(maxListNum)
    print(II)
    for ii in range(int(II)):
        pe_constrainList = []

        for k in range(maxListNum):
            if (ii + k * II) < times:
                for n in range(points):
                    for i in range(times):
                        pe_constrainList.append(vars[n][i*times+ii + k * II])
                        pe_constrainList.append(varsY[n][i*times+ii + k * II])
                        pe_constrainList.append(varsZ[n][i*times+ii + k * II])

        prob += lpSum(pe_constrainList) <= PE

    # for n in range(points):
    #     for i in range(times):
    #         for ii in range(II):

    print(prob)

    status = prob.solve()
    print(status)
    # print(prob)


if __name__ == "__main__":
    read("./in")
    write("./out")
    pre()  # 预处理得到精简图
    printG(ORIGIN_NODE_GRAPH)
    # for i in NODE_LIST:
    #     print(i.now_id)
    #     print(i.t_start, i.t_end)

    ILP_plus(16, 11, 8, ORIGIN_NODE_GRAPH, NODE_LIST)
