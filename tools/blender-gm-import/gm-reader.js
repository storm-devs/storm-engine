/* eslint-disable import/extensions */
/* eslint-disable no-plusplus */
import {
  CHAR_LENGTH,
  UINT16_LENGTH,
  UINT32_LENGTH,
  INT32_LENGTH,
  FLOAT_LENGTH,
  RDF_FLAGS,
  RDF_OBJECT_FLAGS,
} from './constants.js';

const textDecoder = new TextDecoder('windows-1251');

// eslint-disable-next-line import/prefer-default-export
export class GmReader {
  constructor(buffer) {
    this.buffer = buffer;
    this.currentOffset = 0;

    this.initFields();

    this.parseData();
  }

  initFields() {
    this.header = {};
    this.names = new Map();
    this.textureNames = [];
    this.materials = [];
    this.labels = [];
    this.objects = [];
    this.triangles = [];
    this.vertexBuffers = [];
    this.vertices = [];
  }

  readUCharLE() {
    const value = this.buffer.readUInt8(this.currentOffset);
    this.currentOffset += CHAR_LENGTH;
    return value;
  }

  readUInt16LE() {
    const value = this.buffer.readUInt16LE(this.currentOffset);
    this.currentOffset += UINT16_LENGTH;
    return value;
  }

  readInt32LE() {
    const value = this.buffer.readInt32LE(this.currentOffset);
    this.currentOffset += INT32_LENGTH;
    return value;
  }

  readUInt32LE() {
    const value = this.buffer.readUInt32LE(this.currentOffset);
    this.currentOffset += UINT32_LENGTH;
    return value;
  }

  readFloatLE() {
    const value = this.buffer.readFloatLE(this.currentOffset);
    this.currentOffset += FLOAT_LENGTH;
    return value;
  }

  readVector() {
    const x = this.readFloatLE();
    const y = this.readFloatLE();
    const z = this.readFloatLE();

    return { x, y, z };
  }

  getHeaderFlagsNames(flags) {
    return Object.keys(RDF_FLAGS).map((flagName) => {
      // eslint-disable-next-line no-bitwise
      if ((RDF_FLAGS[flagName] & flags) >>> 0) {
        return flagName;
      }
    }).filter(Boolean);
  }

  readHeader() {
    const version = this.readInt32LE();

    const flags = this.readInt32LE();

    const flagsNames = this.getHeaderFlagsNames(flags);

    const nameSize = this.readInt32LE();

    const names = this.readInt32LE();

    const ntextures = this.readInt32LE();

    const nmaterials = this.readInt32LE();

    const nlights = this.readInt32LE();

    const nlabels = this.readInt32LE();

    const nobjects = this.readInt32LE();

    const ntriangles = this.readInt32LE();

    const nvrtbuffs = this.readInt32LE();

    const bboxSize = this.readVector();

    const bboxCenter = this.readVector();

    const radius = this.readFloatLE();

    this.header = {
      version,
      flags,
      flagsNames,
      nameSize,
      names,
      ntextures,
      nmaterials,
      nlights,
      nlabels,
      nobjects,
      ntriangles,
      nvrtbuffs,
      bboxSize,
      bboxCenter,
      radius,
    };
  }

  readGlobname() {
    const globname = textDecoder.decode(
      this.buffer
        .slice(this.currentOffset, this.currentOffset + this.header.nameSize),
    );
    this.currentOffset += this.header.nameSize;

    return globname;
  }

  readNamesOffsets() {
    const namesOffsets = [];
    for (let i = 0; i < this.header.names; i++) {
      namesOffsets.push(this.readInt32LE());
    }
    return namesOffsets;
  }

  readNames() {
    const globname = this.readGlobname();

    const namesOffsets = this.readNamesOffsets();

    for (let i = 0; i < namesOffsets.length; i++) {
      const currentOffset = namesOffsets[i];
      const nextOffset = i === namesOffsets.length - 1
        ? globname.length
        : namesOffsets[i + 1];
      const name = globname.slice(currentOffset, nextOffset).replace('\0', '');
      this.names.set(currentOffset, name);
    }
  }

