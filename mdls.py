import argparse

def strfloat(x):
    return str(float(x)) + "f"

parser = argparse.ArgumentParser()
parser.add_argument("path", help="Path to model file", type=str)
parser.add_argument("vpath", help="Path to vertecies file", type=str)
parser.add_argument("ipath", help="Path to idices file", type=str)
args = parser.parse_args()
path = args.path
vpath = args.vpath
ipath = args.ipath

v = []
vt = []
vn = []
vertecies = []
indices = []

vertecies_count = 0

with open(path, "r") as file:
    text = file.read().split("\n")

for raw_line in text:
    line = raw_line.split()

    if len(line) <= 1:
        continue
    elif line[0] == "v":
        v.append(", ".join(list(map(strfloat, line[1:]))))
    elif line[0] == "vt":
        vt.append(", ".join(list(map(strfloat, line[1:]))))
    elif line[0] == "vn":
        vn.append(", ".join(list(map(strfloat, line[1:]))))
    elif line[0] == "f":
        for raw_vertex in line[1:]:
            vertex = list(map(int, raw_vertex.split("/")))

            vertecies.append(v[vertex[0] - 1] + ", " + vn[vertex[2] - 1] + ", " + vt[vertex[1] - 1])

        if len(line[1:]) == 3:
            indices.append(f"{len(vertecies) - 3}, {len(vertecies) - 2}, {len(vertecies) - 1}")
        elif len(line[1:]) == 4:
            indices.append(f"{len(vertecies) - 3}, {len(vertecies) - 2}, {len(vertecies) - 1}")
            indices.append(f"{len(vertecies) - 3}, {len(vertecies) - 1}, {len(vertecies) - 4}")

with open(ipath, "w") as file:
    file.write(", ".join(vertecies))

with open(vpath, "w") as file:
    file.write(", ".join(indices))