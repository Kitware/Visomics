
// Qt includes
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

// QtPropertyBrowser includes
#include <QtGroupBoxPropertyBrowser>
#include <QtVariantEditorFactory>

// Visomics includes
#include "voAnalysis.h"
#include "voAnalysisParameterDialog.h"

class voAnalysisParameterDialogPrivate
{
  Q_DECLARE_PUBLIC(voAnalysisParameterDialog);

protected:
  voAnalysisParameterDialog* const q_ptr;

public:
  voAnalysisParameterDialogPrivate(voAnalysisParameterDialog& object);
  void init();

  QLabel* AnalysisDescriptionLabel;
  QtGroupBoxPropertyBrowser * AnalysisParameterEditor;
};

// --------------------------------------------------------------------------
// voAnalysisParameterDialogPrivate methods

// --------------------------------------------------------------------------
voAnalysisParameterDialogPrivate::voAnalysisParameterDialogPrivate(voAnalysisParameterDialog& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void voAnalysisParameterDialogPrivate::init()
{
  Q_Q(voAnalysisParameterDialog);

  q->setWindowTitle("Run analysis - Parameters");

  QVBoxLayout * verticalLayout = new QVBoxLayout();
  q->setLayout(verticalLayout);

  this->AnalysisDescriptionLabel = new QLabel();
  this->AnalysisDescriptionLabel->setWordWrap(true);
  verticalLayout->addWidget(this->AnalysisDescriptionLabel);

  this->AnalysisParameterEditor = new QtGroupBoxPropertyBrowser(q);
  verticalLayout->addWidget(this->AnalysisParameterEditor);

  QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  verticalLayout->addWidget(buttonBox);

  buttonBox->button(QDialogButtonBox::Ok)->setText("Run Analysis");

  QObject::connect(buttonBox, SIGNAL(rejected()), q, SLOT(reject()));
  QObject::connect(buttonBox, SIGNAL(accepted()), q, SLOT(accept()));
}

// --------------------------------------------------------------------------
// voAnalysisParameterDialog methods

// --------------------------------------------------------------------------
voAnalysisParameterDialog::voAnalysisParameterDialog(voAnalysis * analysis, QWidget* newParent) :
  Superclass(newParent), d_ptr(new voAnalysisParameterDialogPrivate(*this))
{
  Q_ASSERT(analysis);

  Q_D(voAnalysisParameterDialog);
  d->init();

  d->AnalysisDescriptionLabel->setText(analysis->parameterDescription());

  QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);

  d->AnalysisParameterEditor->clear();
  d->AnalysisParameterEditor->setFactoryForManager(analysis->propertyManager(), variantFactory);

  foreach(QtProperty* prop, analysis->topLevelParameterGroups())
    {
    d->AnalysisParameterEditor->addProperty(prop);
    }
}

// --------------------------------------------------------------------------
voAnalysisParameterDialog::~voAnalysisParameterDialog()
{
}

