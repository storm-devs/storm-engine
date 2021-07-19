bl_info = {
    "name": "JSON GM",
    "description": "Import JSON GM files",
    "author": "Artess999",
    "version": (0, 0, 1),
    "blender": (2, 92, 0),
    "location": "File > Import",
    "warning": "",
    "support": "COMMUNITY",
    "category": "Import",
}


import bpy, bmesh, mathutils
import time, struct, os
import math
import json

from bpy_extras.io_utils import ImportHelper, axis_conversion
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

correction_matrix = axis_conversion(from_forward='X', from_up='Y', to_forward='Y', to_up='Z')

def get_armature_obj(file_path, collection):
    file_name = os.path.basename(file_path)[:-8]
    f = open(file_path,)
    data = json.load(f)

    header = data['header']
    frames_quantity = header['nFrames']
    joints_quantity = header['nJoints']
    fps = header['framesPerSec']

    parent_indices = data['parentIndices']
    start_joints_positions = data['startJointsPositions']
    blender_start_joints_positions = data['blenderStartJointsPositions']
    root_bone_positions = data['rootBonePositions']
    joints_angles = data['jointsAngles']

    bpy.context.scene.frame_set(0)
    bpy.context.scene.render.fps = fps
    bpy.context.scene.frame_start = 0
    bpy.context.scene.frame_end = frames_quantity - 1

    armature = bpy.data.armatures.new('armature')
    armature_obj = bpy.data.objects.new('armature_obj', armature)
    collection.objects.link(armature_obj)

    animation_data = armature_obj.animation_data_create()
    actions = bpy.data.actions.new(name="Joints_action")
    animation_data.action = actions

    armature_obj.data.display_type = 'STICK'

    bpy.context.view_layer.objects.active = armature_obj
    bpy.ops.object.mode_set(mode='EDIT', toggle=False)

    armature_edit_bones = armature_obj.data.edit_bones

    bones_arr = []

    for idx in range(joints_quantity):
        bone = armature_edit_bones.new("Bone" + str(idx))

        if idx != 0:
            bone.parent = bones_arr[parent_indices[idx]]

        pos = mathutils.Vector(start_joints_positions[idx])
        prepared_pos = mathutils.Vector(blender_start_joints_positions[idx])
        parent_pos = mathutils.Vector(blender_start_joints_positions[parent_indices[idx]])

        if idx in parent_indices:
            child_idx = parent_indices.index(idx)
            child_pos = mathutils.Vector(blender_start_joints_positions[child_idx])
        else:
            child_pos = mathutils.Vector(prepared_pos) + mathutils.Vector([0,0.00001,0])

        bone.head = (prepared_pos[0], prepared_pos[1] - 0.00001, prepared_pos[2])
        bone.tail = (prepared_pos[0], prepared_pos[1] + 0.00001, prepared_pos[2])

        bone.matrix = correction_matrix.to_4x4() @ bone.matrix

        bones_arr.append(bone)

    bpy.ops.object.mode_set(mode='POSE', toggle=False)

    for bone_idx in range(joints_quantity):
        bone_name = "Bone" + str(bone_idx)

        if bone_idx == 0:
            for idx in range(3):
                fc = actions.fcurves.new('pose.bones["' + bone_name + '"].location', index=idx)
                fc.keyframe_points.add(count=frames_quantity)

                key_values = []
                for frame in range(frames_quantity):
                    key_values.append(frame)
                    key_values.append(root_bone_positions[frame][idx])
                fc.keyframe_points.foreach_set("co", key_values)

                fc.update()

        for idx in range(4):
            fc = actions.fcurves.new('pose.bones["' + bone_name + '"].rotation_quaternion', index=idx)
            fc.keyframe_points.add(count=frames_quantity)

            key_values = []
            for frame in range(frames_quantity):
                key_values.append(frame)
                key_values.append(joints_angles[bone_idx][frame][idx])
            fc.keyframe_points.foreach_set("co", key_values)

            fc.update()

    return armature_obj


