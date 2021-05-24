import cloneDeep from 'lodash.clonedeep';

const engineFunctions = [
  'Main',
  'ExitMain',
  'OnLoad',
  'OnSave',
];

export default function getRootUnusedFunctions(
  functionsList, concatenatedScriptsWithoutDeclarations,
) {
  const unusedRootFunctionsList = {};

  Object.keys(functionsList).forEach((funcName) => {
    if (funcName.toLowerCase().startsWith('lai_') || engineFunctions.join(' ').includes(funcName)) {
      return;
    }

    const { calledFrom } = functionsList[funcName];

    const funcCallRegEx = new RegExp(`\\b${funcName}\\(`);

    if (calledFrom.length === 0) {
      /* additional check for some cases, for example,
         nesting: `funcName1(funcName2())`, seems like regexp can't handle it */
      if (concatenatedScriptsWithoutDeclarations.search(funcCallRegEx) > -1) {
        return;
      }

      if (!concatenatedScriptsWithoutDeclarations.includes(`"${funcName}"`)) {
        unusedRootFunctionsList[funcName] = cloneDeep(functionsList[funcName]);
      }
    }
  });

  return unusedRootFunctionsList;
}
