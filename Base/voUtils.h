#ifndef __voUtils_h
#define __voUtils_h

class vtkAbstractArray;
class vtkStringArray;
class vtkTable;
template <class T> class QList;
class QString;
class vtkArray;
template <class T> class vtkSmartPointer;

namespace voUtils 
{

bool transposeTable(vtkTable* srcTable, vtkTable* destTable);

bool transposeTable(vtkTable* table);

bool insertColumnIntoTable(vtkTable * table, int position, vtkAbstractArray * column);

void setTableColumnNames(vtkTable * table, vtkStringArray * columnNames);

bool parseRangeString(const QString& rangeString, QList<int>& rangeList, bool alpha);

QString counterIntToAlpha(int intVal);

int counterAlphaToInt(const QString& alphaVal);

bool tableToArray(vtkTable* srcTable, vtkSmartPointer<vtkArray>& destArray, const QList<int>& columnList);

void arrayToTable(vtkArray* srcArray, vtkTable* destTable);
}

#endif

