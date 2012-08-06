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

// Qt includes
#include <QDebug>
#include <QDialogButtonBox>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

// QtPropertyBrowser includes
#include <QtTreePropertyBrowser>
#include <QtVariantEditorFactory>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisParameterEditorWidget.h"

class voAnalysisParameterEditorWidgetPrivate
{
  Q_DECLARE_PUBLIC(voAnalysisParameterEditorWidget);

  void setButtonsEnabled(bool value);

  void setLocalPropertyManagerConnected(bool value);

  static bool comparePropertySet(QSet<QtProperty*> set1, QSet<QtProperty*> set2);

  void parameterValues(QHash<QString, QVariant>& parameters);

protected:
  voAnalysisParameterEditorWidget* const q_ptr;

public:
  voAnalysisParameterEditorWidgetPrivate(voAnalysisParameterEditorWidget& object);
  void init();

  QtTreePropertyBrowser * AnalysisParameterEditor;
  QPushButton*            ApplyButton;
  QPushButton*            ResetButton;
  QPushButton*            OkButton;
  QPointer<voAnalysis>    Analysis;

  // The QtTreePropertyBrowser is associated with a "local" property manager
  // containing a copy of the property associated with the analysis.
  // Doing so allows to reset using the "unchanged" value still associated with the analysis.
  QtVariantPropertyManager LocalPropertyManager;
};

// --------------------------------------------------------------------------
// voAnalysisParameterEditorWidgetPrivate methods

