import math
import os
import struct
import time
import re
import copy
import ctypes
import sys

import bmesh
import bpy
import mathutils
from bpy.props import BoolProperty, EnumProperty, StringProperty
from bpy.types import Operator
from bpy_extras.io_utils import ExportHelper, axis_conversion

sys.setrecursionlimit(10000)

bl_info = {
    "name": "SeaDogs GM",
    "description": "Export GM files",
    "author": "Artess999",
    "version": (0, 9, 9),
    "blender": (2, 92, 0),
    "location": "File > Export",
    "warning": "",
    "support": "COMMUNITY",
    "category": "Export",
}

# TODO 1024 in sources
MAX_TREE_DEPTH = 2048
MAX_PLANES_CALCULATED = 256
PRECISION = 0.00001
# TODO 0.0001 or 0.00001 in different sources
LIE_PREC = 0.0001
MAX_PLANE_FACES = 15
BSP_NODE_SIZE = 24

correction_export_matrix = axis_conversion(
    from_forward='Y', from_up='Z', to_forward='X', to_up='Y')

nodes_to_skip = 0


def get_bounding_box_coords(objects, x_is_mirrored):
    bound_box = objects[0].bound_box

    x1 = bound_box[0][1]
    x2 = bound_box[0][1]
    y1 = bound_box[0][2]
    y2 = bound_box[0][2]
    z1 = bound_box[0][0]
    z2 = bound_box[0][0]

    for obj in objects:
        bound_box = obj.bound_box

        for [x, y, z] in bound_box:
            if y < x1:
                x1 = y
            if y > x2:
                x2 = y
            if z < y1:
                y1 = z
            if z > y2:
                y2 = z
            if x < z1:
                z1 = x
            if x > z2:
                z2 = x

    if x_is_mirrored:
        tmp_x1 = x1
        x1 = -x2
        x2 = -tmp_x1

    return {
        "x1": x1,
        "x2": x2,
        "y1": y1,
        "y2": y2,
        "z1": z1,
        "z2": z2,
    }


def get_box_size(bounding_box_coords):
    x1 = bounding_box_coords.get("x1")
    x2 = bounding_box_coords.get("x2")
    y1 = bounding_box_coords.get("y1")
    y2 = bounding_box_coords.get("y2")
    z1 = bounding_box_coords.get("z1")
    z2 = bounding_box_coords.get("z2")

    return [x2 - x1, y2 - y1, z2 - z1]


def get_box_center(bounding_box_coords):
    x1 = bounding_box_coords.get("x1")
    x2 = bounding_box_coords.get("x2")
    y1 = bounding_box_coords.get("y1")
    y2 = bounding_box_coords.get("y2")
    z1 = bounding_box_coords.get("z1")
    z2 = bounding_box_coords.get("z2")

    return [(x2 + x1) / 2, (y2 + y1) / 2, (z2 + z1) / 2]


def get_box_radius(box_center, vertices):
    center_vector = mathutils.Vector(box_center)
    box_radius = 0

    for pos in vertices:
        pos_vector = mathutils.Vector(pos)
        [x, y, z] = center_vector - pos_vector
        radius = math.sqrt(x * x + y * y + z * z)
        if radius > box_radius:
            box_radius = radius

    return box_radius


def prepare_vertices_with_multiple_uvs(bm_verts, uv_layer):
    for vertex in bm_verts:
        uv = []
        for loop in vertex.link_loops:
            loop_index = loop.index

            if len(uv) == 0:
                uv = uv_layer.data[loop_index].uv

            if uv_layer.data[loop_index].uv != uv:
                #print(uv, obj_uv_layer.data[loop_index].uv, loop.face.index)
                #diff = uv_layer.data[loop_index].uv - uv
                # if abs(diff[0]) >= 0.01 or abs(diff[1]) >= 0.01:
                bmesh.utils.loop_separate(loop)
                bm_verts.index_update()


def get_material_data(object_data):
    try:
        obj_material = object_data.materials[0]

        name = remove_blender_name_postfix(obj_material.name)
        textures = []

        bsdf = obj_material.node_tree.links[0].from_node

        base_color_node_output = bsdf.inputs['Base Color'].links[0].from_node
        base_color_node_output_type = base_color_node_output.type

        if base_color_node_output_type == 'TEX_IMAGE':
            textures = [remove_blender_name_postfix(
                base_color_node_output.image.name)]
        elif base_color_node_output_type == 'MIX_RGB':
            texture = remove_blender_name_postfix(
                base_color_node_output.inputs['Color1'].links[0].from_node.image.name)
            texture_normals = remove_blender_name_postfix(base_color_node_output.inputs['Color2'].links[
                0].from_node.inputs['Color1'].links[0].from_node.image.name)

            textures = [texture, texture_normals]

        return {
            "name": name,
            "textures": textures
        }
    except:
        return {
            "name": 'placeholder_material',
            "textures": ['checker.tga']
        }


def remove_blender_name_postfix(name):
    return re.sub(r'\.\d{3}', '', name)


def NODESIZE(a):
    return (1+(a*3+BSP_NODE_SIZE-4)/BSP_NODE_SIZE)


