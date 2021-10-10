import math
import os
import struct
import time
from ctypes import *

import bmesh
import bpy
import mathutils
from bpy.props import BoolProperty, EnumProperty, StringProperty
from bpy.types import Operator
from bpy_extras.io_utils import ImportHelper, axis_conversion

bl_info = {
    "name": "SeaDogs PTC",
    "description": "Import PTC files",
    "author": "Artess999",
    "version": (0, 0, 1),
    "blender": (2, 92, 0),
    "location": "File > Import",
    "warning": "",
    "support": "COMMUNITY",
    "category": "Import",
}

correction_matrix = axis_conversion(
    from_forward='X', from_up='Y', to_forward='Y', to_up='Z')


class PTC:
    class PtcHeader(Structure):
        _fields_ = [
            ("id", c_int),
            ("space0", c_int),
            ("ver", c_int),
            ("space1", c_int),
            ("isSea", c_int),
            ("space2", c_int),
            ("numTriangles", c_int),
            ("numVerteces", c_int),
            ("numNormals", c_int),
            ("mapL", c_int),
            ("mapW", c_int),
            ("numIndeces", c_int),
            ("lineSize", c_int),
            ("minX", c_float),
            ("minY", c_float),
            ("minZ", c_float),
            ("maxX", c_float),
            ("maxY", c_float),
            ("maxZ", c_float),
        ]

    class PtcTriangle(Structure):
        _fields_ = [
            ("i", c_ushort * 3),
            ("n", c_ushort),
            ("nb", c_short * 3),
            ("mtl", c_byte),
            ("flags", c_char),
        ]

    class PtcVertex(Structure):
        _fields_ = [
            ("x", c_float),
            ("y", c_float),
            ("z", c_float),
        ]

    class PtcNormal(Structure):
        _fields_ = [
            ("x", c_float),
            ("y", c_float),
            ("z", c_float),
        ]

    class PtcMap(Structure):
        _fields_ = [
            ("start", c_int),
            ("size", c_int),
        ]

    class PtcMaterials(Structure):
        _fields_ = [
            ("numMaterials", c_int),
            ("material", (c_char * 16) * 16),
        ]

    def __init__(self, file_path=""):
        with open(file_path, mode='rb') as file:
            self.ptcHeader = self.PtcHeader()
            file.readinto(self.ptcHeader)

            self.triangle = []
            for i in range(self.ptcHeader.numTriangles):
                ptcTriangle = self.PtcTriangle()
                file.readinto(ptcTriangle)
                self.triangle.append(ptcTriangle)

            self.vertex = []
            for i in range(self.ptcHeader.numVerteces):
                ptcVertex = self.PtcVertex()
                file.readinto(ptcVertex)
                self.vertex.append(ptcVertex)

            self.normal = []
            for i in range(self.ptcHeader.numNormals):
                ptcNormal = self.PtcNormal()
                file.readinto(ptcNormal)
                self.normal.append(ptcNormal)

            self.map = []
            for i in range(self.ptcHeader.mapL * self.ptcHeader.mapW):
                map = self.PtcMap()
                file.readinto(map)
                self.map.append(map)

            self.indeces = []
            for i in range(self.ptcHeader.numIndeces):
                idx = struct.unpack("<H", file.read(2))[0]
                self.indeces.append(idx)

            self.table = []
            for i in range(self.ptcHeader.lineSize * self.ptcHeader.numTriangles):
                cell = struct.unpack("<B", file.read(1))[0]
                self.table.append(cell)

            self.materials = self.PtcMaterials()
            file.readinto(self.materials)

            self.preparedMaterials = []
            for word in self.materials.material:
                self.preparedMaterials.append(word.value.decode("utf-8"))


def import_ptc(context, file_path=""):
    file_name = os.path.basename(file_path)[:-4]
    ptc = PTC(file_path)

    collection = bpy.data.collections.new(file_name)
    bpy.context.scene.collection.children.link(collection)

    root = bpy.data.objects.new("root", None)
    collection.objects.link(root)

    objects_triangles = []
    current_object_triangles = []
    current_material = ptc.triangle[0].mtl

    for trg in ptc.triangle:
        if trg.mtl != current_material:
            objects_triangles.append(current_object_triangles)
            current_object_triangles = []
            current_material = trg.mtl
        current_object_triangles.append(trg)
    objects_triangles.append(current_object_triangles)

    for objects_triangles in objects_triangles:
        me = bpy.data.meshes.new(file_name)
        ob = bpy.data.objects.new(file_name, me)
        ob.parent = root

        bm = bmesh.new()
        bm.from_mesh(me)
        uv_layer = bm.loops.layers.uv.new()

        collection.objects.link(ob)
        bpy.context.view_layer.objects.active = ob

        bpy.ops.object.mode_set(mode='EDIT', toggle=False)

        material_name_idx = objects_triangles[0].mtl
        mtl = bpy.data.materials.new(name=ptc.preparedMaterials[material_name_idx])

        mtl.use_nodes = True
        bsdf = mtl.node_tree.nodes["Principled BSDF"]
        tex = mtl.node_tree.nodes.new('ShaderNodeTexImage')

        placeholder_image = bpy.data.images.new('checker.tga', width=1, height=1)
        placeholder_image.pixels = [0.5,0.5,0.5,1]
        tex.image = placeholder_image
        mtl.node_tree.links.new(
                        bsdf.inputs['Base Color'], tex.outputs['Color'])

        ob.data.materials.append(mtl)

        for vertex in ptc.vertex:
            vtx = bm.verts.new([-vertex.x, vertex.y, vertex.z])

        bm.verts.ensure_lookup_table()
        for f in objects_triangles:
            try:
                face = bm.faces.new(
                    (bm.verts[f.i[1]], bm.verts[f.i[0]], bm.verts[f.i[2]])) # opposite
                face.material_index = 0
                normal = ptc.normal[f.n]
                face.normal = mathutils.Vector((-normal.x, normal.y, normal.z))

            except Exception as e:
                print(str(e))

        bmesh.ops.rotate(bm, verts=bm.verts, cent=(
            0.0, 0.0, 0.0), matrix=correction_matrix)

        bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
        bm.to_mesh(me)
        bm.free()

    return {'FINISHED'}


class ImportPtc(Operator, ImportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "import.ptc"
    bl_label = "Import PTC"

    # ImportHelper mixin class uses this
    filename_ext = ".ptc"

    filter_glob: StringProperty(
        default="*.ptc",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return import_ptc(context, self.filepath)


def menu_func_import(self, context):
    self.layout.operator(ImportPtc.bl_idname,
                         text="PTC Import(.ptc)")


def register():
    bpy.utils.register_class(ImportPtc)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportPtc)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
    register()