// --------------------------------------------------------------------------
voAnalysisParameterEditorWidgetPrivate::voAnalysisParameterEditorWidgetPrivate(voAnalysisParameterEditorWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidgetPrivate::init()
{
  Q_Q(voAnalysisParameterEditorWidget);

  q->setWindowTitle("Run analysis - Parameters");

  QVBoxLayout * verticalLayout = new QVBoxLayout();
  verticalLayout->setMargin(0);
  q->setLayout(verticalLayout);

  // Editor
  this->AnalysisParameterEditor = new QtTreePropertyBrowser(q);
  this->AnalysisParameterEditor->setPropertiesWithoutValueMarked(true);
  this->AnalysisParameterEditor->setRootIsDecorated(true);
  this->AnalysisParameterEditor->setHeaderVisible(false);

  verticalLayout->addWidget(this->AnalysisParameterEditor);

  // ButtonBox
  QDialogButtonBox * dialogButton =
      new QDialogButtonBox(QDialogButtonBox::Reset | QDialogButtonBox::Ok | QDialogButtonBox::Apply);
  dialogButton->setCenterButtons(true);
  verticalLayout->addWidget(dialogButton);

  this->ApplyButton = dialogButton->button(QDialogButtonBox::Apply);
  this->ResetButton = dialogButton->button(QDialogButtonBox::Reset);
  this->OkButton = dialogButton->button(QDialogButtonBox::Ok);
  this->ApplyButton->setText("Update");
  this->OkButton->setText("Clone");

  QObject::connect(this->ApplyButton, SIGNAL(clicked()), q, SLOT(updateAnalysis()));
  QObject::connect(this->ResetButton, SIGNAL(clicked()), q, SLOT(reset()));
  QObject::connect(this->OkButton, SIGNAL(clicked()), q, SLOT(cloneAnalysis()));

  this->setButtonsEnabled(false);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidgetPrivate::setButtonsEnabled(bool value)
{
  this->ApplyButton->setEnabled(value);
  this->ResetButton->setEnabled(value);
  this->OkButton->setEnabled(value);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidgetPrivate::setLocalPropertyManagerConnected(bool value)
{
  Q_Q(voAnalysisParameterEditorWidget);
  if (value) // Connect
    {
    QObject::connect(&this->LocalPropertyManager, SIGNAL(valueChanged(QtProperty*,QVariant)),
        q, SLOT(onLocalValueChanged(QtProperty*,QVariant)));
    }
  else // Disconnect
    {
    QObject::disconnect(&this->LocalPropertyManager, SIGNAL(valueChanged(QtProperty*,QVariant)),
                        q, SLOT(onLocalValueChanged(QtProperty*,QVariant)));
    }
}

// --------------------------------------------------------------------------
bool voAnalysisParameterEditorWidgetPrivate::comparePropertySet(QSet<QtProperty*> set1, QSet<QtProperty*> set2)
{
  if (set1.count() != set2.count())
    {
    return false;
    }
  foreach(QtProperty* propSet1, set1)
    {
    QtProperty* matchedPropSet2 = 0;
    foreach(QtProperty* propSet2, set2)
      {
      if (propSet1->compare(propSet2))
        {
        matchedPropSet2 = propSet2;
        break;
        }
      }
    if (!matchedPropSet2)
      {
      return false;
      }
    set2.remove(matchedPropSet2);
    }
  return (set2.count() == 0);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidgetPrivate::parameterValues(QHash<QString, QVariant>& parameters)
{
  foreach(QtProperty * localProp, this->LocalPropertyManager.properties())
    {
    QtVariantProperty * localVariantProp = dynamic_cast<QtVariantProperty*>(localProp);
    Q_ASSERT(localVariantProp);
    parameters[localVariantProp->propertyId()] = localVariantProp->value();
    }
}

// --------------------------------------------------------------------------
// voAnalysisParameterEditorWidget methods

// --------------------------------------------------------------------------
voAnalysisParameterEditorWidget::voAnalysisParameterEditorWidget(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voAnalysisParameterEditorWidgetPrivate(*this))
{
  Q_D(voAnalysisParameterEditorWidget);
  d->init();
}

// --------------------------------------------------------------------------
voAnalysisParameterEditorWidget::~voAnalysisParameterEditorWidget()
{
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidget::setAnalysis(voAnalysis* newAnalysis)
{
  Q_D(voAnalysisParameterEditorWidget);

  if (!newAnalysis)
    {
    return;
    }

  d->setLocalPropertyManagerConnected(false);

  // Copy analysis property
  d->LocalPropertyManager.setProperties(newAnalysis->propertyManager()->properties());

  // Setup editor
  d->AnalysisParameterEditor->clear();
  d->AnalysisParameterEditor->setFactoryForManager(&d->LocalPropertyManager, new QtVariantEditorFactory());

  // Add top level property to editor
  foreach(QtProperty* prop, d->LocalPropertyManager.properties())
    {
    if (!prop->isSubProperty())
      {
      d->AnalysisParameterEditor->addProperty(prop);
      }
    }

  d->setLocalPropertyManagerConnected(true);

  d->setButtonsEnabled(false);

  d->Analysis = QPointer<voAnalysis>(newAnalysis);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidget::onLocalValueChanged(QtProperty *localProp, const QVariant &localValue)
{
  Q_D(voAnalysisParameterEditorWidget);
  Q_UNUSED(localProp);
  Q_UNUSED(localValue);
  bool equal = d->comparePropertySet(
        d->LocalPropertyManager.properties(), d->Analysis->propertyManager()->properties());
  d->setButtonsEnabled(!equal);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidget::reset()
{
  Q_D(voAnalysisParameterEditorWidget);

  d->setLocalPropertyManagerConnected(false);

  // Loop over analysis properties
  foreach(QtProperty * prop, d->Analysis->propertyManager()->properties())
    {
    QtVariantProperty * variantProp = dynamic_cast<QtVariantProperty*>(prop);
    Q_ASSERT(variantProp);
    // Retrieve associated local property
    QtVariantProperty * localVariantProp = dynamic_cast<QtVariantProperty*>(
          d->LocalPropertyManager.qtProperty(prop->propertyId()));
    Q_ASSERT(localVariantProp);
    // Reset local property using analysis one
    localVariantProp->setValue(variantProp->value()); // Note that this emit valueChanged signal
    }

  d->AnalysisParameterEditor->setCurrentItem(0);
  d->setButtonsEnabled(false);
  d->setLocalPropertyManagerConnected(true);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidget::updateAnalysis()
{
  Q_D(voAnalysisParameterEditorWidget);
  d->setButtonsEnabled(false);

  QHash<QString, QVariant> parameters;
  d->parameterValues(parameters);
  d->AnalysisParameterEditor->setCurrentItem(0);

  emit updateAnalysisRequested(d->Analysis, parameters);
}

// --------------------------------------------------------------------------
void voAnalysisParameterEditorWidget::cloneAnalysis()
{
  Q_D(voAnalysisParameterEditorWidget);
  d->setButtonsEnabled(false);

  QHash<QString, QVariant> parameters;
  d->parameterValues(parameters);
  this->reset();
  d->AnalysisParameterEditor->setCurrentItem(0);

  emit runAnalysisRequested(d->Analysis->objectName(), parameters);
}
