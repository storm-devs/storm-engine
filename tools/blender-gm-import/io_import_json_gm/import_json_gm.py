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

from bpy_extras.io_utils import ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


def import_json_gm(context,file_path=""):
    file_name = os.path.basename(file_path)[:-8]
    textures_path = os.path.join(os.path.dirname(file_path),'textures')
    f = open(file_path,)
    data = json.load(f)

    xIsMirrored = data['xIsMirrored']

    collection = bpy.data.collections.new(file_name)
    bpy.context.scene.collection.children.link(collection)

    root = bpy.data.objects.new( "root", None )
    collection.objects.link(root)

    for object in data['objects']:
        name = object.get('name')

        vertices = object.get('verticies')
        normals = object.get('normals')
        uv_array = object.get('uv')
        uv_normals_array = object.get('uvNormals')
        faces = object.get('faces')
        colors = object.get('colors')

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

        """ bmesh.ops.rotate(bm, verts=bm.verts, cent=(0.0, 0.0, 0.0), matrix=mathutils.Matrix.Rotation(math.radians(90.0), 3, 'X'))
        bmesh.ops.rotate(bm, verts=bm.verts, cent=(0.0, 0.0, 0.0), matrix=mathutils.Matrix.Rotation(math.radians(90.0), 3, 'Z')) """

        """ TODO backface Culling """

        bpy.ops.object.mode_set(mode='OBJECT', toggle=False)
        bm.to_mesh(me)
        bm.free()

        col = me.vertex_colors.new()

        for polygon in me.polygons:
            for i, index in enumerate(polygon.vertices):
                loop_index = polygon.loop_indices[i]
                col.data[loop_index].color = colors[index]

        """ hack, texture is too dark without it """
        bpy.ops.object.mode_set(mode='EDIT', toggle=False)
        bpy.ops.object.mode_set(mode='OBJECT', toggle=False)

    for locators_tree in data['locatorsTrees']:
        group_locator_name = locators_tree
        group_locator = bpy.data.objects.new( group_locator_name, None )
        collection.objects.link(group_locator)
        group_locator.parent = root

        for locator_data in data['locatorsTrees'][locators_tree]:
            locator_name = locator_data['name']
            locator_m = locator_data['m']
            locator = bpy.data.objects.new( locator_name, None )
            collection.objects.link(locator)
            locator.parent = group_locator
            locator.matrix_basis = locator_m

            if xIsMirrored:
                locator.location[0] = -locator.location[0]

    root.rotation_euler[0] = math.radians(90)
    root.rotation_euler[2] = math.radians(90)
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

    def execute(self, context):
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
