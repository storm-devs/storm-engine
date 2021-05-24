import cloneDeep from 'lodash.clonedeep';

export default function getRootUnusedFunctionsWithNestedCalls(
  functionsList, unusedFunctionsList, unusedFunctionName = '',
) {
  let unusedList = cloneDeep(unusedFunctionsList);

  if (!unusedFunctionName) {
    Object.keys(unusedList).forEach((unusedFuncName) => {
      unusedList = getRootUnusedFunctionsWithNestedCalls(functionsList, unusedList, unusedFuncName);
    });
  } else if (functionsList[unusedFunctionName].calls.length > 0
      && !unusedList[unusedFunctionName]?.isHandled
  ) {
    functionsList[unusedFunctionName].calls.forEach((callFuncName) => {
      if (!callFuncName.toLowerCase().startsWith('lai_')) {
        unusedList[callFuncName] = { ...functionsList[callFuncName], isHandled: true };

        if (unusedList[callFuncName].calls.length > 0) {
          unusedList = getRootUnusedFunctionsWithNestedCalls(
            functionsList, unusedList, callFuncName,
          );
        }
      }
    });
  }

  return unusedList;
}
