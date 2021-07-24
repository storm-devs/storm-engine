bl_info = {
    "name": "JSON AN",
    "description": "Export JSON AN files",
    "author": "Artess999",
    "version": (0, 0, 1),
    "blender": (2, 92, 0),
    "location": "File > Export",
    "warning": "",
    "support": "COMMUNITY",
    "category": "Export",
}


import bpy, bmesh, mathutils
import time, struct, os
import math
import json

from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator
from bpy_extras.io_utils import axis_conversion


correction_export_matrix = axis_conversion(from_forward='Y', from_up='Z', to_forward='X', to_up='Y')

def export_json_an(context,file_path=""):
    armature_obj = bpy.context.view_layer.objects.active
    bones = armature_obj.data.bones
    pose_bones = armature_obj.pose.bones
    action = armature_obj.animation_data.action
    fcurves = action.fcurves

    header = {
        "nFrames": bpy.context.scene.frame_end + 1,
        "nJoints": len(bones),
        "framesPerSec": bpy.context.scene.render.fps
    }

    root_bone = armature_obj.data.bones[0]
    bones_list = root_bone.children_recursive
    bones_list.insert(0, root_bone)

    parent_indices = []
    start_joints_positions = []
    blender_start_joints_positions = []
    root_bone_positions = []
    joints_angles = []

    for bone_idx in range(len(bones_list)):
        joint_angles = []

        bone = bones_list[bone_idx]
        pose_bone = pose_bones.get(bone.name)

        if bone_idx == 0:
            parent_indices.append(-1)

            location_fcu_x = fcurves.find('pose.bones["' + bone.name + '"].location', index=0)
            location_fcu_y = fcurves.find('pose.bones["' + bone.name + '"].location', index=1)
            location_fcu_z = fcurves.find('pose.bones["' + bone.name + '"].location', index=2)

            location_x = []
            location_y = []
            location_z = []

            for keyframe in location_fcu_x.keyframe_points:
                location_x.append(keyframe.co[1])
            for keyframe in location_fcu_y.keyframe_points:
                location_y.append(keyframe.co[1])
            for keyframe in location_fcu_z.keyframe_points:
                location_z.append(keyframe.co[1])

            for idx in range(len(location_x)):
                root_bone_positions.append([location_x[idx], location_y[idx], location_z[idx]])

        else:
            parent_bone = bone.parent_recursive[0]
            parent_index = bones_list.index(parent_bone)
            parent_indices.append(parent_index)

        start_joint_position = mathutils.Vector(bone.head)

        if bone_idx == 0:
            start_joint_position.rotate(correction_export_matrix)

        start_joint_position = start_joint_position + mathutils.Vector([0,0.00001,0])

        start_joints_positions.append(start_joint_position.to_tuple(21))

        corrected_blender_start_joint_position = mathutils.Vector(bone.head_local)
        corrected_blender_start_joint_position.rotate(correction_export_matrix)
        corrected_blender_start_joint_position = corrected_blender_start_joint_position + mathutils.Vector([0,0.00001,0])

        blender_start_joints_positions.append(corrected_blender_start_joint_position.to_tuple(21))

        rotation_quaternion_fcu_w = fcurves.find('pose.bones["' + bone.name + '"].rotation_quaternion', index=0)
        rotation_quaternion_fcu_x = fcurves.find('pose.bones["' + bone.name + '"].rotation_quaternion', index=1)
        rotation_quaternion_fcu_y = fcurves.find('pose.bones["' + bone.name + '"].rotation_quaternion', index=2)
        rotation_quaternion_fcu_z = fcurves.find('pose.bones["' + bone.name + '"].rotation_quaternion', index=3)

        rotation_quaternion_w = []
        rotation_quaternion_x = []
        rotation_quaternion_y = []
        rotation_quaternion_z = []

        for keyframe in rotation_quaternion_fcu_w.keyframe_points:
            rotation_quaternion_w.append(keyframe.co[1])
        for keyframe in rotation_quaternion_fcu_x.keyframe_points:
            rotation_quaternion_x.append(keyframe.co[1])
        for keyframe in rotation_quaternion_fcu_y.keyframe_points:
            rotation_quaternion_y.append(keyframe.co[1])
        for keyframe in rotation_quaternion_fcu_z.keyframe_points:
            rotation_quaternion_z.append(keyframe.co[1])

        for idx in range(len(rotation_quaternion_w)):
            joint_angles.append([rotation_quaternion_w[idx], rotation_quaternion_x[idx], rotation_quaternion_y[idx], rotation_quaternion_z[idx]])

        joints_angles.append(joint_angles)

    result = {
        "header": header,
        "parentIndices": parent_indices,
        "startJointsPositions": start_joints_positions,
        "blenderStartJointsPositions": blender_start_joints_positions,
        "rootBonePositions": root_bone_positions,
        "jointsAngles": joints_angles
    }

    with open(file_path, 'w') as f:
        json.dump(result, f)

    return {'FINISHED'}

class ExportJsonAn(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_an.json"
    bl_label = "Import JSON AN"

    # ExportHelper mixin class uses this
    filename_ext = ".an.json"

    filter_glob: StringProperty(
        default="*.an.json",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    def execute(self, context):
        return export_json_an(context, self.filepath)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportJsonAn.bl_idname, text="JSON AN Export(.an.json)")


def register():
    bpy.utils.register_class(ExportJsonAn)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportJsonAn)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
