/* eslint-disable import/extensions */
import fs from 'fs';
import path from 'path';
import { AnReader } from './an-reader.js';

const dirname = process.cwd();

const anPath = process.argv.find((argv) => argv.includes('.an'));

if (!anPath) {
  throw new Error('anPath is not set!');
}

if (!anPath.includes('.an')) {
  throw new Error('Wrong file format!');
}

const isDevMode = process.argv.includes('--dev');

const anName = path.basename(anPath);

const preparedPath = path.resolve(dirname, anPath);
const resultDir = isDevMode ? dirname : path.dirname(preparedPath);

const modelBuffer = fs.readFileSync(preparedPath);

const anReader = new AnReader(modelBuffer);

if (isDevMode) {
  fs.writeFileSync(
    path.resolve(resultDir, 'result-raw.json'),
    // cheap filter, kind of
    JSON.stringify({ ...anReader, buffer: undefined, currentOffset: undefined },
      '',
      2),
  );
}

const resultName = isDevMode ? 'result.an.json' : `${anName}.json`;

fs.writeFileSync(
  path.resolve(resultDir, resultName),
  JSON.stringify(anReader.prepareForBlenderImport()),
);