class Build_bsp_node:
    min_l = 0
    min_r = 0
    min_c = 0
    min_m = 0

    def __init__(self, col):
        self.col = col

        self.right = None  # []
        self.left = None  # []
        self.norm = mathutils.Vector((0, 0, 0))
        self.pld = 0
        self.tot_faces = 0
        self._face = []

    def best_plane(self, faces, nfaces):
        res0 = 0
        res1 = 0
        res2 = 0
        best_plane = 0

        gd = 1e20

        f = 0

        for f in range(min(nfaces, MAX_PLANES_CALCULATED)):
            l = 0
            r = 0
            c = 0
            m = 0

            for i in range(nfaces):
                normal = faces[f].get("normal")
                plane_distance = faces[f].get("plane_distance")
                f_trg = self.col.trg[faces[i].get("trg")]

                res0 = normal.dot(self.col.vrt[f_trg[0]]) - plane_distance
                res1 = normal.dot(self.col.vrt[f_trg[1]]) - plane_distance
                res2 = normal.dot(self.col.vrt[f_trg[2]]) - plane_distance

                if abs(res0) < LIE_PREC and abs(res1) < LIE_PREC and abs(res2) < LIE_PREC:
                    m += 1
                    continue

                min_dist = 1e300
                max_dist = -1e300
                for v in range(faces[i].get("nvertices")):
                    dist = normal.dot(faces[i].get(
                        "vertices")[v]) - plane_distance

                    if dist > max_dist:
                        max_dist = dist
                    if dist < min_dist:
                        min_dist = dist

                if min_dist >= -PRECISION and max_dist >= -PRECISION:
                    r += 1
                    continue

                if min_dist <= PRECISION and max_dist <= PRECISION:
                    l += 1
                    continue
                c += 1

            dist = 3.0 * c + 1.0 * abs(r - l)

            if dist < gd:
                gd = dist
                best_plane = f
                Build_bsp_node.min_l = l
                Build_bsp_node.min_r = r
                Build_bsp_node.min_c = c
                Build_bsp_node.min_m = m

        return best_plane

    def best_empty_plane(self, faces, nfaces, epnorm, epdist):
        res0 = 0
        res1 = 0
        res2 = 0
        f = 0

        gd = 1e20

        for f in range(min(nfaces, MAX_PLANES_CALCULATED)):
            for e in range(3):
                e1 = e + 1
                if e1 == 3:
                    e1 = 0

                edge = faces[f].get("vertices")[e1] - \
                    faces[f].get("vertices")[e]
                bnormal = (faces[f].get(
                    "normal") * math.sqrt(edge.length_squared)).cross(edge).normalized()
                bplane_distance = bnormal.dot(faces[f].get("vertices")[e])

                l = 0
                r = 0
                c = 0
                m = 0

                for i in range(nfaces):
                    f_trg = self.col.trg[faces[i].get("trg")]

                    res0 = bnormal.dot(self.col.vrt[f_trg[0]]) - bplane_distance
                    res1 = bnormal.dot(self.col.vrt[f_trg[1]]) - bplane_distance
                    res2 = bnormal.dot(self.col.vrt[f_trg[2]]) - bplane_distance

                    if abs(res0) < LIE_PREC and abs(res1) < LIE_PREC and abs(res2) < LIE_PREC:
                        break

                    min_dist = 1e300
                    max_dist = -1e300
                    for v in range(faces[i].get("nvertices")):
                        dist = bnormal.dot(faces[i].get("vertices")[
                            v]) - bplane_distance

                        if dist > max_dist:
                            max_dist = dist
                        if dist < min_dist:
                            min_dist = dist

                    if min_dist >= -PRECISION and max_dist >= -PRECISION:
                        r += 1
                        continue

                    if min_dist <= PRECISION and max_dist <= PRECISION:
                        l += 1
                        continue
                    c += 1

                if i < nfaces:
                    continue

                dist = 3.0 * c + 1.0 * abs(r - l)

                if r > 0 and l > 0 and dist < gd:
                    gd = dist
                    Build_bsp_node.min_l = l
                    Build_bsp_node.min_r = r
                    Build_bsp_node.min_c = c
                    Build_bsp_node.min_m = m
                    epnorm = bnormal
                    epdist = bplane_distance

            return (epnorm, epdist)

    def fill_node(self, faces, nfaces):
        best_plane = self.best_plane(faces, nfaces)

        self.norm = faces[best_plane].get("normal")
        self.pld = self.norm.dot(self.col.vrt[self.col.trg[faces[best_plane].get("trg")][0]])

        if Build_bsp_node.min_m > MAX_PLANE_FACES:
            (self.norm, self.pld) = self.best_empty_plane(faces, nfaces, self.norm, self.pld)
            best_plane = -1

        rlist = []
        llist = []

        self.col.cur_node += 1
        self.col.cur_depth += 1

        if self.col.cur_depth > self.col.max_depth:
            self.col.max_depth = self.col.cur_depth

        if self.col.cur_depth > 1000:
            print("cur_depth > 1000 interrupt")

        self.tot_faces = 0

        self._face = []

        l = 0
        r = 0

        for i in range(nfaces):
            res = []

            f_trg_idx = faces[i].get("trg")
            f_trg = self.col.trg[f_trg_idx]

            res.append(self.norm.dot(self.col.vrt[f_trg[0]]) - self.pld)
            res.append(self.norm.dot(self.col.vrt[f_trg[1]]) - self.pld)
            res.append(self.norm.dot(self.col.vrt[f_trg[2]]) - self.pld)

            if abs(res[0]) < LIE_PREC and abs(res[1]) < LIE_PREC and abs(res[2]) < LIE_PREC:
                # for loop c++ increment hack
                tf = -1
                has_break = False

                for tf in range(self.tot_faces):
                    if self._face[tf] == f_trg_idx:
                        has_break = True
                        break

                # for loop c++ increment hack
                if not has_break:
                    tf += 1

                if tf == self.tot_faces:
                    self.tot_faces += 1
                    self._face.append(f_trg_idx)

                continue

            vdist = [None] * 256
            min_did = 0
            max_did = 0
            min_dist = 1e300
            max_dist = -1e300

            for v in range(faces[i].get("nvertices")):
                vdist[v] = self.norm.dot(faces[i].get("vertices")[v]) - self.pld

                if vdist[v] > max_dist:
                    max_dist = vdist[v]
                    max_did = v

                if vdist[v] < min_dist:
                    min_dist = vdist[v]
                    min_did = v

            if min_dist >= -PRECISION and max_dist >= -PRECISION:
                r += 1
                rlist.append(faces[i])
                continue

            if min_dist <= PRECISION and max_dist <= PRECISION:
                l += 1
                llist.append(faces[i])
                continue

            right_face = copy.deepcopy(faces[i])

            while vdist[max_did] > -PRECISION:
                max_did -= 1
                if max_did == -1:
                    max_did = faces[i].get("nvertices") - 1

            right_face["nvertices"] = 0
            right_face["vertices"] = []

            # do .. while hack
            while True:
                right_face["nvertices"] += 1

                if vdist[max_did] <= -PRECISION:
                    v = max_did + 1
                    if v == faces[i].get("nvertices"):
                        v = 0

                    d = vdist[max_did] / (vdist[max_did] - vdist[v])
                    right_face["vertices"].append(faces[i].get("vertices")[
                        max_did] + d * (faces[i].get("vertices")[v] - faces[i].get("vertices")[max_did]))
                else:
                    right_face["vertices"].append(
                        faces[i].get("vertices")[max_did])

                max_did += 1
                if max_did == faces[i].get("nvertices"):
                    max_did = 0

                if vdist[max_did] <= -PRECISION:
                    break

            right_face["nvertices"] += 1
            v = max_did - 1

            if v == -1:
                v = faces[i].get("nvertices") - 1

            d = vdist[max_did] / (vdist[max_did] - vdist[v])
            right_face["vertices"].append(faces[i].get("vertices")[
                max_did] + d * (faces[i].get("vertices")[v] - faces[i].get("vertices")[max_did]))
            r += 1
            rlist.append(right_face)

            # left, lol
            right_face = copy.deepcopy(faces[i])

            while vdist[min_did] < PRECISION:
                min_did -= 1
                if min_did == -1:
                    min_did = faces[i].get("nvertices") - 1

            right_face["nvertices"] = 0
            right_face["vertices"] = []

            # do .. while hack
            while True:
                right_face["nvertices"] += 1

                if vdist[min_did] >= PRECISION:
                    v = min_did + 1
                    if v == faces[i].get("nvertices"):
                        v = 0

                    d = vdist[min_did] / (vdist[min_did] - vdist[v])
                    right_face["vertices"].append(faces[i].get("vertices")[
                        min_did] + d * (faces[i].get("vertices")[v] - faces[i].get("vertices")[min_did]))
                else:
                    right_face["vertices"].append(
                        faces[i].get("vertices")[min_did])

                min_did += 1
                if min_did == faces[i].get("nvertices"):
                    min_did = 0

                if vdist[min_did] >= PRECISION:
                    break

            right_face["nvertices"] += 1
            v = min_did - 1

            if v == -1:
                v = faces[i].get("nvertices") - 1

            d = vdist[min_did] / (vdist[min_did] - vdist[v])
            right_face["vertices"].append(faces[i].get("vertices")[
                min_did] + d * (faces[i].get("vertices")[v] - faces[i].get("vertices")[min_did]))
            l += 1
            llist.append(right_face)

        self.col.ssize += int(NODESIZE(self.tot_faces))
        self.col.ndepth[self.col.cur_depth] += int(NODESIZE(self.tot_faces))

        if r > 0:
            self.right = Build_bsp_node(self.col)
            self.right.fill_node(rlist, r)
        else:
            self.right = None

        if l > 0:
            self.left = Build_bsp_node(self.col)
            self.left.fill_node(llist, l)
        else:
            self.left = None

        self.col.cur_depth -= 1

    def store(self, sroot):
        node_idx = self.col.ndepth[self.col.cdepth]
        # TODO: why deepcopy?
        node = copy.deepcopy(sroot[node_idx])

        node["sign"] = 0
        node["type"] = 0
        node["norm"] = self.norm
        node["pd"] = self.pld

        faces = self._face
        node["face"] = bytearray()
        for i in range(self.tot_faces):
            # TODO: check if overflow does not happen here
            # if it happens, this might be a point for invalid BSP faces
            face_bytes = faces[i].to_bytes(4, "little")
            node["face"].append(face_bytes[0])
            node["face"].append(face_bytes[1])
            node["face"].append(face_bytes[2])

        # TODO Must be signed=True?? FIXME 0xFFFFFFFF & overflow fix, delete later
        # node["face"] = 0xFFFFFFFF & int.from_bytes(
        #     node.get("face"), byteorder='little', signed=False)

        node["nfaces"] = self.tot_faces

        if node["nfaces"] > MAX_PLANE_FACES:
            print("Internal error: too many faces on the BSP node")

        self.col.ndepth[self.col.cdepth] += node["nfaces"]
        self.col.cdepth += 1

        node["left"] = 0
        node["node"] = 0

        if self.left != None:
            (sroot, num) = self.left.store(sroot)
            node["node"] = num
            node["left"] = 1

        if self.right != None:
            if self.left is None:
                (sroot, num) = self.right.store(sroot)
                node["node"] = num - 1
                node["right"] = 1
            else:
                (sroot, num) = self.right.store(sroot)
                node["right"] = num - node.get("node")

        self.col.cdepth -= 1

        sroot[node_idx] = node

        return (sroot, node_idx)

    def clear_static_fields():
        Build_bsp_node.min_l = 0
        Build_bsp_node.min_r = 0
        Build_bsp_node.min_c = 0
        Build_bsp_node.min_m = 0


