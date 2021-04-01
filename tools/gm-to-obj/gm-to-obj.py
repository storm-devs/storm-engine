from ctypes import *
import os
# Emulate all c structs and unions


class c_deep(Structure):
    _fields_ = [
        ('x', c_float),
        ('y', c_float),
        ('z', c_float),
    ]


class c_vector(Union):
    _fields_ = [
        ('xyz', c_deep),
        ('v', c_float*3)
    ]


class rdf_head(Structure):
    _fields_ = [('version', c_long),
                ('flags', c_long),
                ('name_size', c_long),
                ('names', c_long),
                ('ntextures', c_long),
                ('nmaterials', c_long),
                ('nlights', c_long),
                ('nlabels', c_long),
                ('nobjects', c_long),
                ('ntriangles', c_long),
                ('nvrtbuffs', c_long),
                ('bbox_size', c_vector),
                ('bbox_center', c_vector),
                ('radius', c_float)]


class rdf_texture_type(Structure):
    (TEXTURE_NONE, TEXTURE_BASE, TEXTURE_NORMAL,
     TEXTURE_FORCE_DWORD) = (0, 1, 2, 0x7FFFFFFF)


class rdf_material(Structure):
    _fields_ = [('group_name', c_long),
                ('name', c_long),
                ('diffuse', c_float),
                ('specular', c_float),
                ('gloss', c_float),
                ('selfIllum', c_float),
                ('texture_type', c_long*4),  # rdf_texture_type * 4),
                ('texture', c_long*4)]


class rdf_light_type(Structure):
    (LIGHT_POINT, LIGHT_SPOT, LIGHT_DIRECTIONAL,
     LIGHT_FORCE_DWORD) = (0, 1, 2, 0x7FFFFFFF)


class rdf_light(Structure):
    _fields_ = [('flags', c_long),
                ('type', c_long),  # rdf_light_type),
                ('group_name', c_long),
                ('name', c_long),
                ('r', c_float),
                ('g', c_float),
                ('b', c_float),
                ('range', c_float),
                ('pos', c_vector),
                ('atten', c_float*3),
                ('inner', c_float),
                ('outer', c_float),
                ('falloff', c_float),
                ('dir', c_vector),
                ]


class rdf_label(Structure):
    _fields_ = [('group_name', c_long),
                ('name', c_long),
                ('flags', c_long),
                ('m', c_float*16),
                ('bones', c_long*4),
                ('weight', c_float*4)]


class rdf_object(Structure):
    _fields_ = [('group_name', c_long),
                ('name', c_long),
                ('flags', c_long),
                ('center', c_vector),
                ('radius', c_float),
                ('vertex_buff', c_long),
                ('ntriangles', c_long),
                ('striangle', c_long),
                ('nvertices', c_long),
                ('svertex', c_long),
                ('material', c_long),
                ('lights', c_long*8),
                ('bones', c_long*4),
                ('atriangles', c_long)]


class rdf_triangle(Structure):
    _fields_ = [
        ('vindex', c_ushort*3)
    ]


class rdf_vertexbuff(Structure):
    _fields_ = [
        ('type', c_long),
        ('size', c_long)
    ]


class rdf_vertex0(Structure):
    _fields_ = [
        ('pos', c_vector),
        ('norm', c_vector),
        ('color', c_long),
        ('tu0', c_float),
        ('tv0', c_float)
    ]


class rdf_bsphead(Structure):
    _fields_ = [
        ('nnodes', c_long),
        ('nvertices', c_long),
        ('ntriangles', c_long),
    ]


class bsp_node(Structure):
    _fields_ = [
        ('norm', c_vector),
        ('pd', c_long),
        ('node', c_ulong),
        ('sign', c_ulong),
        ('left', c_ulong),
        ('nfaces', c_ulong),
        ('right', c_ulong),
        ('type', c_ulong),
        ('face', c_long),
    ]


class rdf_bsptriangle(Structure):
    _fields_ = [
        ('vindex', c_char*9)
    ]


