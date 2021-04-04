import argparse
from ctypes import *
import os
import ntpath
# Emulate all c structs and unions
# The c version can be found under storm-engine/src/libs/Geometry/geom_static.cpp


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


def print_array(arr):
    # Print all properties of an arr/obj
    for o in arr:
        print("___________________")
        for field_name, field_type in o._fields_:
            print(field_name, getattr(o, field_name))


def gm_to_obj(input_name, output_name):
    # Open the file to read
    f = open(input_name, "rb")

    def get_array_of(struct, size):
        # Read an array of the struct type with the fiven size
        res = []
        for _ in range(0, size):
            m = eval(struct)()
            f.readinto(m)
            res.append(m)
        return res

    # Get the header with all the info
    rhead = rdf_head()
    f.readinto(rhead)

    # Read all the following lines till the buffer
    globname = f.read(getattr(rhead, "name_size"))
    names = f.read(getattr(rhead, "names") * sizeof(c_long))
    tname = f.read(getattr(rhead, "ntextures") * sizeof(c_long))
    rmaterials = get_array_of("rdf_material", rhead.nmaterials)
    rlights = get_array_of("rdf_light", rhead.nlights)
    rlabels = get_array_of("rdf_label", rhead.nlabels)
    robjects = get_array_of("rdf_object", rhead.nobjects)
    rtriangles = get_array_of("rdf_triangle", rhead.ntriangles)
    rvb = get_array_of("rdf_vertexbuff", rhead.nvrtbuffs)

    # Find all the vertices
    vertices = []
    for v in range(0, rhead.nvrtbuffs):
        size = getattr(rvb[v], "size")
        stride = sizeof(rdf_vertex0) + \
            (rvb[v].type & 3) * 8 + (rvb[v].type >> 2) * 8
        nverts = int(size/stride)
        vertices.extend(get_array_of("rdf_vertex0", nverts))
    v_len = len(vertices)
    t_len = len(rtriangles)

    # Start transforming to obj
    print("File read succesfully!")
    # Each .obj has the following values

    # All the properties which a .obj file has. https://en.wikipedia.org/wiki/Wavefront_.obj_file
    class ProperObj:
        def __init__(self, name, vertex_shift, triangle_shift):
            self.name = name
            self.vertex_shift = vertex_shift
            self.triangle_shift = triangle_shift
            self.o = "o "+str(name)+"\n"
            self.v = ""
            self.vn = ""
            self.vt = ""
            self.f = ""
        # List of vertices

        def add_v(self, x, y, z):
            self.v += "v %f %f %f\n" % (x, y, z)

        # List of vertices' normals
        def add_vn(self, x, y, z):
            self.vn += "vn %f %f %f\n" % (x, y, z)

        # List of the texture coordinates
        def add_vt(self, u, v):
            self.vt += "vt %f %f\n" % (u, v)

        # List of the faces
        def add_f(self, shift, x, y, z):
            self.f += "f %d %d %d\n" % (x+1+shift, y+1+shift, z+1+shift)

    # Dictionaries used to store objects' start and end
    list_objects = []

    # Get instructions from objects
    print("Parsing objects")
    for obj in robjects:
        list_objects.append(ProperObj(getattr(obj, "name"), getattr(
            obj, "svertex"), getattr(obj, "striangle")))

    # Groups are stored in this dictionary
    print("Parsing vertices")
    objects_iterator = iter(list_objects)
    selected_object = next(objects_iterator)
    past_index = 0
    # Transform vertex to valid .obj structure
    for index, vert in enumerate(vertices):
        print("Vertex %d of %d" % (index, v_len), end="\r")
        if(index > selected_object.vertex_shift+past_index):
            past_index += selected_object.vertex_shift
            selected_object = next(objects_iterator)

        selected_object.add_v(*getattr(getattr(vert, "pos"), "v"))
        selected_object.add_vn(* getattr(getattr(vert, "norm"), "v"))
        selected_object.add_vt(getattr(vert, "tu0"),  getattr(vert, "tv0"))

    # Transform triangles to valid .obj structure
    print("Parsing triangles              ")
    objects_iterator = iter(list_objects)
    selected_object = next(objects_iterator)
    past_index = 0
    for index, tri in enumerate(rtriangles):
        print("Triangle %d of %d" % (index, t_len), end="\r")
        if(index > selected_object.triangle_shift+past_index):
            past_index += selected_object.triangle_shift
            selected_object = next(objects_iterator)
        selected_object.add_f(
            selected_object.triangle_shift, *getattr(tri, "vindex"))

    # start crating obj file
    print("Writing output               ")
    out = open(output_name, "w")

    out.write("# Author: https://github.com/MangioneAndrea \n")
    # Add smoothing
    out.write("s 1 \n")
    # Write everything to file
    for obj in list_objects:
        out.write(obj.o)
        out.write(obj.v)
        out.write(obj.vt)
        out.write(obj.vn)
        out.write(obj.f)

    out.close()

    print("Done")


if (__name__ == "__main__"):
    # define args
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "path", help="Where is the .gm file located?", type=str)
    parser.add_argument("--output", "-o", help="Output name", type=str)
    args = parser.parse_args()
    args_input = str(args.path)
    args_output = args.output
    if(args_output is None):
        args_output = ntpath.basename(args_input).replace(".gm", ".obj")
    gm_to_obj(args_input, args_output)