class Collide:
    def __init__(self):
        self.nvrts = 0
        self.vrt = []
        self.ntrgs = 0
        self.trg = []
        self.cur_node = -1
        self.cur_depth = 0
        self.cdepth = 0
        self.tnode = 0
        self.max_depth = 0
        self.ndepth = [0] * MAX_TREE_DEPTH

    def add_mesh(self, vertices, faces):
        vertices_quantity = len(vertices)
        faces_quantity = len(faces)
        ref = [0] * vertices_quantity

        for face in faces:
            for vert in face:
                ref[vert] += 1

        # TODO Akella source was so messy, need to rewrote
        for vert_idx in range(vertices_quantity):
            if ref[vert_idx] > 0:

                # for loop c++ increment hack
                vert_idx_1 = -1
                has_break = False

                for vert_idx_1 in range(self.nvrts):
                    if vertices[vert_idx] == self.vrt[vert_idx_1]:
                        has_break = True
                        break

                # for loop c++ increment hack
                if not has_break:
                    vert_idx_1 += 1

                if vert_idx_1 == self.nvrts:
                    self.nvrts += 1
                    self.vrt.append(mathutils.Vector(vertices[vert_idx]))

                ref[vert_idx] = vert_idx_1

        # add triangles and relink vrtindex
        for face in faces:
            temp_face = []
            for vert in face:
                temp_face.append(ref[vert])
            self.trg.append(temp_face)
        self.ntrgs += faces_quantity

    def build_bsp(self):
        faces = []

        for t in range(self.ntrgs):
            face = {}

            vertices = []
            nvertices = 3

            for i in range(3):
                vertices.append(self.vrt[self.trg[t][i]])

            normal = ((vertices[1] - vertices[0])
                      .cross(vertices[2] - vertices[0])).normalized()

            plane_distance = vertices[0].dot(normal)

            face["nvertices"] = nvertices
            face["vertices"] = vertices
            face["normal"] = normal
            face["plane_distance"] = plane_distance
            face["trg"] = t

            faces.append(face)

        self.ssize = 0
        root = Build_bsp_node(self)
        root.fill_node(faces, self.ntrgs)

        prv = 0
        for d in range(self.max_depth):
            prv += self.ndepth[d]
            self.ndepth[d] = prv

        sroot = [{
            "norm": mathutils.Vector((0, 0, 0)),
            "pd": 0,
            "node": 0,
            "sign": 0,
            "left": 0,
            "nfaces": 0,
            "right": 0,
            "type": 0,
            "face": 0,
        }] * self.ssize
        (sroot, num) = root.store(sroot)

        return sroot