class vertex_buffer(Structure):
    _fields_ = [
        ('type', c_ulong),
        ('size', c_long),
        ('stride', c_ulong),
        ('nverts', c_ulong),
        ('dev_buff', c_ulong)
    ]


def printArray(arr):
    # Print all properties of an arr/obj
    for o in arr:
        print("___________________")
        for field_name, field_type in o._fields_:
            print(field_name, getattr(o, field_name))


def getArrayof(struct, size):
    # Read an array of the struct type with the fiven size
    res = []
    for _ in range(0, size):
        m = eval(struct)()
        f.readinto(m)
        res.append(m)
    return res


user_input = input("Enter path of the .gm file ")
assert os.path.exists(user_input), "File not found at: "+str(user_input)

# Open the file to read
f = open(user_input, "rb")
# Get the header with all the info
rhead = rdf_head()
f.readinto(rhead)


# Read all the following lines till the buffer
globname = f.read(getattr(rhead, "name_size"))
names = f.read(getattr(rhead, "names") * sizeof(c_long))
tname = f.read(getattr(rhead, "ntextures") * sizeof(c_long))
rmaterials = getArrayof("rdf_material", rhead.nmaterials)
rlights = getArrayof("rdf_light", rhead.nlights)
rlabels = getArrayof("rdf_label", rhead.nlabels)
robjects = getArrayof("rdf_object", rhead.nobjects)
rtriangles = getArrayof("rdf_triangle", rhead.ntriangles)
rvb = getArrayof("rdf_vertexbuff", rhead.nvrtbuffs)


# Find all the vertices
vertices = []
for v in range(0, rhead.nvrtbuffs):
    size = getattr(rvb[v], "size")
    stride = sizeof(rdf_vertex0) + \
        (rvb[v].type & 3) * 8 + (rvb[v].type >> 2) * 8
    nverts = int(size/stride)
    vertices.extend(getArrayof("rdf_vertex0", nverts))

# Start transforming to obj

print("File read succesfully!")
# Each .obj has the following values


class ProperObject:
    def __init__(self, name):
        self.o = "o "+str(name)+"\n"
        self.v = ""
        self.vn = ""
        self.vt = ""
        self.f = ""

    def addV(self, x, y, z):
        self.v += "v %f %f %f\n" % (x, y, z)

    def addVN(self, x, y, z):
        self.vn += "vn %f %f %f\n" % (x, y, z)

    def addVT(self, u, v):
        self.vt += "vt %f %f\n" % (u, v)

    def addF(self, shift, x, y, z):
        self.f += "f %d %d %d\n" % (x+1+shift, y+1+shift, z+1+shift)


triangleObjects = {}
vertexObjects = {}

print("Parsing objects")
for obj in robjects:
    triangleObjects[getattr(obj, "striangle")] = {
        "name": getattr(obj, "name"),
        "shift": getattr(obj, "svertex")
    }
    vertexObjects[getattr(obj, "svertex")] = getattr(obj, "name")

g = {}

print("Parsing vertices")
objIndex = vertexObjects[0]
vLen = len(vertices)
for index, vert in enumerate(vertices):
    print("Vertex %d of %d" % (index, vLen), end="\r")
    if(index in vertexObjects):
        objIndex = vertexObjects[index]
        g[objIndex] = ProperObject(objIndex)

    g[objIndex].addV(*getattr(getattr(vert, "pos"), "v"))
    g[objIndex].addVN(* getattr(getattr(vert, "norm"), "v"))
    g[objIndex].addVT(getattr(vert, "tu0"),  getattr(vert, "tv0"))

print("Parsing triangles              ")
objIndex = triangleObjects[0]
tLen = len(rtriangles)
for index, tri in enumerate(rtriangles):
    print("Triangle %d of %d" % (index, vLen), end="\r")
    if(index in triangleObjects):
        objIndex = triangleObjects[index]
    g[objIndex.get("name")].addF(
        objIndex.get("shift"), *getattr(tri, "vindex"))


print("Writing output               ")
out = open("output.obj", "w")
for key, value in g.items():
    out.write(value.o)
    out.write(value.v)
    out.write(value.vt)
    out.write(value.vn)
    out.write(value.f)

out.close()

print("Done")
