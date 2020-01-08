extern SciErr (* getMatrixOfDouble) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,double **_pdblReal);
extern SciErr (* getComplexMatrixOfDouble) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,double **_pdblReal,double **_pdblImg);
extern SciErr (* getComplexZMatrixOfDouble) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,doublecomplex **_pdblZ);
extern SciErr (* allocMatrixOfDouble) (void *_pvCtx,int _iVar,int _iRows,int _iCols,double **_pdblReal);
extern SciErr (* allocComplexMatrixOfDouble) (void *_pvCtx,int _iVar,int _iRows,int _iCols,double **_pdblReal,double **_pdblImg);
extern SciErr (* createMatrixOfDouble) (void *_pvCtx,int _iVar,int _iRows,int _iCols,double const *_pdblReal);
extern SciErr (* createComplexMatrixOfDouble) (void *_pvCtx,int _iVar,int _iRows,int _iCols,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* createComplexZMatrixOfDouble) (void *_pvCtx,int _iVar,int _iRows,int _iCols,doublecomplex const *_pdblData);
extern SciErr (* createNamedMatrixOfDouble) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,double const *_pdblReal);
extern SciErr (* createNamedComplexMatrixOfDouble) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* createNamedComplexZMatrixOfDouble) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,doublecomplex const *_pdblData);
extern SciErr (* readNamedMatrixOfDouble) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,double *_pdblReal);
extern SciErr (* readNamedComplexMatrixOfDouble) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,double *_pdblReal,double *_pdblImg);
extern int (* isDoubleType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedDoubleType) (void *_pvCtx,char const *_pstName);
extern int (* getScalarDouble) (void *_pvCtx,int *_piAddress,double *_pdblReal);
extern int (* getScalarComplexDouble) (void *_pvCtx,int *_piAddress,double *_pdblReal,double *_pdblImg);
extern int (* getNamedScalarDouble) (void *_pvCtx,char const *_pstName,double *_pdblReal);
extern int (* getNamedScalarComplexDouble) (void *_pvCtx,char const *_pstName,double *_pdblReal,double *_pdblImg);
extern int (* createScalarDouble) (void *_pvCtx,int _iVar,double _dblReal);
extern int (* createScalarComplexDouble) (void *_pvCtx,int _iVar,double _dblReal,double _dblImg);
extern int (* createNamedScalarDouble) (void *_pvCtx,char const *_pstName,double _dblReal);
extern int (* createNamedScalarComplexDouble) (void *_pvCtx,char const *_pstName,double _dblReal,double _dblImg);
extern SciErr (* getMatrixOfString) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piLength,char **_pstStrings);
extern SciErr (* getMatrixOfWideString) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piwLength,wchar_t **_pwstStrings);
extern SciErr (* createMatrixOfString) (void *_pvCtx,int _iVar,int _iRows,int _iCols,char const *const *_pstStrings);
extern SciErr (* createMatrixOfWideString) (void *_pvCtx,int _iVar,int _iRows,int _iCols,wchar_t const *const *_pwstStrings);
extern SciErr (* createNamedMatrixOfString) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,char const *const *_pstStrings);
extern SciErr (* createNamedMatrixOfWideString) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,wchar_t const *const *_pwstStrings);
extern SciErr (* readNamedMatrixOfString) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piLength,char **_pstStrings);
extern SciErr (* readNamedMatrixOfWideString) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piwLength,wchar_t **_pwstStrings);
extern int (* isStringType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedStringType) (void *_pvCtx,char const *_pstName);
extern int (* getAllocatedSingleString) (void *_pvCtx,int *_piAddress,char **_pstData);
extern int (* getAllocatedSingleWideString) (void *_pvCtx,int *_piAddress,wchar_t **_pwstData);
extern int (* getAllocatedMatrixOfString) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,char ***_pstData);
extern int (* getAllocatedMatrixOfWideString) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,wchar_t ***_pwstData);
extern int (* getAllocatedNamedSingleString) (void *_pvCtx,char const *_pstName,char **_pstData);
extern int (* getAllocatedNamedSingleWideString) (void *_pvCtx,char const *_pstName,wchar_t **_pwstData);
extern int (* getAllocatedNamedMatrixOfString) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,char ***_pstData);
extern int (* getAllocatedNamedMatrixOfWideString) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,wchar_t ***_pwstData);
extern int (* createSingleString) (void *_pvCtx,int _iVar,char const *_pstStrings);
extern int (* createSingleWideString) (void *_pvCtx,int _iVar,wchar_t const *_pwstStrings);
extern int (* createNamedSingleString) (void *_pvCtx,char const *_pstName,char const *_pstStrings);
extern int (* createNamedSingleWideString) (void *_pvCtx,char const *_pstName,wchar_t const *_pwstStrings);
extern void (* freeAllocatedSingleString) (char *_pstData);
extern void (* freeAllocatedSingleWideString) (wchar_t *_pwstData);
extern void (* freeAllocatedMatrixOfString) (int _iRows,int _iCols,char **_pstData);
extern void (* freeAllocatedMatrixOfWideString) (int _iRows,int _iCols,wchar_t **_pwstData);
extern SciErr (* getMatrixOfIntegerPrecision) (void *_pvCtx,int *_piAddress,int *_piPrecision);
extern SciErr (* getMatrixOfInteger8) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,char **_pcData8);
extern SciErr (* getMatrixOfUnsignedInteger8) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,unsigned char **_pucData8);
extern SciErr (* getMatrixOfInteger16) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,short **_psData16);
extern SciErr (* getMatrixOfUnsignedInteger16) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,unsigned short **_pusData16);
extern SciErr (* getMatrixOfInteger32) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int **_piData32);
extern SciErr (* getMatrixOfUnsignedInteger32) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,unsigned int **_puiData32);
extern SciErr (* createMatrixOfInteger8) (void *_pvCtx,int _iVar,int _iRows,int _iCols,char const *_pcData8);
extern SciErr (* createMatrixOfUnsignedInteger8) (void *_pvCtx,int _iVar,int _iRows,int _iCols,unsigned char const *_pucData8);
extern SciErr (* createMatrixOfInteger16) (void *_pvCtx,int _iVar,int _iRows,int _iCols,short const *_psData16);
extern SciErr (* createMatrixOfUnsignedInteger16) (void *_pvCtx,int _iVar,int _iRows,int _iCols,unsigned short const *_pusData16);
extern SciErr (* createMatrixOfInteger32) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int const *_piData32);
extern SciErr (* createMatrixOfUnsignedInteger32) (void *_pvCtx,int _iVar,int _iRows,int _iCols,unsigned int const *_puiData32);
extern SciErr (* createNamedMatrixOfInteger8) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,char const *_pcData8);
extern SciErr (* createNamedMatrixOfUnsignedInteger8) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,unsigned char const *_pucData8);
extern SciErr (* createNamedMatrixOfInteger16) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,short const *_psData16);
extern SciErr (* createNamedMatrixOfUnsignedInteger16) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,unsigned short const *_pusData16);
extern SciErr (* createNamedMatrixOfInteger32) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,int const *_piData32);
extern SciErr (* createNamedMatrixOfUnsignedInteger32) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,unsigned int const *_puiData32);
extern SciErr (* allocMatrixOfInteger8) (void *_pvCtx,int _iVar,int _iRows,int _iCols,char **_pcData8);
extern SciErr (* allocMatrixOfUnsignedInteger8) (void *_pvCtx,int _iVar,int _iRows,int _iCols,unsigned char **_pucData8);
extern SciErr (* allocMatrixOfInteger16) (void *_pvCtx,int _iVar,int _iRows,int _iCols,short **_psData16);
extern SciErr (* allocMatrixOfUnsignedInteger16) (void *_pvCtx,int _iVar,int _iRows,int _iCols,unsigned short **_pusData16);
extern SciErr (* allocMatrixOfInteger32) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int **_piData32);
extern SciErr (* allocMatrixOfUnsignedInteger32) (void *_pvCtx,int _iVar,int _iRows,int _iCols,unsigned int **_puiData32);
extern SciErr (* getNamedMatrixOfIntegerPrecision) (void *_pvCtx,char const *_pstName,int *_piPrecision);
extern SciErr (* readNamedMatrixOfInteger8) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,char *_pcData8);
extern SciErr (* readNamedMatrixOfUnsignedInteger8) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,unsigned char *_pucData8);
extern SciErr (* readNamedMatrixOfInteger16) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,short *_psData16);
extern SciErr (* readNamedMatrixOfUnsignedInteger16) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,unsigned short *_pusData16);
extern SciErr (* readNamedMatrixOfInteger32) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piData32);
extern SciErr (* readNamedMatrixOfUnsignedInteger32) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,unsigned int *_puiData32);
extern int (* isIntegerType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedIntegerType) (void *_pvCtx,char const *_pstName);
extern int (* getScalarInteger8) (void *_pvCtx,int *_piAddress,char *_pcData);
extern int (* getScalarUnsignedInteger8) (void *_pvCtx,int *_piAddress,unsigned char *_pucData);
extern int (* getScalarInteger16) (void *_pvCtx,int *_piAddress,short *_psData);
extern int (* getScalarUnsignedInteger16) (void *_pvCtx,int *_piAddress,unsigned short *_pusData);
extern int (* getScalarInteger32) (void *_pvCtx,int *_piAddress,int *_piData);
extern int (* getScalarUnsignedInteger32) (void *_pvCtx,int *_piAddress,unsigned int *_puiData);
extern int (* getNamedScalarInteger8) (void *_pvCtx,char const *_pstName,char *_pcData);
extern int (* getNamedScalarUnsignedInteger8) (void *_pvCtx,char const *_pstName,unsigned char *_pucData);
extern int (* getNamedScalarInteger16) (void *_pvCtx,char const *_pstName,short *_psData);
extern int (* getNamedScalarUnsignedInteger16) (void *_pvCtx,char const *_pstName,unsigned short *_pusData);
extern int (* getNamedScalarInteger32) (void *_pvCtx,char const *_pstName,int *_piData);
extern int (* getNamedScalarUnsignedInteger32) (void *_pvCtx,char const *_pstName,unsigned int *_puiData);
extern int (* createScalarInteger8) (void *_pvCtx,int _iVar,char _cData);
extern int (* createScalarUnsignedInteger8) (void *_pvCtx,int _iVar,unsigned char _ucData);
extern int (* createScalarInteger16) (void *_pvCtx,int _iVar,short _sData);
extern int (* createScalarUnsignedInteger16) (void *_pvCtx,int _iVar,unsigned short _usData);
extern int (* createScalarInteger32) (void *_pvCtx,int _iVar,int _iData);
extern int (* createScalarUnsignedInteger32) (void *_pvCtx,int _iVar,unsigned int _uiData);
extern int (* createNamedScalarInteger8) (void *_pvCtx,char const *_pstName,char _cData);
extern int (* createNamedScalarUnsignedInteger8) (void *_pvCtx,char const *_pstName,unsigned char _ucData);
extern int (* createNamedScalarInteger16) (void *_pvCtx,char const *_pstName,short _sData);
extern int (* createNamedScalarUnsignedInteger16) (void *_pvCtx,char const *_pstName,unsigned short _usData);
extern int (* createNamedScalarInteger32) (void *_pvCtx,char const *_pstName,int _iData);
extern int (* createNamedScalarUnsignedInteger32) (void *_pvCtx,char const *_pstName,unsigned int _uiData);
extern SciErr (* getPolyVariableName) (void *_pvCtx,int *_piAddress,char *_pstVarName,int *_piVarNameLen);
extern SciErr (* getMatrixOfPoly) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal);
extern SciErr (* getComplexMatrixOfPoly) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern SciErr (* createMatrixOfPoly) (void *_pvCtx,int _iVar,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal);
extern SciErr (* createComplexMatrixOfPoly) (void *_pvCtx,int _iVar,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal,double const *const *_pdblImg);
extern SciErr (* createNamedMatrixOfPoly) (void *_pvCtx,char const *_pstName,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal);
extern SciErr (* createNamedComplexMatrixOfPoly) (void *_pvCtx,char const *_pstName,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal,double const *const *_pdblImg);
extern SciErr (* readNamedMatrixOfPoly) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal);
extern SciErr (* readNamedComplexMatrixOfPoly) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern int (* isPolyType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedPolyType) (void *_pvCtx,char const *_pstName);
extern int (* getAllocatedSinglePoly) (void *_pvCtx,int *_piAddress,int *_piNbCoef,double **_pdblReal);
extern int (* getAllocatedSingleComplexPoly) (void *_pvCtx,int *_piAddress,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern int (* getAllocatedNamedSinglePoly) (void *_pvCtx,char const *_pstName,int *_piNbCoef,double **_pdblReal);
extern int (* getAllocatedNamedSingleComplexPoly) (void *_pvCtx,char const *_pstName,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern int (* getAllocatedMatrixOfPoly) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int **_piNbCoef,double ***_pdblReal);
extern int (* getAllocatedMatrixOfComplexPoly) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int **_piNbCoef,double ***_pdblReal,double ***_pdblImg);
extern int (* getAllocatedNamedMatrixOfPoly) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int **_piNbCoef,double ***_pdblReal);
extern int (* getAllocatedNamedMatrixOfComplexPoly) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int **_piNbCoef,double ***_pdblReal,double ***_pdblImg);
extern void (* freeAllocatedSinglePoly) (double *_pdblReal);
extern void (* freeAllocatedSingleComplexPoly) (double *_pdblReal,double *_pdblImg);
extern void (* freeAllocatedMatrixOfPoly) (int _iRows,int _iCols,int *_piNbCoef,double **_pdblReal);
extern void (* freeAllocatedMatrixOfComplexPoly) (int _iRows,int _iCols,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern SciErr (* getSparseMatrix) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal);
extern SciErr (* getComplexSparseMatrix) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal,double **_pdblImg);
extern SciErr (* allocSparseMatrix) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int _iNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal);
extern SciErr (* allocComplexSparseMatrix) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int _iNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal,double **_pdblImg);
extern SciErr (* createSparseMatrix) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal);
extern SciErr (* createComplexSparseMatrix) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* createNamedSparseMatrix) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal);
extern SciErr (* createNamedComplexSparseMatrix) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* readNamedSparseMatrix) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbItem,int *_piNbItemRow,int *_piColPos,double *_pdblReal);
extern SciErr (* readNamedComplexSparseMatrix) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbItem,int *_piNbItemRow,int *_piColPos,double *_pdblReal,double *_pdblImg);
extern int (* isSparseType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedSparseType) (void *_pvCtx,char const *_pstName);
extern int (* getAllocatedSparseMatrix) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal);
extern int (* getAllocatedComplexSparseMatrix) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal,double **_pdblImg);
extern int (* getNamedAllocatedSparseMatrix) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal);
extern int (* getNamedAllocatedComplexSparseMatrix) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal,double **_pdblImg);
extern void (* freeAllocatedSparseMatrix) (int *_piNbItemRows,int *_piColPos,double *_pdblReal);
extern void (* freeAllocatedComplexSparseMatrix) (int *_piNbItemRows,int *_piColPos,double *_pdblReal,double *_pdblImg);
extern SciErr (* getMatrixOfBoolean) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int **_piBool);
extern SciErr (* allocMatrixOfBoolean) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int **_piBool);
extern SciErr (* createMatrixOfBoolean) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int const *_piBool);
extern SciErr (* createNamedMatrixOfBoolean) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,int const *_piBool);
extern SciErr (* readNamedMatrixOfBoolean) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piBool);
extern int (* isBooleanType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedBooleanType) (void *_pvCtx,char const *_pstName);
extern int (* getScalarBoolean) (void *_pvCtx,int *_piAddress,int *_piBool);
extern int (* getNamedScalarBoolean) (void *_pvCtx,char const *_pstName,int *_piBool);
extern int (* createScalarBoolean) (void *_pvCtx,int _iVar,int _iBool);
extern int (* createNamedScalarBoolean) (void *_pvCtx,char const *_pstName,int _iBool);
extern SciErr (* getBooleanSparseMatrix) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos);
extern SciErr (* allocBooleanSparseMatrix) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int _iNbItem,int **_piNbItemRow,int **_piColPos);
extern SciErr (* createBooleanSparseMatrix) (void *_pvCtx,int _iVar,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos);
extern SciErr (* createNamedBooleanSparseMatrix) (void *_pvCtx,char const *_pstName,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos);
extern SciErr (* readNamedBooleanSparseMatrix) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbItem,int *_piNbItemRow,int *_piColPos);
extern int (* isBooleanSparseType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedBooleanSparseType) (void *_pvCtx,char const *_pstName);
extern int (* getAllocatedBooleanSparseMatrix) (void *_pvCtx,int *_piAddress,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos);
extern int (* getNamedAllocatedBooleanSparseMatrix) (void *_pvCtx,char const *_pstName,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos);
extern void (* freeAllocatedBooleanSparse) (int *_piNbItemRow,int *_piColPos);
extern SciErr (* getPointer) (void *_pvCtx,int *_piAddress,void **_pvPtr);
extern SciErr (* allocPointer) (void *_pvCtx,int _iVar,void **_pvPtr);
extern SciErr (* createPointer) (void *_pvCtx,int _iVar,void *_pvPtr);
extern SciErr (* createNamedPointer) (void *_pvCtx,char const *_pstName,void *_pvPtr);
extern SciErr (* readNamedPointer) (void *_pvCtx,char const *_pstName,void **_pvPtr);
extern int (* isPointerType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedPointerType) (void *_pvCtx,char const *_pstName);
extern SciErr (* getListItemNumber) (void *_pvCtx,int *_piAddress,int *_piNbItem);
extern SciErr (* getListItemAddress) (void *_pvCtx,int *_piAddress,int _iItemNum,int **_piItemAddress);
extern SciErr (* getListInList) (void *_pvCtx,int *_piParent,int _iItemPos,int **_piAddress);
extern SciErr (* getTListInList) (void *_pvCtx,int *_piParent,int _iItemPos,int **_piAddress);
extern SciErr (* getMListInList) (void *_pvCtx,int *_piParent,int _iItemPos,int **_piAddress);
extern SciErr (* getListInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int **_piAddress);
extern SciErr (* getTListInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int **_piAddress);
extern SciErr (* getMListInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int **_piAddress);
extern SciErr (* createList) (void *_pvCtx,int _iVar,int _iNbItem,int **_piAddress);
extern SciErr (* createTList) (void *_pvCtx,int _iVar,int _iNbItem,int **_piAddress);
extern SciErr (* createMList) (void *_pvCtx,int _iVar,int _iNbItem,int **_piAddress);
extern SciErr (* createNamedList) (void *_pvCtx,char const *_pstName,int _iNbItem,int **_piAddress);
extern SciErr (* createNamedTList) (void *_pvCtx,char const *_pstName,int _iNbItem,int **_piAddress);
extern SciErr (* createNamedMList) (void *_pvCtx,char const *_pstName,int _iNbItem,int **_piAddress);
extern SciErr (* readNamedList) (void *_pvCtx,char const *_pstName,int *_piNbItem,int **_piAddress);
extern SciErr (* readNamedTList) (void *_pvCtx,char const *_pstName,int *_piNbItem,int **_piAddress);
extern SciErr (* readNamedMList) (void *_pvCtx,char const *_pstName,int *_piNbItem,int **_piAddress);
extern SciErr (* createListInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iNbItem,int **_piAddress);
extern SciErr (* createTListInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iNbItem,int **_piAddress);
extern SciErr (* createMListInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iNbItem,int **_piAddress);
extern SciErr (* createListInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iNbItem,int **_piAddress);
extern SciErr (* createTListInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iNbItem,int **_piAddress);
extern SciErr (* createMListInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iNbItem,int **_piAddress);
extern SciErr (* createVoidInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos);
extern SciErr (* createUndefinedInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos);
extern SciErr (* getMatrixOfDoubleInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,double **_pdblReal);
extern SciErr (* getComplexMatrixOfDoubleInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,double **_pdblReal,double **_pdblImg);
extern SciErr (* allocMatrixOfDoubleInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,double **_pdblReal);
extern SciErr (* allocComplexMatrixOfDoubleInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,double **_pdblReal,double **_pdblImg);
extern SciErr (* createMatrixOfDoubleInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,double const *_pdblReal);
extern SciErr (* createComplexMatrixOfDoubleInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* createComplexZMatrixOfDoubleInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,doublecomplex const *_pdblData);
extern SciErr (* createMatrixOfDoubleInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,double const *_pdblReal);
extern SciErr (* createComplexMatrixOfDoubleInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* createComplexZMatrixOfDoubleInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,doublecomplex const *_pdblData);
extern SciErr (* readMatrixOfDoubleInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,double *_pdblReal);
extern SciErr (* readComplexMatrixOfDoubleInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,double *_pdblReal,double *_pdblImg);
extern SciErr (* getMatrixOfStringInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piLength,char **_pstStrings);
extern SciErr (* createMatrixOfStringInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,char const *const *_pstStrings);
extern SciErr (* createMatrixOfStringInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,char const *const *_pstStrings);
extern SciErr (* readMatrixOfStringInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piLength,char **_pstStrings);
extern SciErr (* getMatrixOfBooleanInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int **_piBool);
extern SciErr (* allocMatrixOfBooleanInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int **_piBool);
extern SciErr (* createMatrixOfBooleanInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int const *_piBool);
extern SciErr (* createMatrixOfBooleanInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,int const *_piBool);
extern SciErr (* readMatrixOfBooleanInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piBool);
extern SciErr (* getMatrixOfPolyInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal);
extern SciErr (* getComplexMatrixOfPolyInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern SciErr (* createMatrixOfPolyInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal);
extern SciErr (* createComplexMatrixOfPolyInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal,double const *const *_pdblImg);
extern SciErr (* readMatrixOfPolyInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal);
extern SciErr (* readComplexMatrixOfPolyInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbCoef,double **_pdblReal,double **_pdblImg);
extern SciErr (* createMatrixOfPolyInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal);
extern SciErr (* createComplexMatrixOfPolyInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,char *_pstVarName,int _iRows,int _iCols,int const *_piNbCoef,double const *const *_pdblReal,double const *const *_pdblImg);
extern SciErr (* createMatrixOfInteger8InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,char const *_pcData);
extern SciErr (* createMatrixOfUnsignedInteger8InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned char const *_pucData);
extern SciErr (* createMatrixOfInteger16InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,short const *_psData);
extern SciErr (* createMatrixOfUnsignedInteger16InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned short const *_pusData);
extern SciErr (* createMatrixOfInteger32InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int const *_piData);
extern SciErr (* createMatrixOfUnsignedInteger32InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned int const *_puiData);
extern SciErr (* allocMatrixOfInteger8InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,char **_pcData);
extern SciErr (* allocMatrixOfUnsignedInteger8InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned char **_pucData);
extern SciErr (* allocMatrixOfInteger16InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,short **_psData);
extern SciErr (* allocMatrixOfUnsignedInteger16InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned short **_pusData);
extern SciErr (* allocMatrixOfInteger32InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int **_piData);
extern SciErr (* allocMatrixOfUnsignedInteger32InList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned int **_puiData);
extern SciErr (* getMatrixOfInteger8InList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,char **_pcData);
extern SciErr (* getMatrixOfUnsignedInteger8InList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,unsigned char **_pucData);
extern SciErr (* getMatrixOfInteger16InList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,short **_psData);
extern SciErr (* getMatrixOfUnsignedInteger16InList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,unsigned short **_pusData);
extern SciErr (* getMatrixOfInteger32InList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int **_piData);
extern SciErr (* getMatrixOfUnsignedInteger32InList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,unsigned int **_puiData);
extern SciErr (* createMatrixOfInteger8InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,char const *_pcData);
extern SciErr (* createMatrixOfUnsignedInteger8InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned char const *_pucData);
extern SciErr (* createMatrixOfInteger16InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,short const *_psData);
extern SciErr (* createMatrixOfUnsignedInteger16InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned short const *_pusData);
extern SciErr (* createMatrixOfInteger32InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,int const *_piData);
extern SciErr (* createMatrixOfUnsignedInteger32InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,unsigned int const *_puiData);
extern SciErr (* readMatrixOfIntger8InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,char *_pcData);
extern SciErr (* readMatrixOfUnsignedInteger8InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,unsigned char *_pucData);
extern SciErr (* readMatrixOfIntger16InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,short *_psData);
extern SciErr (* readMatrixOfUnsignedInteger16InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,unsigned short *_pusData);
extern SciErr (* readMatrixOfIntger32InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piData);
extern SciErr (* readMatrixOfUnsignedInteger32InNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,unsigned int *_puiData);
extern SciErr (* createSparseMatrixInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal);
extern SciErr (* createComplexSparseMatrixInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* createSparseMatrixInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal);
extern SciErr (* createComplexSparseMatrixInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos,double const *_pdblReal,double const *_pdblImg);
extern SciErr (* getSparseMatrixInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal);
extern SciErr (* getComplexSparseMatrixInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos,double **_pdblReal,double **_pdblImg);
extern SciErr (* readSparseMatrixInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbItem,int *_piNbItemRow,int *_piColPos,double *_pdblReal);
extern SciErr (* readComplexSparseMatrixInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbItem,int *_piNbItemRow,int *_piColPos,double *_pdblReal,double *_pdblImg);
extern SciErr (* createBooleanSparseMatrixInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos);
extern SciErr (* createBooleanSparseMatrixInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int _iRows,int _iCols,int _iNbItem,int const *_piNbItemRow,int const *_piColPos);
extern SciErr (* getBooleanSparseMatrixInList) (void *_pvCtx,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbItem,int **_piNbItemRow,int **_piColPos);
extern SciErr (* readBooleanSparseMatrixInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,int *_piRows,int *_piCols,int *_piNbItem,int *_piNbItemRow,int *_piColPos);
extern SciErr (* getPointerInList) (void *_pvCtx,int *_piParent,int _iItemPos,void **_pvPtr);
extern SciErr (* createPointerInList) (void *_pvCtx,int _iVar,int *_piParent,int _iItemPos,void *_pvPtr);
extern SciErr (* readPointerInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,void **_pvPtr);
extern SciErr (* createPointerInNamedList) (void *_pvCtx,char const *_pstName,int *_piParent,int _iItemPos,void *_pvPtr);
extern int (* isListType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedListType) (void *_pvCtx,char const *_pstName);
extern int (* isTListType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedTListType) (void *_pvCtx,char const *_pstName);
extern int (* isMListType) (void *_pvCtx,int *_piAddress);
extern int (* isNamedMListType) (void *_pvCtx,char const *_pstName);
extern int (* addStackSizeError) (SciErr *_psciErr,char *_pstCaller,int iNeeded);
extern int (* addErrorMessage) (SciErr *_psciErr,int _iErr,char const *_pstMsg,...);
extern int (* printError) (SciErr *_psciErr,int _iLastMsg);
extern char (* getErrorMessage) (SciErr _sciErr);
extern BOOL (* TerminateScilab) (char *ScilabQuit);
extern int (* SendScilabJob) (char *job);