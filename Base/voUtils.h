#ifndef __voUtils_h
#define __voUtils_h

class vtkAbstractArray;
class vtkTable;

namespace voUtils 
{

bool transposeTable(vtkTable* srcTable, vtkTable* destTable);

bool transposeTable(vtkTable* table);

bool insertColumnIntoTable(vtkTable * table, int position, vtkAbstractArray * column);

}

#endif

