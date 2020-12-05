import numpy as np

for k in range(100):
    with open(str(k), "w") as td:
        for i in range(8):
            for j in range(8*16):
                td.write(str(np.random.randint(0, 255)))
                td.write(" ")
            td.write("\n")
    td.close()

