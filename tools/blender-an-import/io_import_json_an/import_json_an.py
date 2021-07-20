bl_info = {
    "name": "JSON AN",
    "description": "Import JSON AN files",
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

def import_json_an(context,file_path=""):
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

    collection = bpy.data.collections.new(file_name)
    bpy.context.scene.collection.children.link(collection)

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

    """ armature_obj.rotation_euler[0] = math.radians(90)
    armature_obj.rotation_euler[2] = math.radians(90) """

    bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
    return {'FINISHED'}


class ImportJsonAn(Operator, ImportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "import_an.json"
    bl_label = "Import JSON AN"

    # ImportHelper mixin class uses this
    filename_ext = ".an.json"

    filter_glob: StringProperty(
        default="*.an.json",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return import_json_an(context, self.filepath)


def menu_func_import(self, context):
    self.layout.operator(ImportJsonAn.bl_idname, text="JSON AN Import(.an.json)")


def register():
    bpy.utils.register_class(ImportJsonAn)
    bpy.types.TOPBAR_MT_file_import.append(menu_func_import)


def unregister():
    bpy.utils.unregister_class(ImportJsonAn)
    bpy.types.TOPBAR_MT_file_import.remove(menu_func_import)


if __name__ == "__main__":
    register()
