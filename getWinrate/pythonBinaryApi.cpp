#include "pythonBinaryApi.h"
#include "Python.h"

int makeAStakeInPython (std::string pairname = NULL, std::string expireTime = NULL, int moneyAmount = NULL, bool stavitVverh = NULL) {


    // Инициализируем питон или что там, в общем в любом случае
    // всегда в начале писать надо
    Py_Initialize();

    char filename[] = "makeStake.py";
    FILE* fp;
    fp = _Py_fopen(filename, "r");
    PyRun_SimpleFile(fp, filename);

    if (Py_FinalizeEx() < 0) {
        return 120;
    }

}

/*
PyObject *pName, *pModule, *pFunc;
PyObject *pArgs, *pValue;

std::string funcNameInPyFile = "makeStake";
std::string pyModuleName = "makeStake";


pName = PyUnicode_DecodeFSDefault(pyModuleName.c_str());
// Загружаем .py файл (pName должен быть без .py)
pModule = PyImport_Import(pName);
// Удаляем pName
//    Py_DECREF(pName);

if (pModule != NULL) {
pFunc = PyObject_GetAttrString(pModule, funcNameInPyFile.c_str());

if (pFunc && PyCallable_Check(pFunc)) {
pArgs = PyTuple_New(5);

PyObject *pDirection;
if (stavitVverh) {
pDirection = PyUnicode_FromString("CALL");
} else {
pDirection = PyUnicode_FromString("PUT");
}
PyObject *pMoneyAmount = PyLong_FromLong(moneyAmount);
pairname.insert(0, "frx");
pairname.erase(pairname.find('/'), 1);
PyObject *pPairname = PyUnicode_FromString(pairname.c_str());

// Чтобы получить цифры, просто убираем последний символ, так как там всегда 'm', 'h' или 'd'
std::string timeAmountInString = expireTime;
timeAmountInString.pop_back();
int timeAmountInInt = std::stoi(timeAmountInString);
PyObject *pTimeAmount = PyLong_FromLong(timeAmountInInt);
// Чтобы получить букву, просто оставляем только последний символ, так как у меня возможные
// значения только 1-2 цифры (записанные в string) и на конце всегда буква ('m', 'h' или 'd')
char* duration_unit[0] = {};
duration_unit[0] = reinterpret_cast<char *>(expireTime.back());
PyObject *pDurationUnit = PyUnicode_FromString(reinterpret_cast<const char *>(duration_unit));

// TODO: тут может происходить утечка памяти, по-хорошему надо бы Py_DECREF, наверное
PyTuple_SetItem(pArgs, 0, pDirection);
PyTuple_SetItem(pArgs, 1, pMoneyAmount);
PyTuple_SetItem(pArgs, 2, pPairname);
PyTuple_SetItem(pArgs, 3, pTimeAmount);
PyTuple_SetItem(pArgs, 4, pDurationUnit);
//            Py_DecRef(pDirection);
//            Py_DecRef(pMoneyAmount);
//            Py_DecRef(pPairname);
//            Py_DecRef(pTimeAmount);
//            Py_DecRef(pDurationUnit);

// TODO: Здесь EXC_BAD_ACCESS. К слову, я не исправил pairname (TODO пятью строчками выше)
pValue = PyObject_CallObject(pFunc, pArgs);
//            Py_DECREF(pArgs);
if (pValue != NULL) {
printf("Result of call: %ld\n", PyLong_AsLong(pValue));
//                Py_DECREF(pValue);
} else {
//                Py_DECREF(pFunc);
//                Py_DECREF(pModule);
PyErr_Print();
fprintf(stderr,"Call failed\n");
return 1;
}

}
else {
if (PyErr_Occurred())
PyErr_Print();
fprintf(stderr, "Cannot find function \"%s\"\n", "makeStake");
}
Py_XDECREF(pFunc);
Py_DECREF(pModule);

}
else {
PyErr_Print();
fprintf(stderr, "Failed to load \"%s\"\n", "makeStake");
return 1;
}
 */