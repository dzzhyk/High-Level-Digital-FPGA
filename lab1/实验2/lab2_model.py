from pulp import *
import numpy as np

CHILD_NUMBER = 4  # 最大子节点数量
NODE_LIST = []  # 节点列表
ORIGIN_NODE_GRAPH = {}  # 原始图
NEW_NODE_GRAPH = {}  # 结果图

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


def generateModelAndVariables(pointNums, maxTimeStep, maxPE):
    model = LpProblem("prob", LpMinimize)
    # x
    x_variable_array = []
    for point in range(pointNums):
        new_x_array = []
        for turn in range(maxTimeStep):
            new_x_line = []
            for sub_turn in range(maxTimeStep):
                extend_name = "," + str(point) + "," + str(turn) + "," + str(sub_turn)
                new_x_line.append(LpVariable("x" + extend_name, lowBound=0, upBound=1, cat=LpInteger))
            new_x_array.append(new_x_line)
        x_variable_array.append(new_x_array)
    # y
    y_variable_array = []
    for point in range(pointNums):
        new_y_array = []
        for turn in range(maxTimeStep):
            new_y_line = []
            for sub_turn in range(maxTimeStep):
                extend_name = "," + str(point) + "," + str(turn) + "," + str(sub_turn)
                new_y_line.append(LpVariable("y" + extend_name, lowBound=0, upBound=1, cat=LpInteger))
            new_y_array.append(new_y_line)
        y_variable_array.append(new_y_array)
    # z
    z_variable_array = []
    for point in range(pointNums):
        new_z_array = []
        for turn in range(maxTimeStep):
            new_z_line = []
            for sub_turn in range(maxTimeStep):
                extend_name = "," + str(point) + "," + str(turn) + "," + str(sub_turn)
                new_z_line.append(LpVariable("z" + extend_name, lowBound=0, upBound=1, cat=LpInteger))
            new_z_array.append(new_z_line)
        z_variable_array.append(new_z_array)

    pe = LpVariable("pe", lowBound=0, upBound=maxPE, cat=LpInteger)

    return model, x_variable_array, y_variable_array, z_variable_array, pe


def readSnLn():
    node_sn_ln_dict = {}

    for i in NODE_LIST:
        node_sn_ln_dict[int(i.now_id)] = [i.t_start, i.t_end]

    return node_sn_ln_dict


def addUniqueConstraint(model, xV, pointNums, nodeTimeStepDict):  # 唯一性
    for p in range(pointNums):
        Sn, Ln = nodeTimeStepDict[p+1][0], nodeTimeStepDict[p+1][1]

        sum_x_ptt = 0
        if nodeTimeStepDict[p + 1][0] == nodeTimeStepDict[p + 1][1]:
            for t in range(maxTimeStep):
                if t == nodeTimeStepDict[p+1][0]:
                    model += xV[p][t][t] == 1
                else:
                    model += xV[p][t][t] == 0
        else:
            for t in range(Sn, Ln+1):
                sum_x_ptt += xV[p][t][t]
            model += sum_x_ptt == 1

    return model


def addNotAllowOthersConstraint(model, xV, pointNums, maxTimeStep, nodeTimeStepDict):  # 排他性
    for p in range(pointNums):
        for firstNodeTime in range(maxTimeStep):
            for nextNodeTime in range(maxTimeStep):
                if firstNodeTime != nextNodeTime:
                    for routingTime in range(maxTimeStep):
                        model += xV[p][firstNodeTime][firstNodeTime] + xV[p][nextNodeTime][routingTime] <= 1

    # for p in range(pointNums):
    #     if nodeTimeStepDict[p+1][0] == nodeTimeStepDict[p+1][1]:
    #         for t in range(maxTimeStep):
    #             if t == nodeTimeStepDict[p+1][0]:
    #                 model += xV[p][t][t] == 1
    #             else:
    #                 model += xV[p][t][t] == 0

    return model


def PECantWithMemoryConstraint(model, pointNums):  # PE与memory路由互斥

    for p in range(pointNums):
        Sn, Ln = nodeTimeStepDict[p + 1][0], nodeTimeStepDict[p + 1][1]
        for timeStep in range(Sn, Ln+1):
            for j1 in range(Sn+1, Ln):
                for j2 in range(Sn + 1, Ln):
                    model += xList[p][timeStep][j1] + yList[p][timeStep][j2] <= 1

    return model


def LoadStoreBondCons(model, yList, zList, pointNums, nodeTimeStepDict):
    for p in range(pointNums):
        Sn, Ln = nodeTimeStepDict[p + 1][0], nodeTimeStepDict[p + 1][1]
        for timeStep in range(Sn, Ln+1):
            for j1 in range(Sn+1, Ln):
                for j2 in range(j1 + 1, Ln):
                    model += yList[p][timeStep][j1] - zList[p][timeStep][j2] == 0

    return model


def PENumsCons(model, pe, pointNums, xList, yList, zList, maxTimeStep, II):
    for iiTime in range(II):
        sum_xyz = 0
        for p in range(pointNums):
            Sn, Ln = nodeTimeStepDict[p + 1][0], nodeTimeStepDict[p + 1][1]
            # for t in range(Sn, Ln+1):
            # print("t/ii")
            # print(int(maxTimeStep/II))
            for t in range(Sn, maxTimeStep):
                for times in range(int(maxTimeStep/II)+1):
                    # if Sn < times*II+iiTime < Ln:
                    if times * II + iiTime < maxTimeStep:
                        sum_xyz += xList[p][t][times*II+iiTime]
                        # sum_xyz += yList[p][t][times*II+iiTime]
                        # sum_xyz += zList[p][t][times*II+iiTime]
        # print(sum_xyz)
        model += sum_xyz <= pe
    return model


