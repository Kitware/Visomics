#ifndef __voUtils_h
#define __voUtils_h

class vtkTable;

namespace voUtils 
{

bool transposeTable(vtkTable* srcTable, vtkTable* destTable);

bool transposeTable(vtkTable* table);

}

#endif

