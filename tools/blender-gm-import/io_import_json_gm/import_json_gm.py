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

fixed_coas_man_head_pos = {
  '16': [ 0.0032584953587507497, 1.6475999020040035, -0.011722753755748272 ],
  '21': [ 0.0032584953587506994, 1.734414953738451, -0.017260584514588118 ],
  '26': [ 0.0032584953587502237, 1.7189353285357356, 0.0032311086542904377 ],
  '27': [ 0.03225849452428567, 1.7078742664307356, 0.06422260822728276 ],
  '28': [ -0.025741503806784274, 1.7078742664307356, 0.06422260822728276 ],
  '29': [ 0.0032584953587502237, 1.714992631226778, 0.06417078943923116 ],
  '30': [ 0.0032584953587502237, 1.71595567651093, 0.06417078943923116 ],
  '31': [ 0.0032584953587502237, 1.670038390904665, 0.017052501905709505 ],
  '32': [ 0.0032584953587502237, 1.666005652397871, 0.0289492798037827 ],
  '33': [ 0.04005750524811494, 1.6590940840542316, 0.043188605923205614 ],
  '34': [ 0.010431578150019525, 1.7059485670179129, 0.09411628963425756 ],
  '35': [ -0.03376904618926299, 1.6590893976390362, 0.043188624549657106 ],
  '36': [ -0.00436244835145724, 1.705950003117323, 0.09411628963425756 ],
  '39': [ 0.014712675241753459, 1.7155753958504647, 0.017472744453698397 ],
  '40': [ -0.00819568452425301, 1.7155732766259462, 0.017472758423537016 ],
  '41': [ 0.003258495358750247, 1.6719406803604215, 0.07180523918941617 ],
  '42': [ 0.014147192006930709, 1.6719406803604215, 0.07180523918941617 ],
  '43': [ -0.007630198495462537, 1.6719442555913702, 0.07180526526644826 ],
  '44': [ 0.02245222474448383, 1.6453957352787256, 0.056133848149329424 ],
  '45': [ -0.014557642163708806, 1.645408296957612, 0.05640476243570447 ],
  '46': [ 0.0032584953587502558, 1.6316347122192383, 0.06349346833303571 ],
  '47': [ 0.01994910533539951, 1.6316347122192383, 0.06349346833303571 ],
  '48': [ -0.013432058738544583, 1.6316353976726532, 0.06349343480542302 ],
  '49': [ 0.040822318522260065, 1.6185563057661057, 0.04033719236031175 ],
  '50': [ 0.05190535425208556, 1.7011534315533936, 0.08644563751295209 ],
  '51': [ -0.03362681638100137, 1.6185519061982632, 0.04056849470362067 ],
  '52': [ -0.045069209532811766, 1.70209743315354, 0.0862667472101748 ],
  '55': [ 0.01744386600330472, 1.7159136612899601, 0.10848532663658261 ],
  '56': [ -0.010926909511908889, 1.7159103897283785, 0.10848526610061526 ],
  '57': [ 0.003258495358750256, 1.6672005250584334, 0.10340505512431264 ],
  '58': [ 0.02708967193029821, 1.663889413466677, 0.10081649431958795 ],
  '59': [ -0.020572699839249253, 1.6638930957997218, 0.10081647569313645 ],
  '60': [ 0.02944240369834006, 1.6386494464240968, 0.08547264384105802 ],
  '61': [ -0.02311395318247378, 1.6386494473554194, 0.08547264384105802 ],
  '62': [ 0.003258495358750243, 1.624780164565891, 0.09175814734771848 ],
  '63': [ 0.020785945875104517, 1.6249020621180534, 0.0917750527150929 ],
  '64': [ -0.014268974424339831, 1.624906247947365, 0.09177503967657685 ],
  '67': [ 0.048700828570872545, 1.7250176309607923, 0.09955776995047927 ],
  '68': [ -0.042183868354186416, 1.725021539896261, 0.09955776808783412 ],
  '69': [ 0.0032584953587502276, 1.6471846054773778, 0.10507304838392884 ],
  '70': [ 0.018761535873636603, 1.6457710040267557, 0.10322161880321801 ],
  '71': [ -0.012244513491168618, 1.6457660753512755, 0.10322158108465374 ],
  '72': [ 0.003258495358750234, 1.6364505612291396, 0.10192738147452474 ],
  '73': [ 0.01705583737930283, 1.6360291168093681, 0.10186893353238702 ],
  '74': [ -0.010538881528191268, 1.6360301873646677, 0.10186888324096799 ]
}