def prepare_globnames(objects, locators, materials, is_animated):
    globnames = ['unknown material group']

    for material in materials:
        name = material.get("name")
        if not (name in globnames):
            globnames.append(name)
        textures = material.get("textures")
        for texture in textures:
            if not (texture in globnames):
                globnames.append(texture)

    for object in objects:
        name = remove_blender_name_postfix(object.name)
        parent_name = remove_blender_name_postfix(object.parent.name)

        if not (name in globnames):
            globnames.append(name)
        if not (parent_name in globnames):
            globnames.append(parent_name)

    for locator in locators:
        name = remove_blender_name_postfix(locator.name)
        parent_name = remove_blender_name_postfix(locator.parent_bone) if is_animated else remove_blender_name_postfix(
            locator.parent.name)

        if not (name in globnames):
            globnames.append(name)
        if not (parent_name in globnames):
            globnames.append(parent_name)

    return globnames


def write_vector(file, value):
    [x, y, z] = value
    file.write(struct.pack('<f', x))
    file.write(struct.pack('<f', y))
    file.write(struct.pack('<f', z))


def write_color(file, value):
    [r, g, b, a] = value
    file.write(struct.pack('<B', r))
    file.write(struct.pack('<B', g))
    file.write(struct.pack('<B', b))
    file.write(struct.pack('<B', a))


def write_vertex0(file, pos, norm, color, tu0, tv0):
    write_vector(file, pos)
    write_vector(file, norm)
    write_color(file, color)

    file.write(struct.pack('<f', tu0))
    file.write(struct.pack('<f', tv0))


def write_vertex1(file, pos, norm, color, tu0, tv0, tu1, tv1):
    write_vector(file, pos)
    write_vector(file, norm)
    write_color(file, color)

    file.write(struct.pack('<f', tu0))
    file.write(struct.pack('<f', tv0))

    file.write(struct.pack('<f', tu1))
    file.write(struct.pack('<f', tv1))


def write_avertex0(file, pos, weight, bone_id, norm, color, tu0, tv0,):
    write_vector(file, pos)
    file.write(struct.pack('<f', weight))
    file.write(struct.pack('<L', bone_id))
    write_vector(file, norm)
    write_color(file, color)

    file.write(struct.pack('<f', tu0))
    file.write(struct.pack('<f', tv0))


# def write_bsp_node(file, norm, pd, node, sign, left, nfaces, right, type, face):
#     write_vector(file, norm)
#     file.write(struct.pack('<f', pd))
#     bitfields = (node & 0x3FFFFF) | ((sign & 0x1) << 22) | ((left & 0x1) << 23) | (
#         (nfaces & 0x1F) << 24) | ((right & 0x3) << 28) | ((type & 0x3) << 30)
#     file.write(struct.pack('<L', bitfields))
#     # TODO was <l originally, but it ain't working; checkme!
#     file.write(struct.pack('<L', face))

