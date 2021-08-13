import math
import os
import struct
import time
import re

import bmesh
import bpy
import mathutils
from bpy.props import BoolProperty, EnumProperty, StringProperty
from bpy.types import Operator
from bpy_extras.io_utils import ImportHelper, axis_conversion

correction_export_matrix = axis_conversion(
    from_forward='Y', from_up='Z', to_forward='X', to_up='Y')

def get_bounding_box_coords(obj):
    bound_box = obj.bound_box
    x1 = 0
    x2 = 0
    y1 = 0
    y2 = 0
    z1 = 0
    z2 = 0
    
    for [x,y,z] in bound_box:
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
    box_radius = 0;
    
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
    return re.sub(r'.\d{3}', '', name)


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


def export_gm(file_path=""):
    bpy.ops.object.mode_set(mode='OBJECT', toggle=False)

    root = bpy.context.view_layer.objects.active
    root_children = root.children

    collection = root.users_collection[0]

    is_animated = False

    objects = []
    locators = []
    bones_list = []

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
                    objects = [child]

            break

    x_is_mirrored = not is_animated

    bpy.context.scene.frame_set(0)

    # TODO multiple objects
    src_obj = objects[0]

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

    print(len(bm.verts), len(bm.faces))

    prepare_vertices_with_multiple_uvs(bm.verts, obj_uv_layer)

    if obj_uv_normals_layer:
        prepare_vertices_with_multiple_uvs(bm.verts, obj_uv_normals_layer)

    bm.to_mesh(obj.data)
    print(len(bm.verts), len(bm.faces))

    obj_vertices = obj_data.vertices
    obj_polygons = obj_data.polygons

    # TODO get active?
    obj_vertex_color = obj_data.vertex_colors[0] if len(
        obj_data.vertex_colors) > 0 else None

    obj_vertex_groups = obj.vertex_groups

    obj_uv_layer = obj_data.uv_layers[0]
    obj_uv_normals_layer = obj_data.uv_layers[1] if len(
        obj_data.uv_layers) > 1 else None

    materials = []
    # TODO
    material = get_material_data(obj_data)
    # TODO
    materials = [material]

    # TODO check
    textures = []
    for material in materials:
        material_textures = material.get("textures")
        for material_texture in material_textures:
            if not (material_texture in textures):
                textures.append(material_texture)

    vertices = []
    normals = []

    weights = []
    bone_ids = []

    for vertex in bm.verts:
        pos = mathutils.Vector(vertex.co)
        pos.rotate(correction_export_matrix)
        if x_is_mirrored:
            pos *= mathutils.Vector([-1, 1, 1])
        vertices.append(pos)

        norm = mathutils.Vector(vertex.normal)
        norm.rotate(correction_export_matrix)
        if x_is_mirrored:
            norm *= mathutils.Vector([-1, 1, 1])
        normals.append(norm)

    faces = []
    for face in bm.faces:
        [v1, v2, v3] = face.verts[:]
        # opposite
        face = [v2.index, v1.index, v3.index]
        faces.append(face)

    obj.to_mesh_clear()
    bm.free()

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

            weights.append(weight_1)
            bone_ids.append((bone_2 << 8) | (bone_1 << 0))

    colors = [[127, 127, 127, 255]] * len(obj_vertices)
    uv_array = [[0, 0]] * len(obj_vertices)

    # TODO idx
    has_uv_normals = len(materials[0].get(
        "textures")) == 2 and obj_uv_normals_layer

    uv_normals_array = [[0, 0]] * len(obj_vertices) if has_uv_normals else None

    for polygon in obj_polygons:
        for i, index in enumerate(polygon.vertices):
            loop_index = polygon.loop_indices[i]
            if obj_vertex_color:
                [r, g, b, a] = obj_vertex_color.data[loop_index].color[:]
                colors[index] = [int(r*255), int(g*255),
                                 int(b*255), int(a*255)]
            uv_array[index] = mathutils.Vector(
                obj_uv_layer.data[loop_index].uv) * mathutils.Vector([1, -1])
            if has_uv_normals:
                uv_normals_array[index] = mathutils.Vector(
                    obj_uv_normals_layer.data[loop_index].uv) * mathutils.Vector([1, -1])

    types = []

    for i in range(len(objects)):
        if is_animated:
            types.append(4)
            continue
        if has_uv_normals:
            types.append(1)
            continue
        types.append(0)

    bounding_box = get_bounding_box_coords(obj)

    # TODO delete on error
    bpy.data.objects.remove(obj, do_unlink=True)

    # TODO nested
    with open(file_path, 'wb') as file:
        header_version = 825110581
        file.write(struct.pack('<l', header_version))

        # TODO BSP
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

        # TODO
        header_nobjects = 1
        file.write(struct.pack('<l', header_nobjects))

        header_ntriangles = len(faces)
        file.write(struct.pack('<l', header_ntriangles))

        # TODO
        header_nvrtbuffs = 1
        file.write(struct.pack('<l', header_nvrtbuffs))

        # TODO
        header_bboxSize = get_box_size(bounding_box)
        write_vector(file, header_bboxSize)

        # TODO
        header_bboxCenter = get_box_center(bounding_box)
        write_vector(file, header_bboxCenter)

        # TODO
        header_radius = get_box_radius(header_bboxCenter, vertices)
        file.write(struct.pack('<f', header_radius))

        for c in globname:
            file.write(struct.pack('<s', c.encode("utf-8")))

        current_name_offset = 0
        for i in range(header_names_quantity):
            file.write(struct.pack('<l', current_name_offset))
            current_name_offset = globname.index('\0', current_name_offset) + 1

        # TODO
        for i in range(header_ntextures):
            current_texture_name = textures[i]
            current_texture_name_offset = globname.index(
                current_texture_name + '\0')
            file.write(struct.pack('<l', current_texture_name_offset))

        for i in range(header_nmaterials):
            material = materials[i]

            material_group_name_idx = globname.index(
                'unknown material group' + '\0')
            file.write(struct.pack('<l', material_group_name_idx))

            material_name = material.get("name")
            material_name_idx = globname.index(material_name + '\0')
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

            label_group_name_idx = globname.index(label_group_name + '\0')
            file.write(struct.pack('<l', label_group_name_idx))

            label_name_idx = globname.index(label_name + '\0')
            file.write(struct.pack('<l', label_name_idx))

            label_flags = 0
            file.write(struct.pack('<l', label_flags))

            label_m = correction_export_matrix.to_4x4() @ locator.matrix_basis

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
            object_group_name = remove_blender_name_postfix(
                objects[i].parent.name)
            object_group_name_idx = globname.index(object_group_name + '\0')
            file.write(struct.pack('<l', object_group_name_idx))

            object_name = remove_blender_name_postfix(objects[i].name)
            object_name_idx = globname.index(object_name + '\0')
            file.write(struct.pack('<l', object_name_idx))

            # TODO check
            object_flags = 3103
            file.write(struct.pack('<l', object_flags))

            # TODO
            object_center = get_box_center(bounding_box)
            write_vector(file, object_center)

            # TODO
            object_radius = get_box_radius(object_center, vertices)
            file.write(struct.pack('<f', object_radius))
            

            # TODO
            object_vertex_buff = 0
            file.write(struct.pack('<l', object_vertex_buff))

            # TODO
            object_ntriangles = len(faces)
            file.write(struct.pack('<l', object_ntriangles))

            # TODO
            object_striangle = 0
            file.write(struct.pack('<l', object_striangle))

            # TODO
            object_nvertices = len(vertices)
            file.write(struct.pack('<l', object_nvertices))

            # TODO
            object_svertex = 0
            file.write(struct.pack('<l', object_svertex))

            # TODO
            object_material = 0
            file.write(struct.pack('<l', object_material))

            object_lights = [0, 0, 0, 0, 0, 0, 0, 0]
            for i in range(8):
                file.write(struct.pack('<l', object_lights[i]))

            # TODO check
            object_bones = [0, 0, 0, 0]
            for i in range(4):
                file.write(struct.pack('<l', object_bones[i]))

            # TODO
            object_atriangles = 0
            file.write(struct.pack('<l', object_atriangles))

        for i in range(header_ntriangles):
            for j in range(3):
                file.write(struct.pack('<H', faces[i][j]))

        for i in range(header_nvrtbuffs):
            # TODO
            vertex_buffer_type = types[0]
            file.write(struct.pack('<l', vertex_buffer_type))

            vertex_buffer_stride = 36 + \
                (vertex_buffer_type & 3) * 8 + (vertex_buffer_type >> 2) * 8

            vertex_buffer_size = len(vertices) * vertex_buffer_stride
            file.write(struct.pack('<l', vertex_buffer_size))

        print(len(vertices), len(weights), len(bone_ids),
              len(normals), len(colors), len(uv_array))

        for i in range(len(vertices)):
            # TODO
            vertex_type = types[0]
            if vertex_type == 0:
                write_vertex0(file, vertices[i], normals[i], colors[i],
                              uv_array[i][0], uv_array[i][1])
            if vertex_type == 1:
                write_vertex1(file, vertices[i], normals[i], colors[i],
                              uv_array[i][0], uv_array[i][1], uv_normals_array[i][0], uv_normals_array[i][1])
            if vertex_type == 4:
                write_avertex0(file, vertices[i], weights[i], bone_ids[i], normals[i],
                               colors[i], uv_array[i][0], uv_array[i][1])


export_gm(file_path='/Users/Artess/WebstormProjects/storm-engine/tools/blender-gm-import/PGG_Port_0.gm')
