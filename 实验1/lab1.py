# coding=utf-8
# author：dzzhyk
# date：2020年10月25日16:33:48
# title：lab1.py

import pulp
CHILD_NUMBER = 4    # 最大子节点数量
NODE_LIST = []      # 节点列表
ORIGIN_NODE_GRAPH = {}     # 原始图
NEW_NODE_GRAPH = {}        # 结果图


# 子节点类
class ChildNode:
    def __init__(self, node_id, edge_type):
        self.node_id = node_id
        self.edge_type = edge_type

    def __str__(self):
        return "[ChildNode] node_id=" + str(self.node_id) + " edge_type=" + str(self.edge_type)

# 节点类
class Node:
    def __init__(self, _id, child_list, t_start, t_end, node_type, has_parent):
        self.now_id = _id               # 现节点编号 
        self.origin_id = _id            # 原节点编号
        self.child_list = child_list    # 子节点列表
        self.t_start = t_start          # 开始
        self.t_end = t_end              # 结束
        self.node_type = node_type      # 节点类型
        self.has_parent = has_parent    # 是否有父节点
        self.t_step = t_start-1         # 时间步初始化为t_start-1

    def __str__(self):
        return "[Node] now_id=" + str(self.now_id) + " origin_id=" + str(self.origin_id) + " child_list=[" + ",".join(str(c) for c in self.child_list) + "]"+\
         " t_start=" + str(self.t_start) + " t_end=" + str(self.t_end) + " node_type=" +\
          str(self.node_type) + " has_parent=" + str(self.has_parent) + "\n"

    def toFile(self):
        ss = ""
        ss += str(self.now_id) + ","
        for i in range(CHILD_NUMBER):
            ss += (str(self.child_list[i].node_id) + ",")
        ss += str(self.node_type) + ","
        ss += str(self.origin_id) + "\n"
        return ss
        
        

# 读入函数
def read(filename):
    with open(filename, "r", encoding="utf-8") as fin:
        print("<=== 读入文件：" + filename)
        while True:
            temp = fin.readline()
            if temp=="":
                print("***读入完成***")
                break
            temp_ls = temp.split(",")
            pos=0
            temp_id = int(temp_ls[pos])
            pos += 1
            temp_child = []
            for i in range(CHILD_NUMBER):
                temp_child.append(ChildNode(int(temp_ls[i*2+1]), int(temp_ls[i*2+2])))
            pos += CHILD_NUMBER*2
            temp_start = int(temp_ls[pos])
            pos+=1
            temp_end = int(temp_ls[pos])
            pos+=1
            temp_type = int(temp_ls[pos])
            pos+=1
            temp_parent = int(temp_ls[pos])
            NODE_LIST.append(Node(temp_id, temp_child, temp_start, temp_end, temp_type, temp_parent))
        fin.close()


# 输出函数
def write(filename):
    with open(filename, "w", encoding="utf-8") as fout:
        print("===> 输出目标：" + filename)
        for node in NODE_LIST:
            fout.write(str(node))
        for node in NODE_LIST:
            fout.write(node.toFile())
        
        fout.flush()
        fout.close()
    print("***写出完成***")


# 预处理输入数据得到原始图
def pre():
    for node in NODE_LIST:
        temp = []
        for child in node.child_list:
            temp.append(child.node_id)
        ORIGIN_NODE_GRAPH[node.origin_id] = temp

# 输出精简图信息
def printG(G):
    for dic in G:
        print(dic, end=": ")
        print(G[dic])

# 图拓扑排序
def TopologicalSort():
    pass

# 图逆拓扑排序
def reverse_topological():
    pass


# ASAP调度
def asap():
    pass


# ALAP
def alap():
    pass

if __name__ == "__main__":
    read("./in")
    # write("./out")
    pre()   # 预处理得到精简图
    printG(ORIGIN_NODE_GRAPH)