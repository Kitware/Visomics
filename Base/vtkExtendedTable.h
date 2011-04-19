#ifndef __vtkExtendedTable_h
#define __vtkExtendedTable_h

// VTK includes
#include <vtkTable.h>

///
/// This class allows to store numerical tabular data and their associated metadata.
///
/// The diagram represented below shows the tabular data (D) and the associated 
/// row (RM) and column (CM) metadata.
///
///                           ---------------------------------------------
///                           |   CM02   |   CM12   |   CM22   |   CM32   |
///                           ---------------------------------------------
///                           |   CM01   |   CM11   |   CM21   |   CM31   |
///                           ---------------------------------------------
///                           |   CM00   |   CM10   |   CM20   |   CM30   |
///                           ---------------------------------------------
///
///  -----------------------  ---------------------------------------------
///  |   RM00   |   RM01   |  |    D00   |    D01   |    D02   |    D03   |
///  -----------------------  --------------------------------------------
///  |   RM10   |   RM11   |  |    D10   |    D11   |    D12   |    D13   |
///  -----------------------  ---------------------------------------------
///  |   RM20   |   RM21   |  |    D20   |    D21   |    D22   |    D23   |
///  -----------------------  ---------------------------------------------
///  |   RM30   |   RM31   |  |    D30   |    D31   |    D32   |    D33   |
///  -----------------------  ---------------------------------------------
///
/// Data and associated metadata are all store within vtkTable. The number 
/// represented above corresponds to the <ROW><COLUMN> indices of the associated table.
///
/// Note that the columns of the ColumnMetaData (CM) table are represented above as rows.
///

class vtkAbstractArray;
class vtkStringArray;
class vtkTable;
class vtkVariantArray;

class vtkExtendedTable : public vtkTable
{
public:
  static vtkExtendedTable* New();
  vtkTypeMacro(vtkExtendedTable, vtkTable);
  void PrintSelf(ostream &os, vtkIndent indent);

  void Dump();

  vtkIdType GetTotalNumberOfRows();

  vtkIdType GetTotalNumberOfColumns();
  
  //
  // Data
  //

  void SetData(vtkTable* data);
  
  vtkTable* GetData();

  vtkTable* GetDataWithRowHeader();
  
  //
  // Column MetaData
  //

  bool HasColumnMetaData() const;

  void SetColumnMetaDataTable(vtkTable* columnMetaData);
  
  vtkIdType GetNumberOfColumnMetaDataTypes() const;
  
  vtkAbstractArray* GetColumnMetaData(vtkIdType id) const;

  vtkStringArray* GetColumnMetaDataAsString(vtkIdType id) const;
  
  vtkIdType GetColumnMetaDataTypeOfInterest() const;
  
  void SetColumnMetaDataTypeOfInterest(vtkIdType id);

  vtkAbstractArray* GetColumnMetaDataOfInterest() const;

  vtkStringArray* GetColumnMetaDataOfInterestAsString() const;
  
  //
  // Row MetaData
  //

  bool HasRowMetaData() const;

  void SetRowMetaDataTable(vtkTable* rowMetaData);
  
  vtkIdType GetNumberOfRowMetaDataTypes()const;
  
  vtkAbstractArray* GetRowMetaData(vtkIdType id) const;

  vtkStringArray* GetRowMetaDataAsString(vtkIdType id) const;
  
  vtkIdType GetRowMetaDataTypeOfInterest() const;
  
  void SetRowMetaDataTypeOfInterest(vtkIdType id);

  vtkAbstractArray* GetRowMetaDataOfInterest() const;

  vtkStringArray* GetRowMetaDataOfInterestAsString() const;
  
  
protected:
  vtkExtendedTable();
  ~vtkExtendedTable();

private:
  vtkExtendedTable(const vtkExtendedTable&); // Not implemented
  void operator=(const vtkExtendedTable&); // Not implemented
  
  class vtkInternal;
  vtkInternal * Internal;
  
};

#endif