def write_bsp_node(file, norm, pd, node, sign, left, nfaces, right, type, face):
    global nodes_to_skip
    if nodes_to_skip != 0:
        nodes_to_skip -= 1
    else:
        write_vector(file, norm)
        file.write(struct.pack('<f', pd))
        bitfields = (node & 0x3FFFFF) | ((sign & 0x1) << 22) | ((left & 0x1) << 23) | (
            (nfaces & 0x1F) << 24) | ((right & 0x3) << 28) | ((type & 0x3) << 30)
        file.write(struct.pack('<L', bitfields))
        for i in range(nfaces):
            file.write(struct.pack('<B', face[i*3]))
            file.write(struct.pack('<B', face[i*3+1]))
            file.write(struct.pack('<B', face[i*3+2]))

        if nfaces == 1:
            file.write(struct.pack('<B', 0))
        else:
            exceeding_nodes_in_bytes = 3*(nfaces-1)-1
            nodes_to_skip = math.ceil(exceeding_nodes_in_bytes/BSP_NODE_SIZE)
            file.write(b'\x00' * (nodes_to_skip *
                       BSP_NODE_SIZE - exceeding_nodes_in_bytes))


def export_gm(context, file_path="", bsp=False):
    global nodes_to_skip

    bpy.ops.object.mode_set(mode='OBJECT', toggle=False)

    root = bpy.context.view_layer.objects.active
    root_children = root.children

    collection = root.users_collection[0]

    is_animated = False

    objects = []
    locators = []
    bones_list = []

    vertices = []
    faces = []
    normals = []

    types = []

    bounding_boxes = []

    materials = []
    textures = []

    objects_data = []

    vertex_buffers = []

    atriangles = 0

    col = Collide()

    # TODO get list of childrens children
    for child in root_children:
        if child.type == 'EMPTY':
            locator = child
            locator_is_root = False
            for child in locator.children:
                if child.type == 'EMPTY':
                    locator_is_root = True
                    locators.append(child)
            if not locator_is_root:
                locators.append(locator)
        if child.type == 'MESH':
            objects.append(child)
        if child.type == 'ARMATURE':
            is_animated = True

            armature_obj = child
            objects = []
            locators = []

            root_bone = child.data.bones[0]
            bones_list = root_bone.children_recursive
            bones_list.insert(0, root_bone)

            for child in armature_obj.children:
                if child.type == 'EMPTY':
                    locators.append(child)
                if child.type == 'MESH':
                    # TODO join?
                    objects.append(child)

            break

    x_is_mirrored = not is_animated

    if is_animated:
        bsp = False

    bpy.context.scene.frame_set(0)

    for object in objects:
        if bpy.context.view_layer.objects.get(object.name):
            bpy.context.view_layer.objects.active = object
            bpy.ops.object.mode_set(mode='EDIT', toggle=False)
            bpy.ops.uv.select_all(action='SELECT')
            bpy.ops.uv.seams_from_islands()
            bpy.ops.object.mode_set(mode='OBJECT', toggle=False)

    bpy.context.view_layer.objects.active = root

    # TODO multiple objects
    for object in objects:
        opposite = object.scale[0] * object.scale[1] * object.scale[2] < 0

        faces_quantity = 0
        vertices_quantity = 0

        src_obj = object

        obj = src_obj.copy()
        obj.data = src_obj.data.copy()
        obj.animation_data_clear()

        obj_data = obj.data
        obj_uv_layer = obj_data.uv_layers[0]
        obj_uv_normals_layer = obj_data.uv_layers[1] if len(
            obj_data.uv_layers) > 1 else None

        collection.objects.link(obj)

        depsgraph = bpy.context.evaluated_depsgraph_get()

        bm = bmesh.new()
        bm.from_mesh(obj.evaluated_get(depsgraph).to_mesh(
            preserve_all_data_layers=True, depsgraph=depsgraph))

        bm.verts.ensure_lookup_table()

        print('\nBefore Blender mesh export preparations:')
        print('Mesh name: ' + src_obj.name + ', vertices: ' +
              str(len(bm.verts)) + ', faces: ' + str(len(bm.faces)))

        # prepare_vertices_with_multiple_uvs(bm.verts, obj_uv_layer)

        # if obj_uv_normals_layer:
        #     prepare_vertices_with_multiple_uvs(bm.verts, obj_uv_normals_layer)

        seams = [e for e in bm.edges if e.seam]
        bmesh.ops.split_edges(bm, edges=seams)

        bm.to_mesh(obj.data)
        print('After Blender mesh export preparations:')
        print('Mesh name: ' + src_obj.name + ', vertices: ' +
              str(len(bm.verts)) + ', faces: ' + str(len(bm.faces)))

        obj_vertices = obj_data.vertices
        obj_vertices_coords = []
        obj_normals = []
        obj_polygons = obj_data.polygons
        obj_faces = []

        # TODO get active?
        obj_vertex_color = obj_data.vertex_colors[0] if len(
            obj_data.vertex_colors) > 0 else None

        obj_vertex_groups = obj.vertex_groups

        obj_uv_layer = obj_data.uv_layers[0]
        obj_uv_normals_layer = obj_data.uv_layers[1] if len(
            obj_data.uv_layers) > 1 else None

        material = get_material_data(obj_data)
        if not material in materials:
            materials.append(material)

        vertices_quantity = len(bm.verts)

        if vertices_quantity > 65536:
            bpy.data.objects.remove(obj, do_unlink=True)
            raise ValueError(
                src_obj.name + ' vertices_quantity bigger than 65536!')

        for vertex in bm.verts:
            pos = (obj.matrix_world @ mathutils.Vector(vertex.co)) - \
                mathutils.Vector(obj.parent.matrix_world.translation)
            pos.rotate(correction_export_matrix)
            if x_is_mirrored:
                pos *= mathutils.Vector([-1, 1, 1])
            vertices.append(pos)
            obj_vertices_coords.append(pos)

            norm = mathutils.Vector(vertex.normal)
            norm.rotate(correction_export_matrix)
            if x_is_mirrored:
                norm *= mathutils.Vector([-1, 1, 1])
            normals.append(norm)
            obj_normals.append(norm)

        faces_quantity = len(bm.faces)
        for face in bm.faces:
            [v1, v2, v3] = face.verts[:]
            # opposite
            face = [v1.index, v2.index, v3.index] if opposite else [
                v2.index, v1.index, v3.index]
            faces.append(face)
            obj_faces.append(face)

        obj.to_mesh_clear()
        bm.free()

        obj_weights = []
        obj_bone_ids = []

        if is_animated:
            for vertex in obj_vertices:
                bone_1 = 0
                bone_2 = 0
                weight_1 = 0
                weight_2 = 0
                for vertex_group in vertex.groups:
                    current_weight = vertex_group.weight

                    vertex_group_name = obj_vertex_groups[vertex_group.group].name
                    try:
                        vertex_bone = next(
                            filter(lambda x: x.name == vertex_group_name, bones_list))
                        vertex_group_id = bones_list.index(vertex_bone)

                        if current_weight > weight_1:
                            weight_2 = weight_1
                            weight_1 = current_weight
                            bone_2 = bone_1
                            bone_1 = int(vertex_group_id)
                        elif current_weight > weight_2:
                            weight_2 = current_weight
                            bone_2 = int(vertex_group_id)
                    except StopIteration as e:
                        print(vertex_group_name + ' is missing in armature!')

                obj_weights.append(weight_1)
                obj_bone_ids.append((bone_2 << 8) | (bone_1 << 0))

        obj_colors = [[127, 127, 127, 255]] * len(obj_vertices)
        obj_uv_array = [[0, 0]] * len(obj_vertices)

        has_uv_normals = len(material.get(
            "textures")) == 2 and obj_uv_normals_layer

        obj_uv_normals_array = [
            [0, 0]] * len(obj_vertices) if has_uv_normals else [None] * len(obj_vertices)

        for polygon in obj_polygons:
            for i, index in enumerate(polygon.vertices):
                loop_index = polygon.loop_indices[i]
                if obj_vertex_color:
                    [r, g, b, a] = obj_vertex_color.data[loop_index].color[:]
                    obj_colors[index] = [int(r*255), int(g*255),
                                         int(b*255), int(a*255)]
                obj_uv_array[index] = mathutils.Vector(
                    obj_uv_layer.data[loop_index].uv) * mathutils.Vector([1, -1])
                if has_uv_normals:
                    obj_uv_normals_array[index] = mathutils.Vector(
                        obj_uv_normals_layer.data[loop_index].uv) * mathutils.Vector([1, -1])

        bounding_box = get_bounding_box_coords([obj], x_is_mirrored)
        bounding_boxes.append(bounding_box)

        center = get_box_center(bounding_box)
        radius = get_box_radius(center, obj_vertices_coords)

        type = 0
        if is_animated:
            type = 4
        elif has_uv_normals:
            type = 1
        types.append(type)

        if len(vertex_buffers) == 0:
            vertex_buffers.append({
                "type": type,
                "index": 0,
                "vertices_quantity": 0,
                "vertices": [],
                "normals": [],
                "colors": [],
                "weights": [],
                "bone_ids": [],
                "uv_array": [],
                "uv_normals_array": []
            })

        current_vertex_buffer_idx = -1
        for idx in range(len(vertex_buffers)):
            buffer_type = vertex_buffers[idx].get("type")
            if buffer_type == type:
                current_vertex_buffer_idx = idx

        if vertex_buffers[current_vertex_buffer_idx].get("vertices_quantity") + vertices_quantity > 65536 or current_vertex_buffer_idx == -1:
            current_vertex_buffer_idx = len(vertex_buffers)
            vertex_buffers.append({
                "type": type,
                "index": current_vertex_buffer_idx,
                "vertices_quantity": 0,
                "vertices": [],
                "normals": [],
                "colors": [],
                "weights": [],
                "bone_ids": [],
                "uv_array": [],
                "uv_normals_array": []
            })

        current_vertex_buffer = vertex_buffers[current_vertex_buffer_idx]

        current_vertex_buffer_vertices_quantity = current_vertex_buffer.get(
            "vertices_quantity")

        if bsp:
            atriangles += faces_quantity
            col.add_mesh(obj_vertices_coords, obj_faces)

        object_data = {
            "vertex_buff": current_vertex_buffer.get("index"),
            "ntriangles": faces_quantity,
            "striangle": len(faces) - faces_quantity,
            "nvertices": vertices_quantity,
            "svertex": current_vertex_buffer_vertices_quantity,
            "material": materials.index(material),
            "center": center,
            "radius": radius,
            "atriangles": atriangles
        }
        objects_data.append(object_data)
        current_vertex_buffer["vertices_quantity"] = current_vertex_buffer_vertices_quantity + vertices_quantity
        current_vertex_buffer["vertices"] += obj_vertices_coords
        current_vertex_buffer["normals"] += obj_normals
        current_vertex_buffer["colors"] += obj_colors
        current_vertex_buffer["weights"] += obj_weights
        current_vertex_buffer["bone_ids"] += obj_bone_ids
        current_vertex_buffer["uv_array"] += obj_uv_array
        current_vertex_buffer["uv_normals_array"] += obj_uv_normals_array

        # TODO delete on error
        bpy.data.objects.remove(obj, do_unlink=True)

    # TODO check
    for material in materials:
        material_textures = material.get("textures")
        for material_texture in material_textures:
            if not (material_texture in textures):
                textures.append(material_texture)

    # TODO nested
    with open(file_path, 'wb') as file:
        header_version = 825110581
        file.write(struct.pack('<l', header_version))

        header_flags = 2 if bsp else 0
        file.write(struct.pack('<l', header_flags))

        globnames = prepare_globnames(
            objects, locators, materials, is_animated)

        globname = '\0'.join(globnames) + '\0'

        header_name_size = len(globname)
        file.write(struct.pack('<l', header_name_size))

        header_names_quantity = len(globnames)
        file.write(struct.pack('<l', header_names_quantity))

        header_ntextures = len(textures)
        file.write(struct.pack('<l', header_ntextures))

        header_nmaterials = len(materials)
        file.write(struct.pack('<l', header_nmaterials))

        header_nlights = 0
        file.write(struct.pack('<l', header_nlights))

        # TODO fix for nested locators
        header_nlabels = len(locators)
        file.write(struct.pack('<l', header_nlabels))

        header_nobjects = len(objects)
        file.write(struct.pack('<l', header_nobjects))

        header_ntriangles = len(faces)
        file.write(struct.pack('<l', header_ntriangles))

        header_nvrtbuffs = len(vertex_buffers)
        file.write(struct.pack('<l', header_nvrtbuffs))

        header_bounding_box = get_bounding_box_coords(objects, x_is_mirrored)
        header_bboxSize = get_box_size(header_bounding_box)
        write_vector(file, header_bboxSize)

        # TODO
        header_bboxCenter = get_box_center(header_bounding_box)
        write_vector(file, header_bboxCenter)

        # TODO
        header_radius = get_box_radius(header_bboxCenter, vertices)
        file.write(struct.pack('<f', header_radius))

        for c in globname:
            file.write(struct.pack('<s', c.encode("utf-8")))

        current_name_offset = 0
        names_offsets = [current_name_offset]
        for i in range(header_names_quantity):
            file.write(struct.pack('<l', current_name_offset))
            current_name_offset = globname.index('\0', current_name_offset) + 1
            names_offsets.append(current_name_offset)

        index_by_names = {}
        for i in range(header_names_quantity):
            offset = names_offsets[i]
            next_offset = header_name_size if i == header_names_quantity - \
                1 else names_offsets[i + 1]
            name = globname[offset:next_offset].replace('\0', '')
            index_by_names[name] = offset

        # TODO
        for i in range(header_ntextures):
            current_texture_name = textures[i]
            current_texture_name_offset = index_by_names.get(
                current_texture_name)
            file.write(struct.pack('<l', current_texture_name_offset))

        for i in range(header_nmaterials):
            material = materials[i]

            material_group_name_idx = index_by_names.get(
                'unknown material group')
            file.write(struct.pack('<l', material_group_name_idx))

            material_name = material.get("name")
            material_name_idx = index_by_names.get(material_name)
            file.write(struct.pack('<l', material_name_idx))

            material_diffuse = 0.8
            file.write(struct.pack('<f', material_diffuse))

            material_specular = 0
            file.write(struct.pack('<f', material_specular))

            material_gloss = 2
            file.write(struct.pack('<f', material_gloss))

            material_selfIllum = 0
            file.write(struct.pack('<f', material_selfIllum))

            material_textures = material.get("textures")

            material_textures_types = [0, 0, 0, 0]

            if len(material_textures) == 1:
                material_textures_types[0] = 1
            if len(material_textures) == 2:
                material_textures_types[0] = 1
                material_textures_types[1] = 2

            for material_textures_type in material_textures_types:
                file.write(struct.pack('<l', material_textures_type))

            material_textures_idxs = [-1, -1, -1, -1]

            for i in range(len(material_textures)):
                material_texture = material_textures[i]
                material_textures_idxs[i] = textures.index(material_texture)

            for material_textures_idx in material_textures_idxs:
                file.write(struct.pack('<l', material_textures_idx))

        # TODO fix for nested locators
        for locator in locators:
            label_name = remove_blender_name_postfix(locator.name)
            label_group_name = remove_blender_name_postfix(locator.parent_bone) if is_animated else remove_blender_name_postfix(
                locator.parent.name)

            label_group_name_idx = index_by_names.get(label_group_name)
            file.write(struct.pack('<l', label_group_name_idx))

            label_name_idx = index_by_names.get(label_name)
            file.write(struct.pack('<l', label_name_idx))

            label_flags = 0
            file.write(struct.pack('<l', label_flags))

            label_m = mathutils.Matrix(locator.matrix_world)
            label_m.translation -= mathutils.Vector(
                locator.parent.matrix_world.translation)

            label_m = correction_export_matrix.to_4x4() @ label_m

            if x_is_mirrored:
                label_m.translation *= mathutils.Vector([-1, 1, 1])

            for i in range(4):
                for j in range(4):
                    file.write(struct.pack('<f', label_m[j][i]))

            label_bones = [
                0,
                0,
                0,
                0
            ]

            if is_animated:
                label_bone = next(
                    filter(lambda x: x.name == locator.parent_bone, bones_list))
                label_bones[0] = bones_list.index(label_bone)

            for i in range(4):
                file.write(struct.pack('<l', label_bones[i]))

            label_weight = [0, 0, 0, 0]
            for i in range(4):
                file.write(struct.pack('<f', label_weight[i]))

        for i in range(header_nobjects):
            object_data = objects_data[i]

            object_group_name = remove_blender_name_postfix(
                objects[i].parent.name)
            object_group_name_idx = index_by_names.get(object_group_name)
            file.write(struct.pack('<l', object_group_name_idx))

            object_name = remove_blender_name_postfix(objects[i].name)
            object_name_idx = index_by_names.get(object_name)
            file.write(struct.pack('<l', object_name_idx))

            # TODO check
            object_flags = 3103
            file.write(struct.pack('<l', object_flags))

            write_vector(file, object_data.get("center"))

            file.write(struct.pack('<f', object_data.get("radius")))

            object_vertex_buff = object_data.get("vertex_buff")
            file.write(struct.pack('<l', object_vertex_buff))

            object_ntriangles = object_data.get("ntriangles")
            file.write(struct.pack('<l', object_ntriangles))

            object_striangle = object_data.get("striangle")
            file.write(struct.pack('<l', object_striangle))

            object_nvertices = object_data.get("nvertices")
            file.write(struct.pack('<l', object_nvertices))

            object_svertex = object_data.get("svertex")
            file.write(struct.pack('<l', object_svertex))

            object_material = object_data.get("material")
            file.write(struct.pack('<l', object_material))

            object_lights = [0, 0, 0, 0, 0, 0, 0, 0]
            for i in range(8):
                file.write(struct.pack('<l', object_lights[i]))

            # TODO check
            object_bones = [0, 0, 0, 0]
            for i in range(4):
                file.write(struct.pack('<l', object_bones[i]))

            object_atriangles = object_data.get("atriangles")
            file.write(struct.pack('<l', object_atriangles))

        for i in range(header_ntriangles):
            for j in range(3):
                file.write(struct.pack('<H', faces[i][j]))

        for vertex_buffer in vertex_buffers:
            vertex_buffer_type = vertex_buffer.get("type")
            file.write(struct.pack('<l', vertex_buffer_type))

            vertex_buffer_stride = 36 + \
                (vertex_buffer_type & 3) * 8 + (vertex_buffer_type >> 2) * 8

            vertex_buffer_size = vertex_buffer.get(
                "vertices_quantity") * vertex_buffer_stride
            file.write(struct.pack('<l', vertex_buffer_size))

        for vertex_buffer in vertex_buffers:
            vertex_type = vertex_buffer.get("type")
            buffer_vertices = vertex_buffer.get("vertices")
            buffer_normals = vertex_buffer.get("normals")
            buffer_colors = vertex_buffer.get("colors")
            buffer_weights = vertex_buffer.get("weights")
            buffer_bone_ids = vertex_buffer.get("bone_ids")
            buffer_uv_array = vertex_buffer.get("uv_array")
            buffer_uv_normals_array = vertex_buffer.get("uv_normals_array")

            for i in range(len(buffer_vertices)):
                if vertex_type == 0:
                    write_vertex0(file, buffer_vertices[i], buffer_normals[i], buffer_colors[i],
                                  buffer_uv_array[i][0], buffer_uv_array[i][1])
                if vertex_type == 1:
                    write_vertex1(file, buffer_vertices[i], buffer_normals[i], buffer_colors[i],
                                  buffer_uv_array[i][0], buffer_uv_array[i][1], buffer_uv_normals_array[i][0], buffer_uv_normals_array[i][1])
                if vertex_type == 4:
                    write_avertex0(file, buffer_vertices[i], buffer_weights[i], buffer_bone_ids[i], buffer_normals[i],
                                   buffer_colors[i], buffer_uv_array[i][0], buffer_uv_array[i][1])

        if bsp:
            sroot = col.build_bsp()

            file.write(struct.pack('<l', col.ssize))
            file.write(struct.pack('<l', col.nvrts))
            file.write(struct.pack('<l', col.ntrgs))

            for root in sroot:
                write_bsp_node(file, root.get("norm"), root.get("pd"), root.get("node"), root.get(
                    "sign"), root.get("left"), root.get("nfaces"), root.get("right"), root.get("type"), root.get("face"))

            for vrt in col.vrt:
                write_vector(file, vrt)

            for trg in col.trg:
                for vert_idx in trg:
                    file.write(struct.pack('<B', (vert_idx >> 0) & 0xFF))
                    file.write(struct.pack('<B', (vert_idx >> 8) & 0xFF))
                    file.write(struct.pack('<B', (vert_idx >> 16) & 0xFF))

            nodes_to_skip = 0
            Build_bsp_node.clear_static_fields()

    print('\nGM Export finished successfully!')

    return {'FINISHED'}


class ExportGm(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export.gm"
    bl_label = "Export GM"

    # ExportHelper mixin class uses this
    filename_ext = ".gm"

    filter_glob: StringProperty(
        default="*.gm",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    BSP: BoolProperty(
        name="BSP (TEST SUPPORT, EXPERIMENTAL FEATURE)",
        default=False,
    )

    def execute(self, context):
        return export_gm(context, self.filepath, self.BSP)


def menu_func_export(self, context):
    self.layout.operator(ExportGm.bl_idname,
                         text="GM Export(.gm)")


def register():
    bpy.utils.register_class(ExportGm)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportGm)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