  readTextureNamesOffsets() {
    const textureNamesOffsets = [];
    for (let i = 0; i < this.header.ntextures; i++) {
      textureNamesOffsets.push(this.readInt32LE());
    }
    return textureNamesOffsets;
  }

  readTextureNames() {
    const textureNamesOffsets = this.readTextureNamesOffsets();

    for (let i = 0; i < textureNamesOffsets.length; i++) {
      const currentOffset = textureNamesOffsets[i];
      const textureName = this.names.get(currentOffset);
      this.textureNames.push(textureName);
    }
  }

  readMaterials() {
    for (let i = 0; i < this.header.nmaterials; i++) {
      const groupNameIdx = this.readInt32LE();

      const groupName = this.names.get(groupNameIdx);

      const nameIdx = this.readInt32LE();

      const name = this.names.get(nameIdx);

      const diffuse = this.readFloatLE();

      const specular = this.readFloatLE();

      const gloss = this.readFloatLE();

      const selfIllum = this.readFloatLE();

      const textureType = [];
      for (let j = 0; j < 4; j++) {
        textureType.push(this.readInt32LE());
      }

      const texture = [];
      for (let j = 0; j < 4; j++) {
        texture.push(this.readInt32LE());
      }

      const textureNames = [];
      for (let j = 0; j < texture.length; j++) {
        const textureIdx = texture[j];
        if (textureIdx >= 0) {
          textureNames.push(this.textureNames[textureIdx]);
        }
      }

      this.materials.push({
        groupName,
        name,
        diffuse,
        specular,
        gloss,
        selfIllum,
        textureType,
        texture,
        textureNames,
      });
    }
  }

  // TODO
  readLights() {
    for (let i = 0; i < this.header.nlights; i++) {
      this.currentOffset += 80;
      console.log('readLights not implemented yet!');
    }
  }

  readLabels() {
    for (let i = 0; i < this.header.nlabels; i++) {
      const groupNameIdx = this.readInt32LE();

      const groupName = this.names.get(groupNameIdx);

      const nameIdx = this.readInt32LE();

      const name = this.names.get(nameIdx);

      const flags = this.readInt32LE();

      const m = [];
      for (let j = 0; j < 4; j++) {
        m[j] = [];
        for (let k = 0; k < 4; k++) {
          m[j].push(this.readFloatLE());
        }
      }

      const bones = [];
      for (let j = 0; j < 4; j++) {
        bones.push(this.readInt32LE());
      }

      const weight = [];
      for (let j = 0; j < 4; j++) {
        weight.push(this.readFloatLE());
      }

      this.labels.push({
        groupName,
        name,
        flags,
        m,
        bones,
        weight,
      });
    }
  }

  getObjectsFlagsNames(flags) {
    return Object.keys(RDF_OBJECT_FLAGS).map((flagName) => {
      // eslint-disable-next-line no-bitwise
      if ((RDF_OBJECT_FLAGS[flagName] & flags) >>> 0) {
        return flagName;
      }
    }).filter(Boolean);
  }

  readObjects() {
    for (let i = 0; i < this.header.nobjects; i++) {
      const groupNameIdx = this.readInt32LE();

      const groupName = this.names.get(groupNameIdx);

      const nameIdx = this.readInt32LE();

      const name = this.names.get(nameIdx);

      const flags = this.readInt32LE();

      const flagsNames = this.getObjectsFlagsNames(flags);

      const center = this.readVector();

      const radius = this.readFloatLE();

      const vertexBuff = this.readInt32LE();

      const ntriangles = this.readInt32LE();

      const striangle = this.readInt32LE();

      const nvertices = this.readInt32LE();

      const svertex = this.readInt32LE();

      const material = this.readInt32LE();

      const lights = [];
      for (let j = 0; j < 8; j++) {
        lights.push(this.readFloatLE());
      }

      const bones = [];
      for (let j = 0; j < 4; j++) {
        bones.push(this.readInt32LE());
      }

      const atriangles = this.readInt32LE();

      this.objects.push({
        groupName,
        name,
        flags,
        flagsNames,
        center,
        radius,
        vertexBuff,
        ntriangles,
        striangle,
        nvertices,
        svertex,
        material,
        lights,
        bones,
        atriangles,
      });
    }
  }

