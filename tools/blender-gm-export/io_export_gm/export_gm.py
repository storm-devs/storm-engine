from math import sqrt, ceil
import struct
import time
import re
import sys
import cProfile

import bmesh
import bpy
from mathutils import Vector, Matrix
from collections import defaultdict
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

correction_export_matrix = axis_conversion(
    from_forward='Y', from_up='Z', to_forward='X', to_up='Y')


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
    center_vector = Vector(box_center)
    box_radius = 0

    for pos in vertices:
        pos_vector = Vector(pos)
        [x, y, z] = center_vector - pos_vector
        radius = sqrt(x * x + y * y + z * z)
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


def export_gm(context, file_path="", triangulate=False):
    # pr = cProfile.Profile()
    # pr.enable()

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

        if triangulate:
            bmesh.ops.triangulate(bm, faces=bm.faces[:])

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
            pos = (obj.matrix_world @ Vector(vertex.co)) - \
                Vector(obj.parent.matrix_world.translation)
            pos.rotate(correction_export_matrix)
            if x_is_mirrored:
                pos *= Vector([-1, 1, 1])
            vertices.append(pos)
            obj_vertices_coords.append(pos)

            norm = Vector(vertex.normal)
            norm.rotate(correction_export_matrix)
            if x_is_mirrored:
                norm *= Vector([-1, 1, 1])
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
                obj_uv_array[index] = Vector(
                    obj_uv_layer.data[loop_index].uv) * Vector([1, -1])
                if has_uv_normals:
                    obj_uv_normals_array[index] = Vector(
                        obj_uv_normals_layer.data[loop_index].uv) * Vector([1, -1])

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

        header_flags = 0
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

            label_m = Matrix(locator.matrix_world)
            label_m.translation -= Vector(
                locator.parent.matrix_world.translation)

            label_m = correction_export_matrix.to_4x4() @ label_m

            if x_is_mirrored:
                label_m.translation *= Vector([-1, 1, 1])

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

    # pr.disable()
    # pr.print_stats(2)

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

    triangulate: BoolProperty(
        name="triangulate",
        default=True,
    )

    def execute(self, context):
        return export_gm(context, self.filepath, self.triangulate)


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
