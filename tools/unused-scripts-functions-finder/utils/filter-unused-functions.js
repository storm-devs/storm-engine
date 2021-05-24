import cloneDeep from 'lodash.clonedeep';

export default function filterUnusedFunctions(functionsList, unusedFunctionsList) {
  const unusedList = cloneDeep(unusedFunctionsList);

  const unusedFunctionNames = Object.keys(unusedFunctionsList);

  unusedFunctionNames.forEach((funcName) => {
    const { calledFrom } = functionsList[funcName];

    if (calledFrom.length > 0) {
      const filteredCalledFrom = calledFrom.filter((el) => unusedFunctionNames.includes(el));

      if (calledFrom.join(' ') !== filteredCalledFrom.join(' ')) {
        delete unusedList[funcName];
      }
    }
  });

  return unusedList;
}
