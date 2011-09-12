/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/
#ifndef __voUtils_h
#define __voUtils_h

// VTK includes
#include <vtkType.h>

class vtkAbstractArray;
class vtkStringArray;
class vtkTable;
class vtkTree;
class vtkDataSetAttributes;
template <class T> class QList;
class QScriptEngine;
class QScriptValue;
class QString;
class vtkArray;
template <class T> class vtkSmartPointer;

namespace voUtils 
{

enum TransposeOption
  {
  WithoutHeaders = 0x0,
  FirstColumnIntoColumnNames = 0x1,
  ColumnNamesIntoFirstColumn = 0x2,
  Headers = 0x3,
  };

enum FlipOption
  {
  FlipHorizontalAxis = 0x1,
  FlipVerticalAxis = 0x2,
  };

bool transposeTable(vtkTable* srcTable, vtkTable* destTable, const TransposeOption& transposeOption = WithoutHeaders);

bool transposeTable(vtkTable* table, const TransposeOption& transposeOption = WithoutHeaders);

bool flipTable(vtkTable* srcTable, vtkTable* destTable, const FlipOption& flipOption, int horizontalOffset = 0, int verticalOffset = 0);

bool flipTable(vtkTable* table, const FlipOption& flipOption, int horizontalOffset = 0, int verticalOffset = 0);

bool insertColumnIntoTable(vtkTable * table, int position, vtkAbstractArray * column);

vtkStringArray* tableColumnNames(vtkTable * table, int offset = 0);

void setTableColumnNames(vtkTable * table, vtkStringArray * columnNames);

bool parseRangeString(const QString& rangeString, QList<int>& rangeList, bool alpha);

QString counterIntToAlpha(int intVal);

int counterAlphaToInt(const QString& alphaVal);

bool tableToArray(vtkTable* srcTable, vtkSmartPointer<vtkArray>& destArray);

bool tableToArray(vtkTable* srcTable, vtkSmartPointer<vtkArray>& destArray, const QList<int>& columnList);

void arrayToTable(vtkArray* srcArray, vtkTable* destTable);

QList<int> range(int start, int stop, int step = 1);

QString stringify(QScriptEngine* scriptEngine, const QScriptValue& scriptValue);

QString stringify(const QString& name, vtkTable * table, const QList<vtkIdType>& columnIdsToSkip);

QString stringify(const QString& name, vtkTree * tree);
}

#endif

