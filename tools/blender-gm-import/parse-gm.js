/* eslint-disable import/extensions */
import fs from 'fs';
import path from 'path';
import { GmReader } from './gm-reader.js';

const dirname = process.cwd();

const gmPath = process.argv.find((argv) => argv.includes('.gm'));

if (!gmPath) {
  throw new Error('gmPath is not set!');
}

if (!gmPath.includes('.gm')) {
  throw new Error('Wrong file format!');
}

const isDevMode = process.argv.includes('--dev');

const gmName = path.basename(gmPath);

const preparedPath = path.resolve(dirname, gmPath);
const resultDir = isDevMode ? dirname : path.dirname(preparedPath);

const modelBuffer = fs.readFileSync(preparedPath);

const gmReader = new GmReader(modelBuffer);

console.log(`Used textures: \n\n${gmReader.textureNames.join('\n')}`);

if (isDevMode) {
  fs.writeFileSync(
    path.resolve(resultDir, 'result-raw.json'),
    // cheap filter, kind of
    JSON.stringify({ ...gmReader, buffer: undefined, currentOffset: undefined },
      '',
      2),
  );
}

const resultName = isDevMode ? 'result.gm.json' : `${gmName}.json`;

fs.writeFileSync(
  path.resolve(resultDir, resultName),
  JSON.stringify(gmReader.prepareForBlenderImport()),
);
