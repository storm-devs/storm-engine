/* eslint-disable import/extensions */
import fs from 'fs';
import path from 'path';
import { AnWriter } from './an-writer.js';

const dirname = process.cwd();

const anJsonPath = process.argv.find((argv) => argv.includes('.an.json'));

if (!anJsonPath) {
  throw new Error('anJsonPath is not set!');
}

if (!anJsonPath.includes('.an.json')) {
  throw new Error('Wrong file format!');
}

const isDevMode = process.argv.includes('--dev');

const anName = path.basename(anJsonPath);

const preparedPath = path.resolve(dirname, anJsonPath);
const resultDir = isDevMode ? dirname : path.dirname(preparedPath);

const anJsonBuffer = fs.readFileSync(preparedPath);

const anWriter = new AnWriter(anJsonBuffer);

const resultName = isDevMode ? 'result.an' : `${anName.replace(/(\.an)+\.json/, '.an')}`;

fs.writeFileSync(
  path.resolve(resultDir, resultName),
  anWriter.getAnimationFile(),
);
