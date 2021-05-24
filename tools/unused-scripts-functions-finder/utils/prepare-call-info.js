import cloneDeep from 'lodash.clonedeep';

const functionCallRegEx = /\b(\w+)\(.*?\)/g;

export default function prepareCallInfo(
  functionsList,
) {
  const list = cloneDeep(functionsList);

  Object.keys(list).forEach((funcName) => {
    const { body } = list[funcName];

    body.forEach((el) => {
      const callsMatches = [...el.matchAll(functionCallRegEx)];

      callsMatches.forEach((callMatch) => {
        const callFuncName = callMatch[1];

        if (list[callFuncName]) {
          if (!list[callFuncName].calledFrom.includes(funcName)) {
            list[callFuncName].calledFrom.push(funcName);
          }
          if (!list[funcName].calls.includes(callFuncName)
              // recursion check
              && funcName !== callFuncName) {
            list[funcName].calls.push(callFuncName);
          }
        }
      });
    });

    delete list[funcName].body;
  });

  return list;
}
