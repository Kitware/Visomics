/*=========================================================================

  Program: Arbor

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

// Arbor includes
#include "voAboutDialog_Arbor.h"
#include "voConfigure.h"

// VTK includes
#include <vtkVersionMacros.h>

// --------------------------------------------------------------------------
class voAboutDialogPrivate
{
  Q_DECLARE_PUBLIC(voAboutDialog);
protected:
  voAboutDialog* const q_ptr;

public:
  voAboutDialogPrivate(voAboutDialog& object);
  void init();
};

// --------------------------------------------------------------------------
// voAboutDialogPrivate methods

// --------------------------------------------------------------------------
voAboutDialogPrivate::voAboutDialogPrivate(voAboutDialog& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void voAboutDialogPrivate::init()
{
  Q_Q(voAboutDialog);

  QString translatedTextAboutArborCaption;
  translatedTextAboutArborCaption =
      voAboutDialog::tr("<h3>Arbor %1</h3>").arg(Visomics_VERSION);

  QString translatedTextAboutArborText;
  translatedTextAboutArborText = voAboutDialog::tr(
        "<p>Arbor is a platform for visualization and analysis of Phylogenetic data.</p>"
        "<p>Based on <a href=\"http://www.r-project.org/\">GnuR</a>, "
        "<a href=\"http://www.vtk.org\">VTK %1</a>,"
        " <a href=\"http://www.commontk.org\">CTK</a>, "
        "<a href=\"http://mbostock.github.com/d3/\">d3.js 1.27.1</a>"
        " and <a href=\"http://qt.nokia.com/\">Qt %2</a></p>"
        "<p>Arbor is licensed under the <a href=\"http://www.apache.org/licenses/LICENSE-2.0\">Apache License version 2.0</a>.</p>"
        "<p>Copyright (C) 2012 <a href=\"http://www.kitware.com\">Kitware, Inc.</a></p>"
        "<p>Arbor is a <a href=\"http://www.kitware.com\">Kitware, Inc.</a> product. "
        "See <a href=\"http://visomics.org/\">arborworkflows.github.com</a> for more information.</p>"
        ).arg(VTK_VERSION).arg(QT_VERSION_STR);
  q->setText(translatedTextAboutArborCaption);
  q->setInformativeText(translatedTextAboutArborText);
  q->setIconPixmap(QPixmap(":/Icons/ArborIcon-128.png"));
  q->setWindowTitle(voAboutDialog::tr("About Arbor"));
}

// --------------------------------------------------------------------------
// voAboutDialog methods

// --------------------------------------------------------------------------
voAboutDialog::voAboutDialog(QWidget* newParent):Superclass(newParent),
  d_ptr(new voAboutDialogPrivate(*this))
{
  Q_D(voAboutDialog);
  d->init();
}

// --------------------------------------------------------------------------
voAboutDialog::~voAboutDialog()
{
}
