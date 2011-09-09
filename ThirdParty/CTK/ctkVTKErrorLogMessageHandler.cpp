/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkVTKErrorLogMessageHandler.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkOutputWindow.h>

namespace {

// --------------------------------------------------------------------------
// ctkVTKOutputWindow

// --------------------------------------------------------------------------
class ctkVTKOutputWindow : public vtkOutputWindow
{
public:
  static ctkVTKOutputWindow *New();
  vtkTypeMacro(ctkVTKOutputWindow,vtkOutputWindow);
  void PrintSelf(ostream& os, vtkIndent indent);

  ctkVTKOutputWindow():MessageHandler(0){}
  ~ctkVTKOutputWindow(){}

  virtual void DisplayText(const char*);
  virtual void DisplayErrorText(const char*);
  virtual void DisplayWarningText(const char*);
  virtual void DisplayGenericWarningText(const char*);

  virtual void DisplayDebugText(const char*);

  ctkErrorLogAbstractMessageHandler * MessageHandler;
};

// --------------------------------------------------------------------------
// ctkVTKOutputWindow methods

// --------------------------------------------------------------------------
vtkStandardNewMacro(ctkVTKOutputWindow);

//----------------------------------------------------------------------------
void ctkVTKOutputWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void ctkVTKOutputWindow::DisplayText(const char* text)
{
  this->MessageHandler->errorLogModel()->addEntry(
        ctkErrorLogModel::Info, this->MessageHandler->handlerPrettyName(), text);
}

//----------------------------------------------------------------------------
void ctkVTKOutputWindow::DisplayErrorText(const char* text)
{
  this->MessageHandler->errorLogModel()->addEntry(
        ctkErrorLogModel::Error, this->MessageHandler->handlerPrettyName(), text);
}

//----------------------------------------------------------------------------
void ctkVTKOutputWindow::DisplayWarningText(const char* text)
{
  this->MessageHandler->errorLogModel()->addEntry(
        ctkErrorLogModel::Warning, this->MessageHandler->handlerPrettyName(), text);
}

//----------------------------------------------------------------------------
void ctkVTKOutputWindow::DisplayGenericWarningText(const char* text)
{
  this->DisplayWarningText(text);
}

//----------------------------------------------------------------------------
void ctkVTKOutputWindow::DisplayDebugText(const char* text)
{
  this->MessageHandler->errorLogModel()->addEntry(
        ctkErrorLogModel::Debug, this->MessageHandler->handlerPrettyName(), text);
}

} // End of anonymous namespace

// --------------------------------------------------------------------------
// ctkVTKErrorLogMessageHandlerPrivate

// --------------------------------------------------------------------------
class ctkVTKErrorLogMessageHandlerPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKErrorLogMessageHandler);
protected:
  ctkVTKErrorLogMessageHandler* const q_ptr;
public:
  ctkVTKErrorLogMessageHandlerPrivate(ctkVTKErrorLogMessageHandler& object);
  ~ctkVTKErrorLogMessageHandlerPrivate();

  vtkOutputWindow * SavedVTKOutputWindow;
  ctkVTKOutputWindow * CTKVTKOutputWindow;

};

// --------------------------------------------------------------------------
// ctkVTKErrorLogMessageHandlerPrivate methods

// --------------------------------------------------------------------------
ctkVTKErrorLogMessageHandlerPrivate::
ctkVTKErrorLogMessageHandlerPrivate(ctkVTKErrorLogMessageHandler& object) : q_ptr(&object)
{
  Q_Q(ctkVTKErrorLogMessageHandler);
  this->SavedVTKOutputWindow = 0;
  this->CTKVTKOutputWindow = ctkVTKOutputWindow::New();
  this->CTKVTKOutputWindow->MessageHandler = q;
}

// --------------------------------------------------------------------------
ctkVTKErrorLogMessageHandlerPrivate::~ctkVTKErrorLogMessageHandlerPrivate()
{
  if (this->SavedVTKOutputWindow)
    {
    this->SavedVTKOutputWindow->Delete();
    }
  this->CTKVTKOutputWindow->Delete();
}

// --------------------------------------------------------------------------
// ctkVTKErrorLogMessageHandler methods

// --------------------------------------------------------------------------
QString ctkVTKErrorLogMessageHandler::HandlerName = QLatin1String("VTK");

//----------------------------------------------------------------------------
ctkVTKErrorLogMessageHandler::ctkVTKErrorLogMessageHandler() :
  Superclass(), d_ptr(new ctkVTKErrorLogMessageHandlerPrivate(*this))
{
}

//----------------------------------------------------------------------------
ctkVTKErrorLogMessageHandler::~ctkVTKErrorLogMessageHandler()
{
}

//----------------------------------------------------------------------------
QString ctkVTKErrorLogMessageHandler::handlerName()const
{
  return ctkVTKErrorLogMessageHandler::HandlerName;
}

//----------------------------------------------------------------------------
void ctkVTKErrorLogMessageHandler::setEnabledInternal(bool value)
{
  Q_D(ctkVTKErrorLogMessageHandler);
  if (value)
    {
    d->SavedVTKOutputWindow = vtkOutputWindow::GetInstance();
    d->SavedVTKOutputWindow->Register(0);
    vtkOutputWindow::SetInstance(d->CTKVTKOutputWindow);
    }
  else
    {
    Q_ASSERT(d->SavedVTKOutputWindow);
    vtkOutputWindow::SetInstance(d->SavedVTKOutputWindow);
    d->SavedVTKOutputWindow->Delete();
    d->SavedVTKOutputWindow = 0;
    }
}

