/* eslint-disable import/extensions */
/* eslint-disable no-plusplus */
import {
  INT32_LENGTH,
  FLOAT_LENGTH,
} from './constants.js';

// eslint-disable-next-line import/prefer-default-export
export class AnReader {
  constructor(buffer) {
    this.buffer = buffer;
    this.currentOffset = 0;

    this.initFields();

    this.parseData();
  }

  initFields() {
    this.header = {};
    this.parentIndices = [];
    this.startJointsPositions = [];
    this.rootBonePositions = [];
    this.jointsAngles = [];
  }

  readInt32LE() {
    const value = this.buffer.readInt32LE(this.currentOffset);
    this.currentOffset += INT32_LENGTH;
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

  readD3DXQuaternion() {
    const x = this.readFloatLE();
    const y = this.readFloatLE();
    const z = this.readFloatLE();
    const w = this.readFloatLE();

    return {
      x, y, z, w,
    };
  }

  readHeader() {
    const nFrames = this.readInt32LE();
    const nJoints = this.readInt32LE();
    const framesPerSec = this.readFloatLE();

    this.header = {
      nFrames,
      nJoints,
      framesPerSec,
    };
  }

  readParentIndices() {
    for (let i = 0; i < this.header.nJoints; i++) {
      const idx = this.readInt32LE();
      this.parentIndices.push(idx);
    }
  }

  readStartJointsPositions() {
    for (let i = 0; i < this.header.nJoints; i++) {
      const vector = this.readVector();
      this.startJointsPositions.push(vector);
    }
  }

  readRootBonePositions() {
    for (let i = 0; i < this.header.nFrames; i++) {
      const vector = this.readVector();
      this.rootBonePositions.push(vector);
    }
  }

  readJointsAngles() {
    for (let i = 0; i < this.header.nJoints; i++) {
      this.jointsAngles[i] = [];
      for (let j = 0; j < this.header.nFrames; j++) {
        const d3DXQuaternion = this.readD3DXQuaternion();
        this.jointsAngles[i].push(d3DXQuaternion);
      }
    }
  }

  parseData() {
    this.readHeader();
    this.readParentIndices();
    this.readStartJointsPositions();
    this.readRootBonePositions();
    this.readJointsAngles();
  }

  prepareForBlenderImport() {
    const { header, parentIndices } = this;

    const startJointsPositions = this.startJointsPositions.map(({ x, y, z }) => [x, y, z]);

    const blenderStartJointsPositions = [];

    for (let i = 0; i < this.header.nJoints; i++) {
      const { x, y, z } = this.startJointsPositions[i];

      if (i === 0) {
        blenderStartJointsPositions.push([x, y, z]);
      } else {
        const [dX, dY, dZ] = blenderStartJointsPositions[this.parentIndices[i]];

        blenderStartJointsPositions.push([x + dX, y + dY, z + dZ]);
      }
    }

    const { x: rootStartX, y: rootStartY, z: rootStartZ } = this.startJointsPositions[0];
    const rootBonePositions = this.rootBonePositions.map(({ x, y, z }) => [x - rootStartX, y - rootStartY, z - rootStartZ]);

    const jointsAngles = this.jointsAngles.map((joint) => joint.map(({
      x, y, z, w,
    }) => (
      [w, x, y, z]
    )));

    return {
      header,
      parentIndices,
      startJointsPositions,
      blenderStartJointsPositions,
      rootBonePositions,
      jointsAngles,
    };
  }
}