  readTriangles() {
    for (let i = 0; i < this.header.ntriangles; i++) {
      this.triangles.push([
        this.readUInt16LE(),
        this.readUInt16LE(),
        this.readUInt16LE(),
      ]);
    }
  }

  readVertexBuffers() {
    for (let i = 0; i < this.header.nvrtbuffs; i++) {
      const type = this.readInt32LE();
      const size = this.readInt32LE();

      // TODO magic
      // eslint-disable-next-line no-bitwise
      const stride = 36 + ((type & 3) >>> 0) * 8 + (type >>> 2) * 8;
      const nverts = parseInt(size / stride, 10);

      this.vertexBuffers.push({
        type,
        size,
        stride,
        nverts,
      });
    }
  }

  readColor() {
    const r = this.readUCharLE();
    const g = this.readUCharLE();
    const b = this.readUCharLE();
    const a = this.readUCharLE();

    return {
      r,
      g,
      b,
      a,
    };
  }

  readVertex0() {
    const pos = this.readVector();

    const norm = this.readVector();

    const color = this.readColor();

    const tu0 = this.readFloatLE();
    const tv0 = this.readFloatLE();

    return {
      pos,
      norm,
      color,
      tu0,
      tv0,
    };
  }

  readVertex1() {
    const pos = this.readVector();

    const norm = this.readVector();

    const color = this.readColor();

    const tu0 = this.readFloatLE();
    const tv0 = this.readFloatLE();

    const tu1 = this.readFloatLE();
    const tv1 = this.readFloatLE();

    return {
      pos,
      norm,
      color,
      tu0,
      tv0,
      tu1,
      tv1,
    };
  }

  readVertex2() {
    const pos = this.readVector();

    const norm = this.readVector();

    const color = this.readColor();

    const tu0 = this.readFloatLE();
    const tv0 = this.readFloatLE();

    const tu1 = this.readFloatLE();
    const tv1 = this.readFloatLE();

    const tu2 = this.readFloatLE();
    const tv2 = this.readFloatLE();

    return {
      pos,
      norm,
      color,
      tu0,
      tv0,
      tu1,
      tv1,
      tu2,
      tv2,
    };
  }

  readVertex3() {
    const pos = this.readVector();

    const norm = this.readVector();

    const color = this.readColor();

    const tu0 = this.readFloatLE();
    const tv0 = this.readFloatLE();

    const tu1 = this.readFloatLE();
    const tv1 = this.readFloatLE();

    const tu2 = this.readFloatLE();
    const tv2 = this.readFloatLE();

    const tu3 = this.readFloatLE();
    const tv3 = this.readFloatLE();

    return {
      pos,
      norm,
      color,
      tu0,
      tv0,
      tu1,
      tv1,
      tu2,
      tv2,
      tu3,
      tv3,
    };
  }

  readAVertex0() {
    const pos = this.readVector();

    const weight = this.readFloatLE();

    const boneId = this.readUInt32LE();

    const norm = this.readVector();

    const color = this.readColor();

    const tu0 = this.readFloatLE();
    const tv0 = this.readFloatLE();

    return {
      pos,
      weight,
      boneId,
      norm,
      color,
      tu0,
      tv0,
    };
  }

  readVertices() {
    for (let i = 0; i < this.header.nvrtbuffs; i++) {
      this.vertices[i] = [];
      const currentVertexBuffer = this.vertexBuffers[i];

      for (let j = 0; j < currentVertexBuffer.nverts; j++) {
        switch (currentVertexBuffer.type) {
          case 0:
            this.vertices[i].push(this.readVertex0());
            break;
          case 1:
            this.vertices[i].push(this.readVertex1());
            break;
          case 2:
            this.vertices[i].push(this.readVertex2());
            break;
          case 3:
            this.vertices[i].push(this.readVertex3());
            break;
          case 4:
            this.vertices[i].push(this.readAVertex0());
            break;
          default:
            break;
        }
      }
    }
  }

  readBspHeader() {
    const nnodes = this.readInt32LE();
    const nvertices = this.readInt32LE();
    const ntriangles = this.readInt32LE();

    return {
      nnodes,
      nvertices,
      ntriangles,
    };
  }