def import_json_gm(context,file_path="",an_path=""):
    file_name = os.path.basename(file_path)[:-8]
    textures_path = os.path.join(os.path.dirname(file_path),'textures')
    f = open(file_path,)
    data = json.load(f)

    xIsMirrored = data.get('xIsMirrored')

    is_animated = data.get('isAnimated') or False
    has_animation = is_animated and an_path != ""

    collection = bpy.data.collections.new(file_name)
    bpy.context.scene.collection.children.link(collection)

    root = bpy.data.objects.new( "root", None )
    collection.objects.link(root)

    if has_animation:
        armature_obj = get_armature_obj(an_path, collection)
        armature_obj.parent = root

    for object in data['objects']:
        name = object.get('name')

        vertices = object.get('verticies')
        normals = object.get('normals')
        uv_array = object.get('uv')
        uv_normals_array = object.get('uvNormals')
        faces = object.get('faces')
        colors = object.get('colors')

        weights = object.get('weights')
        bone_ids = object.get('boneIds')

        material = object.get('material')

        me = bpy.data.meshes.new(name)
        ob = bpy.data.objects.new(name, me)
        ob.parent = root

        bm = bmesh.new()
        bm.from_mesh(me)
        uv_layer = bm.loops.layers.uv.new()
        if uv_normals_array is not None:
            uv_normals_layer = bm.loops.layers.uv.new('UVMap_normals')

        collection.objects.link(ob)
        bpy.context.view_layer.objects.active = ob
        bpy.ops.object.mode_set(mode='EDIT', toggle=False)

        # setup our material
        texture_file = None if len(material['textureNames']) == 0 else material['textureNames'][0]
        texture_normals_file = None if len(material['textureNames']) == 1 else material['textureNames'][1]
        material_name = None if texture_file is None else (collection.name + '_' + material['name'])

        if texture_file is not None:
            texture_path = os.path.join(textures_path, texture_file)

        if texture_normals_file is not None:
            texture_normals_path = os.path.join(textures_path, texture_normals_file)

        #
        mtl = None
        if material_name is not None and material_name in bpy.data.materials:
            mtl = bpy.data.materials[material_name]
        elif texture_file is not None:
            mtl = bpy.data.materials.new(name=material_name)

            mtl.use_nodes = True
            mtl.blend_method = 'CLIP'

            bsdf = mtl.node_tree.nodes["Principled BSDF"]
            bsdf.inputs['Specular'].default_value = 0.0

            # load texture onto the material
            if texture_file in bpy.data.textures:
                mtex = mtl.texture_slots.add()
                mtex.texture = bpy.data.textures[texture_file]
            elif os.path.isfile(texture_path):
                tex = mtl.node_tree.nodes.new('ShaderNodeTexImage')

                if texture_file in bpy.data.images:
                    tex.image = bpy.data.images[texture_file]
                else:
                    tex.image = bpy.data.images.load(texture_path)

                mtl.node_tree.links.new(bsdf.inputs['Alpha'], tex.outputs['Alpha'])

                if texture_normals_file is not None:
                    normalTex = mtl.node_tree.nodes.new('ShaderNodeTexImage')

                    if texture_normals_file in bpy.data.images:
                        normalTex.image = bpy.data.images[texture_normals_file]
                    elif os.path.isfile(texture_normals_path):
                        normalTex.image = bpy.data.images.load(texture_normals_path)

                    normalUVMap = mtl.node_tree.nodes.new('ShaderNodeUVMap')
                    normalUVMap.uv_map = "UVMap_normals"

                    mixer = mtl.node_tree.nodes.new('ShaderNodeMixRGB')
                    mixer.blend_type = 'MULTIPLY'
                    mixer.inputs['Fac'].default_value = 1.0

                    gray_divider = mtl.node_tree.nodes.new('ShaderNodeMixRGB')
                    gray_divider.blend_type = 'DIVIDE'
                    gray_divider.inputs['Fac'].default_value = 1.0
                    gray_divider.inputs['Color2'].default_value = (0.2, 0.2, 0.2, 1)

                    mtl.node_tree.links.new(mixer.inputs['Color1'], tex.outputs['Color'])

                    mtl.node_tree.links.new(gray_divider.inputs['Color1'], normalTex.outputs['Color'])
                    mtl.node_tree.links.new(mixer.inputs['Color2'], gray_divider.outputs['Color'])
                    mtl.node_tree.links.new(normalTex.inputs['Vector'], normalUVMap.outputs['UV'])

                    mtl.node_tree.links.new(bsdf.inputs['Base Color'], mixer.outputs['Color'])
                else:
                    mtl.node_tree.links.new(bsdf.inputs['Base Color'], tex.outputs['Color'])


        if mtl is not None:
            ob.data.materials.append(mtl)

        for x in range(len(vertices)):
          vtx = bm.verts.new(vertices[x])
          vtx.normal = mathutils.Vector(normals[x])

        bm.verts.ensure_lookup_table()
        for f in faces:
              try:
                face = bm.faces.new((bm.verts[f[0]], bm.verts[f[1]], bm.verts[f[2]]))
                face.smooth = True
                face.material_index = 0

                for uv_set_loop in range(3):
                    face.loops[uv_set_loop][uv_layer].uv = uv_array[f[uv_set_loop]]

                    if uv_normals_array is not None:
                        face.loops[uv_set_loop][uv_normals_layer].uv = uv_normals_array[f[uv_set_loop]]

              except Exception as e:
                print(str(e))

        bmesh.ops.rotate(bm, verts=bm.verts, cent=(0.0, 0.0, 0.0), matrix=correction_matrix)

        """ TODO backface Culling """

        bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
        bm.to_mesh(me)
        bm.free()

        if is_animated:
            for x in range(len(vertices)):
                weight = weights[x]
                first_bone_idx = bone_ids[x][0]
                second_bone_idx = bone_ids[x][1]
                first_bone_name = "Bone" + str(first_bone_idx)
                second_bone_name = "Bone" + str(second_bone_idx)

                if ob.vertex_groups.get(first_bone_name) is None:
                    ob.vertex_groups.new(name=first_bone_name)
                if ob.vertex_groups.get(second_bone_name) is None:
                    ob.vertex_groups.new(name=second_bone_name)

                ob.vertex_groups[first_bone_name].add([x], weight, 'REPLACE')
                ob.vertex_groups[second_bone_name].add([x], 1.0 - weight, 'REPLACE')

        col = me.vertex_colors.new()

        for polygon in me.polygons:
            for i, index in enumerate(polygon.vertices):
                loop_index = polygon.loop_indices[i]
                col.data[loop_index].color = colors[index]

        if has_animation:
            ob.parent = armature_obj
            modifier = ob.modifiers.new(type='ARMATURE', name="Armature")
            modifier.object = armature_obj
            """ bpy.context.view_layer.objects.active = armature_obj
            bpy.ops.pose.armature_apply(selected=False)
            bpy.context.view_layer.objects.active = ob """

        """ hack, texture is too dark without it """
        bpy.ops.object.mode_set(mode='EDIT', toggle=False)
        bpy.ops.object.mode_set(mode='OBJECT', toggle=False)

    for locators_tree in data['locatorsTrees']:
        group_locator_name = locators_tree
        group_locator = bpy.data.objects.new( group_locator_name, None )
        collection.objects.link(group_locator)
        group_locator.parent = root

        for locator_data in data['locatorsTrees'][locators_tree]:
            locator_name = locator_data.get('name')
            locator_m = locator_data.get('m')
            locator_bone_idx = locator_data.get('boneIdx')
            locator = bpy.data.objects.new( locator_name, None )
            collection.objects.link(locator)
            locator.parent = group_locator
            locator.matrix_basis = locator_m
            locator.matrix_basis = correction_matrix.to_4x4() @ locator.matrix_basis
            locator.empty_display_size = 0.5
            if has_animation and locator_bone_idx > 0:
                locator.parent = armature_obj
                bone = armature_obj.pose.bones["Bone" + str(locator_bone_idx)]
                locator.parent_bone = bone.name
                locator.parent_type = 'BONE'
                locator.matrix_parent_inverse = bone.matrix.inverted()
            else:
                locator.parent = group_locator

            if xIsMirrored:
                locator.location[0] = -locator.location[0]

    """ root.rotation_euler[0] = math.radians(90)
    root.rotation_euler[2] = math.radians(90) """
    return {'FINISHED'}


class ImportJsonGm(Operator, ImportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "import_gm.json"
    bl_label = "Import JSON GM"

    # ImportHelper mixin class uses this
    filename_ext = ".gm.json"

    filter_glob: StringProperty(
        default="*.gm.json",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    an_name: StringProperty(
        name="Animation name",
        description="Must be in the same folder as model",
        default="man.an.json",
    )

    def execute(self, context):
        an_path = os.path.join(os.path.dirname(self.filepath), self.an_name)
        if os.path.isfile(an_path):
            return import_json_gm(context, self.filepath, an_path)

        return import_json_gm(context, self.filepath)


def menu_func_import(self, context):
    self.layout.operator(ImportJsonGm.bl_idname, text="JSON GM Import(.gm.json)")


def register():
    bpy.utils.register_class(ImportJsonGm)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportJsonGm)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
    register()
