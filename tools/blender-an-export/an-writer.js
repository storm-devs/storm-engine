/* eslint-disable import/extensions */
/* eslint-disable no-plusplus */
import {
  INT32_LENGTH,
  FLOAT_LENGTH,
} from './constants.js';

// eslint-disable-next-line import/prefer-default-export
export class AnWriter {
  constructor(buffer) {
    this.anJson = JSON.parse(buffer);

    this.buffer = Buffer.alloc(0);
    this.bufferArray = [];

    this.initFields();
    this.prepareData();
  }

  initFields() {
    this.header = this.anJson.header;
    this.parentIndices = this.anJson.parentIndices;
    this.startJointsPositions = this.anJson.startJointsPositions;
    this.rootBonePositions = this.anJson.rootBonePositions;
    this.jointsAngles = this.anJson.jointsAngles;
  }

  writeInt32LE(value) {
    const buffer = Buffer.alloc(INT32_LENGTH);
    buffer.writeInt32LE(value);
    this.bufferArray.push(buffer);
  }

  writeFloatLE(value) {
    const buffer = Buffer.alloc(FLOAT_LENGTH);
    buffer.writeFloatLE(value);
    this.bufferArray.push(buffer);
  }

  writeVector([x, y, z]) {
    this.writeFloatLE(x);
    this.writeFloatLE(y);
    this.writeFloatLE(z);
  }

  writeD3DXQuaternion([w, x, y, z]) {
    this.writeFloatLE(x);
    this.writeFloatLE(y);
    this.writeFloatLE(z);
    this.writeFloatLE(w);
  }

  writeHeader() {
    const {
      nFrames,
      nJoints,
      framesPerSec,
    } = this.header;

    this.writeInt32LE(nFrames);
    this.writeInt32LE(nJoints);
    this.writeFloatLE(framesPerSec);
  }

  writeParentIndices() {
    this.parentIndices.forEach((idx) => this.writeInt32LE(idx));
  }

  writeStartJointsPositions() {
    this.startJointsPositions.forEach((vector) => this.writeVector(vector));
  }

  writeRootBonePositions() {
    const [rootStartX, rootStartY, rootStartZ] = this.startJointsPositions[0];
    this.rootBonePositions.forEach(([x, y, z]) => this.writeVector([x + rootStartX, y + rootStartY, z + rootStartZ]));
  }

  writeJointsAngles() {
    this.jointsAngles.forEach((joint) => {
      joint.forEach((angle) => this.writeD3DXQuaternion(angle));
    });
  }

  prepareData() {
    this.writeHeader();
    this.writeParentIndices();
    this.writeStartJointsPositions();
    this.writeRootBonePositions();
    this.writeJointsAngles();

    this.buffer = Buffer.concat(this.bufferArray);
  }

  getAnimationFile() {
    return this.buffer;
  }
}