coas_to_potc_man = {
    '0': '0',
    '1': '1',
    '2': '2',
    '3': '3',
    '4': '4',
    '5': '5',
    '6': '6',
    '7': '7',
    '8': '8',
    '9': '9',
    '10': '10',
    '11': '11',
    '12': '12',
    '13': '13',
    '14': '14',
    '15': '15',
    '16': '16',
    '17': '17',
    '18': '18',
    '19': '19',
    '20': '20',
    '22': '21',
    '23': '22',
    '24': '23',
    '25': '24',
    '37': '25',
    '38': '26',
    '53': '27',
    '54': '28',
    '65': '29',
    '66': '30',
    '75': '31',
    '76': '32',
    '77': '33',
    '78': '34',
    '79': '35',
    '80': '36',
    '81': '37',
    '82': '38',
    '83': '39',
    '84': '40',
    '85': '41',
    '86': '42',
    '87': '43',
    '88': '44',
    '89': '45',
    '90': '46',
    '91': '47',
    '92': '48',
    '93': '49',
    '94': '50',
    '95': '51',
    '96': '52',
    '97': '53',
    '98': '54',
    '99': '55',
    '100': '56',
    '101': '57',
    '102': '58',
    '103': '59',
    '104': '60',
    '105': '61',
    '106': '62',
    '107': '63',
    '108': '64',
    '109': '65',
    '110': '66',
    '111': '67',
    '112': '68',
    '113': '69',
    '114': '70',
    '115': '71',
    '116': '72',
    '117': '73',
    '118': '74',
    '119': '75',
    '120': '76',
    '121': '77',
    '122': '78'
}

potc_to_coas_man = { value:key for key, value in coas_to_potc_man.items() }

# taken from Copy Attributes Menu Addon by Bassam Kurdali, Fabian Fricke, Adam Wiseman
def getmat(bone, active, context, ignoreparent):
    obj_bone = bone.id_data
    obj_active = active.id_data
    data_bone = obj_bone.data.bones[bone.name]

    active_to_selected = obj_bone.matrix_world.inverted() @ obj_active.matrix_world
    active_matrix = active_to_selected @ active.matrix
    otherloc = active_matrix
    bonemat_local = data_bone.matrix_local.copy()
    if data_bone.parent:
        parentposemat = obj_bone.pose.bones[data_bone.parent.name].matrix.copy()
        parentbonemat = data_bone.parent.matrix_local.copy()
    else:
        parentposemat = parentbonemat = mathutils.Matrix()
    if parentbonemat == parentposemat or ignoreparent:
        newmat = bonemat_local.inverted() @ otherloc
    else:
        bonemat = parentbonemat.inverted() @ bonemat_local

        newmat = bonemat.inverted() @ parentposemat.inverted() @ otherloc
    return newmat


def get_armature_obj(file_path, collection, type='', fix_coas_man_head=False):
    file_name = os.path.basename(file_path)[:-8]
    f = open(file_path,)
    data = json.load(f)

    header = data.get('header')
    frames_quantity = header.get('nFrames')
    joints_quantity = header.get('nJoints')
    fps = header.get('framesPerSec')

    parent_indices = data.get('parentIndices')
    start_joints_positions = data.get('startJointsPositions')
    blender_start_joints_positions = data.get('blenderStartJointsPositions')
    root_bone_positions = data.get('rootBonePositions')
    joints_angles = data.get('jointsAngles')

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

        if fix_coas_man_head and str(idx) in fixed_coas_man_head_pos:
            prepared_pos = mathutils.Vector(fixed_coas_man_head_pos[str(idx)])

        bone.head = (prepared_pos[0], prepared_pos[1] - 0.00001, prepared_pos[2])
        bone.tail = (prepared_pos[0], prepared_pos[1] + 0.00001, prepared_pos[2])

        bone.matrix = correction_matrix.to_4x4() @ bone.matrix

        bones_arr.append(bone)

    bpy.ops.object.mode_set(mode='POSE', toggle=False)

    if type == 'POSE_SOURCE':
        return armature_obj

    if type == 'POSE':
        for bone_idx in range(joints_quantity):
            bone = armature_obj.pose.bones["Bone" + str(bone_idx)]

            if bone_idx == 0:
                bone.location = root_bone_positions[0]
            bone.rotation_quaternion = joints_angles[bone_idx][0]
        return armature_obj

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