def addDependenceCons(model, G):
    # Dependence 1:

    global latest_Ln
    for fatherNode in G:  # 子节点减父节点路由加执行时间步<=0
        # print(fatherNode)

        Sn_f, Ln_f = nodeTimeStepDict[int(fatherNode)][0], nodeTimeStepDict[int(fatherNode)][1]
        childList = G[fatherNode]
        for childNode in childList:
            Sn_c, Ln_c = nodeTimeStepDict[int(childNode)][0], nodeTimeStepDict[int(childNode)][1]

            sum_child = 0
            for i2 in range(Sn_c, Ln_c+1):
                sum_child += i2 * xList[int(childNode)-1][i2][i2]
            for i1 in range(Sn_f, Ln_f+1):
                for j1 in range(Sn_f, Ln_f+1):
                    model += i1*xList[int(fatherNode)-1][i1][j1] + 1 <= sum_child  # + 1

    # Dependence 2
    model1 = LpProblem("prob", LpMinimize)
    for fatherNode in G:

        Sn_f, Ln_f = nodeTimeStepDict[int(fatherNode)][0], nodeTimeStepDict[int(fatherNode)][1]
        if Sn_f == Ln_f:
            continue

        childList = G[fatherNode]

        latest_Sn = 0
        latest_child = -1

        if not childList:
            continue
        else:
            for childNode in childList:
                Sn_c, Ln_c = nodeTimeStepDict[int(childNode)][0], nodeTimeStepDict[int(childNode)][1]

                if Sn_c > latest_Sn:
                    latest_Sn = Sn_c
                    latest_Ln = Ln_c
                    latest_child = childNode
            for i2 in range(latest_Sn, latest_Ln):
                for i1 in range(Sn_f, i2):
                    for j1 in range(i1, i2):
                        model  += j1 * xList[int(fatherNode) - 1][i1][j1] + 1 <= i2 * xList[int(latest_child) - 1][i2][i2]
                        model1 += j1 * xList[int(fatherNode) - 1][i1][j1] + 1 <= i2 * xList[int(latest_child) - 1][i2][i2]
    print("==============================model1===============================")
    print(model1)
    return model


def addTargetFunc(model, xList, yList, zList, alpha, G, nPE, pointNums):
    nins = 0
    for node in G:
        Sn, Ln = nodeTimeStepDict[int(node)][0], nodeTimeStepDict[int(node)][1]
        for time in range(Sn, Ln):
            for j in range(time, Ln):
                nins += xList[int(node)-1][time][j] + \
                        alpha*(yList[int(node)-1][time][j] + zList[int(node)-1][time][j])

    beta = 0
    checkedList = []
    for father in G:
        if father in checkedList:
            continue
        Sn_f, Ln_f = nodeTimeStepDict[int(father)][0], nodeTimeStepDict[int(father)][1]
        childList = G[father]
        for child in childList:
            if child in checkedList:
                continue
            Sn_c, Ln_c = nodeTimeStepDict[int(child)][0], nodeTimeStepDict[int(child)][1]
            if Sn_c != Ln_c and Sn_f != Sn_f:
                beta += Ln_c - Sn_f - 1
                checkedList.append(child)
                checkedList.append(father)
            elif Sn_c != Ln_c and Sn_f == Sn_f:
                beta += Ln_c - Sn_c
                checkedList.append(child)
            elif Sn_c == Ln_c and Sn_f != Sn_f:
                beta += Ln_f - Sn_f
                checkedList.append(father)
    print("beta")
    print(beta)
    # model += beta*nPE - nins
    model += nPE
    return model


def computingII():



    return II


if __name__ == '__main__':
    pointNums = 11
    maxTimeStep = 8
    alpha = 2
    II = 3
    maxPE = 16

    read("./in")
    write("./out")
    pre()  # 预处理得到精简图
    printG(ORIGIN_NODE_GRAPH)
    print(NEW_NODE_GRAPH)

    model, xList, yList, zList, pe = generateModelAndVariables(pointNums, maxTimeStep, maxPE)
    nodeTimeStepDict = readSnLn()
    print(nodeTimeStepDict)
    model = addUniqueConstraint(model, xList, pointNums, nodeTimeStepDict)

    model = addNotAllowOthersConstraint(model, xList, pointNums, maxTimeStep, nodeTimeStepDict)
    model = PENumsCons(model, pe, pointNums, xList, yList, zList, maxTimeStep, II)
    # model = LoadStoreBondCons(model, yList, zList, pointNums, nodeTimeStepDict)
    # model = PECantWithMemoryConstraint(model, pointNums)
    model = addDependenceCons(model, ORIGIN_NODE_GRAPH)
    print(model)
    model = addTargetFunc(model, xList, yList, zList, alpha, ORIGIN_NODE_GRAPH, pe, pointNums)
    model.solve()
    print("=========================输出结果=========================")
    print(pe.varValue)
    for i in range(pointNums):
        timeList = []
        for j in range(maxTimeStep):
            timeList.append(j * xList[i][j][j].varValue)
        temp_step = int(sum(timeList))
        # 把结果添加到NODE_LIST中的节点信息中
        NODE_LIST[i].t_step = temp_step
        print(f"{i + 1} 节点时间步是 {temp_step}")

