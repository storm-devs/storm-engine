import fs from 'fs';
import path from 'path';
import cloneDeep from 'lodash.clonedeep';

const functionDeclarationRegEx = /\w+ (\w+)\(.+[\n\t\s]*{/g;

function replaceDeclaration(script) {
  return script.replace(functionDeclarationRegEx, '{');
}

function prepareScriptBody(script, startIdx, endIdx) {
  return replaceDeclaration(script.slice(startIdx, endIdx));
}

export default function prepareData(
  dir,
  data = {
    functionsList: {},
    concatenatedScriptsWithoutDeclarations: '',
  },
) {
  let { concatenatedScriptsWithoutDeclarations } = data;
  let functionsList = cloneDeep(data.functionsList);

  const files = fs.readdirSync(dir);

  files.forEach((file) => {
    const name = `${dir}/${file}`;

    if (fs.statSync(name).isDirectory()) {
      const {
        functionsList: preparedFunctionsList,
        concatenatedScriptsWithoutDeclarations: preparedConcatenatedScriptsWithoutDeclarations,
      } = prepareData(name,
        {
          functionsList,
          concatenatedScriptsWithoutDeclarations,
        });

      functionsList = { ...functionsList, ...preparedFunctionsList };
      concatenatedScriptsWithoutDeclarations = preparedConcatenatedScriptsWithoutDeclarations;
    }

    if (path.extname(name) === '.c') {
      // I hate win-1251 :)
      const script = fs.readFileSync(name, { encoding: 'binary' });

      concatenatedScriptsWithoutDeclarations += replaceDeclaration(`\n${script}`);

      const matches = [...script.matchAll(functionDeclarationRegEx)];

      matches.forEach((match, idx) => {
        const funcName = match[1];
        const { index } = match;

        functionsList[funcName] = functionsList[funcName] || { calledFrom: [], calls: [] };

        if (idx > 0) {
          const prevFunc = matches[idx - 1];
          const prevFuncName = prevFunc[1];
          const prevIndex = prevFunc.index;

          const body = prepareScriptBody(script, prevIndex, index);

          functionsList[prevFuncName].body = functionsList[prevFuncName].body
            ? [...functionsList[prevFuncName].body, body]
            : [body];
        }

        if (idx === matches.length - 1) {
          const body = prepareScriptBody(script, index, script.length);

          functionsList[funcName].body = functionsList[funcName].body
            ? [...functionsList[funcName].body, body]
            : [body];
        }
      });
    }
  });

  return { functionsList, concatenatedScriptsWithoutDeclarations };
}
