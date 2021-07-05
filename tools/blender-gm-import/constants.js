export const CHAR_LENGTH = 1;

export const UINT8_LENGTH = 1;
export const UINT16_LENGTH = 2;
export const UINT32_LENGTH = 4;
export const INT32_LENGTH = 4;
export const FLOAT_LENGTH = 4;

export const RDF_FLAGS = {
  FLAGS_VISIBLE_PRESENT: 1,
  FLAGS_BSP_PRESENT: 2,
};

export const RDF_OBJECT_FLAGS = {
  VISIBLE: (1 << 0) >>> 0,
  STATIC_LIGHT_ENABLE: (1 << 1) >>> 0, // if no - object will not be lited
  DINAMIC_LIGHT_ENABLE: (1 << 2) >>> 0, // if no - no dynamic lighting perfomed
  CAST_SHADOWS_ENABLE: (1 << 3) >>> 0, // if material makes shadows
  COLLISION_ENABLE: (1 << 4) >>> 0, // if yes - object will be stored to BSP
  VERTEX_WEIGHT: (1 << 5) >>> 0, // animation
  MERGE: (1 << 6) >>> 0, // object is "merged"-type
  CULLENABLE: (1 << 7) >>> 0, // single-sided object
};