  readBspNodes(quantity) {
    const bspNodes = [];

    for (let i = 0; i < quantity; i++) {
      const norm = this.readVector();
      const pd = this.readFloatLE();

      const bitField = this.readUInt32LE();
      const node = (bitField & 0xFFFFFC00) >>> 0;
      const sign = (bitField & 0x200) >>> 0;
      const left = (bitField & 0x100) >>> 0;
      const nfaces = (bitField & 0xF0) >>> 0;
      const right = (bitField & 0xC) >>> 0;
      const type = (bitField & 0x3) >>> 0;

      const face = this.readInt32LE();

      bspNodes.push({
        norm,
        pd,
        bitField,
        node,
        sign,
        left,
        nfaces,
        right,
        type,
        face,
      });
    }

    return bspNodes;
  }

  readBspVerticies(quantity) {
    const bspVerticies = [];

    for (let i = 0; i < quantity; i++) {
      const vertex = this.readVector();

      bspVerticies.push(vertex);
    }

    return bspVerticies;
  }

  readBspTriangles(quantity) {
    const bspTriangles = [];

    for (let i = 0; i < quantity; i++) {
      const vindex = [];
      for (let j = 0; j < 3; j++) {
        vindex[j] = [];
        for (let k = 0; k < 3; k++) {
          vindex[j][k] = this.readUCharLE();
        }
      }

      bspTriangles.push(vindex);
    }

    return bspTriangles;
  }

  readBsp() {
    if (this.header.flagsNames.includes('FLAGS_BSP_PRESENT')) {
      const header = this.readBspHeader();

      const sroot = this.readBspNodes(header.nnodes);

      const vrt = this.readBspVerticies(header.nvertices);

      const btrg = this.readBspTriangles(header.ntriangles);

      this.bsp = {
        header,
        sroot,
        vrt,
        btrg,
      };
    }
  }

  parseData() {
    this.readHeader();
    this.readNames();
    this.readTextureNames();
    this.readMaterials();
    this.readLights();
    this.readLabels();
    this.readObjects();
    this.readTriangles();
    this.readVertexBuffers();
    this.readVertices();
    this.readBsp();
  }

  prepareForBlenderImport() {
    const data = [];

    for (let i = 0; i < this.objects.length; i++) {
      const {
        name, vertexBuff, ntriangles, striangle, nvertices, svertex, material: materialIdx,
      } = this.objects[i];

      const vertexBuffer = this.vertices[vertexBuff].slice(svertex, svertex + nvertices);

      const { type } = this.vertexBuffers[vertexBuff];

      const material = this.materials[materialIdx];
      const { groupName: materialGroupName, name: materialName, textureNames } = material;

      const verticies = vertexBuffer.map(({ pos: { x, y, z } }) => {
        // TODO check other types
        const preparedX = type !== 4 ? -x : x;
        return [preparedX, y, z];
      });
      // TODO mirror x on animated???
      const normals = vertexBuffer.map(({ norm: { x, y, z } }) => {
        // TODO check other types
        const preparedX = type !== 4 ? -x : x;
        return [preparedX, y, z];
      });

      // TODO multiple uvs
      const uv = vertexBuffer.map(({ tu0, tv0 }) => [tu0, tv0 * -1]);

      // TODO check if normals can be uv0
      const uvNormalsArray = vertexBuffer.map(({ tu1, tv1 }) => typeof tu1 !== 'undefined' && [tu1, tv1 * -1]).filter(Boolean);
      const uvNormals = uvNormalsArray.length > 0 ? uvNormalsArray : undefined;

      const colors = vertexBuffer.map(({
        color: {
          r, g, b, a,
        },
      }) => [r / 255, g / 255, b / 255, a / 255]);

      const faces = this.triangles.slice(striangle, striangle + ntriangles).map(([v1, v2, v3]) => [v1, v2, v3]);

      data.push({
        name,
        verticies,
        normals,
        uv,
        uvNormals,
        colors,
        faces,
        material: {
          groupName: materialGroupName,
          name: materialName,
          textureNames,
        },
      });
    }

    return data;
  }
}