def import_json_gm(
        context,
        file_path="",
        an_path="",
        fix_coas_man_head=False,
        convert_coas_to_potc_man=False,
        convert_potc_to_coas_man=False,
    ):
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
        armature_obj = get_armature_obj(an_path, collection,fix_coas_man_head=fix_coas_man_head)
        armature_obj.parent = root

        armature_obj_pose = get_armature_obj(an_path, collection, 'POSE',fix_coas_man_head=fix_coas_man_head)
        armature_obj_pose_source = get_armature_obj(an_path, collection, 'POSE_SOURCE',fix_coas_man_head=fix_coas_man_head)

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

                if convert_coas_to_potc_man:
                    converted_first_bone_idx = coas_to_potc_man.get(str(bone_ids[x][0]))
                    converted_second_bone_idx = coas_to_potc_man.get(str(bone_ids[x][1]))

                    if converted_first_bone_idx is None:
                        converted_first_bone_idx = 16
                    if converted_second_bone_idx is None:
                        converted_second_bone_idx = 16

                    first_bone_idx = converted_first_bone_idx
                    second_bone_idx = converted_second_bone_idx

                if convert_potc_to_coas_man:
                    first_bone_idx = potc_to_coas_man.get(str(bone_ids[x][0]))
                    second_bone_idx = potc_to_coas_man.get(str(bone_ids[x][1]))

                first_bone_name = "Bone" + str(first_bone_idx)
                second_bone_name = "Bone" + str(second_bone_idx)

                if ob.vertex_groups.get(first_bone_name) is None:
                    ob.vertex_groups.new(name=first_bone_name)
                if ob.vertex_groups.get(second_bone_name) is None:
                    ob.vertex_groups.new(name=second_bone_name)

                ob.vertex_groups[first_bone_name].add([x], weight, 'ADD')
                ob.vertex_groups[second_bone_name].add([x], 1.0 - weight, 'ADD')

        col = me.vertex_colors.new()

        for polygon in me.polygons:
            for i, index in enumerate(polygon.vertices):
                loop_index = polygon.loop_indices[i]
                col.data[loop_index].color = colors[index]

        if has_animation:
            ob.parent = armature_obj_pose
            modifier = ob.modifiers.new(type='ARMATURE', name="Armature")
            modifier.object = armature_obj_pose
            bpy.context.view_layer.objects.active = armature_obj_pose

            bpy.ops.pose.armature_apply(selected=False)

            bpy.ops.object.mode_set(mode='POSE', toggle=False)

            for pbone in armature_obj.data.bones:
                bone_name = pbone.name

                bone = armature_obj_pose.pose.bones[bone_name]
                active = armature_obj_pose_source.pose.bones[bone_name]

                bone.location = getmat(bone, active, context, False).to_translation()
                bone.rotation_quaternion = getmat(bone, active, context, not bone.id_data.data.bones[bone.name].use_inherit_rotation).to_3x3().to_quaternion()

                """ hack """
                bpy.ops.object.mode_set(mode='EDIT', toggle=False)
                bpy.ops.object.mode_set(mode='OBJECT', toggle=False)

            bpy.context.view_layer.objects.active = ob
            bpy.ops.object.modifier_apply(modifier="Armature")

            bpy.data.objects.remove(armature_obj_pose_source, do_unlink=True)
            bpy.data.objects.remove(armature_obj_pose, do_unlink=True)

            ob.parent = armature_obj
            modifier = ob.modifiers.new(type='ARMATURE', name="Armature")
            modifier.object = armature_obj
            modifier.use_deform_preserve_volume = True

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

                if convert_potc_to_coas_man:
                    bone = armature_obj.pose.bones["Bone" + potc_to_coas_man.get(str(locator_bone_idx))]

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

    fix_coas_man_head: BoolProperty(
        name="Fix CoAS man skeleton head",
        default=False,
    )

    convert_coas_to_potc_man: BoolProperty(
        name="Convert CoAS man skeleton to PotC",
        default=False,
    )

    convert_potc_to_coas_man: BoolProperty(
        name="Convert PotC man skeleton to CoAS",
        default=False,
    )

    def execute(self, context):
        an_path = os.path.join(os.path.dirname(self.filepath), self.an_name)
        if os.path.isfile(an_path):
            return import_json_gm(
                context,
                self.filepath,
                an_path=an_path,
                fix_coas_man_head=self.fix_coas_man_head,
                convert_coas_to_potc_man=self.convert_coas_to_potc_man,
                convert_potc_to_coas_man=self.convert_potc_to_coas_man
            )

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
