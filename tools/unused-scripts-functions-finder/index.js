/* eslint-disable import/extensions */
import fs from 'fs';
import path from 'path';
import prepareData from './utils/prepare-data.js';
import prepareCallInfo from './utils/prepare-call-info.js';
import getRootUnusedFunctions from './utils/get-root-unused-functions.js';
import getRootUnusedFunctionsWithNestedCalls from './utils/get-root-unused-functions-with-nested-calls.js';
import filterUnusedFunctions from './utils/filter-unused-functions.js';

const dirname = process.cwd();

const scriptsPath = process.argv[2];

if (!scriptsPath) {
  throw new Error('scriptsPath not set!');
}

const workingDir = path.resolve(dirname, scriptsPath);

console.log(`WorkingDir: ${workingDir}`);
console.log('Work in progress, please wait!');

const {
  functionsList,
  concatenatedScriptsWithoutDeclarations,
} = prepareData(workingDir);

const functionsListWithCallInfo = prepareCallInfo(functionsList);

const rootUnusedFunctions = getRootUnusedFunctions(
  functionsListWithCallInfo, concatenatedScriptsWithoutDeclarations,
);

const rootUnusedFunctionsWithNestedCalls = getRootUnusedFunctionsWithNestedCalls(
  functionsListWithCallInfo, rootUnusedFunctions,
);

const filteredUnusedFunctions = filterUnusedFunctions(
  functionsListWithCallInfo, rootUnusedFunctionsWithNestedCalls,
);

const unusedFunctionsNamesList = Object.keys(filteredUnusedFunctions);

fs.writeFileSync(path.resolve(dirname, 'result.txt'), unusedFunctionsNamesList.join('\n'));
