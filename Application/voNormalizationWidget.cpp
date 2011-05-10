// Qt includes
#include <QGroupBox>
#include <QHash>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QSignalMapper>
#include <QStackedLayout>
#include <QString>

// Visomics includes
#include "voNormalizationWidget.h"

class voNormalizationWidgetPrivate
{
public:
  voNormalizationWidgetPrivate();

  void setupUi(voNormalizationWidget * widget);

  void registerNormalizationWidget(const QString& methodName,
                                   QWidget * normalizationDetailsWidget = 0);

  QHBoxLayout*        NormalizationMethodLayout;
  QSignalMapper       RadioButtonMapper;
  QString             SelectedMethod;
  QStackedLayout*     NormalizationDetailsStackedLayout;
  QHash<QString, int> MethodToStackedIndexMap;
};

// --------------------------------------------------------------------------
// voNormalizationWidgetPrivate methods

// --------------------------------------------------------------------------
voNormalizationWidgetPrivate::voNormalizationWidgetPrivate()
{
  this->NormalizationMethodLayout = 0;
  this->NormalizationDetailsStackedLayout = 0;
}

// --------------------------------------------------------------------------
void voNormalizationWidgetPrivate::setupUi(voNormalizationWidget * widget)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(widget);
  QGroupBox * normalizationMethodGroupBox = new QGroupBox();
  mainLayout->addWidget(normalizationMethodGroupBox);

  this->NormalizationMethodLayout = new QHBoxLayout(normalizationMethodGroupBox);


  QObject::connect(&this->RadioButtonMapper, SIGNAL(mapped(const QString&)),
                   widget, SLOT(selectNormalizationMethod(const QString&)));

  this->NormalizationDetailsStackedLayout = new QStackedLayout;
  mainLayout->addLayout(this->NormalizationDetailsStackedLayout);
}

// --------------------------------------------------------------------------
void voNormalizationWidgetPrivate::registerNormalizationWidget(
  const QString& methodName, QWidget * normalizationDetailsWidget)
{
  Q_ASSERT(!methodName.isEmpty());
  Q_ASSERT(!this->MethodToStackedIndexMap.contains(methodName));

  QRadioButton * button = new QRadioButton();
  button->setObjectName(methodName);
  button->setText(methodName);
  this->NormalizationMethodLayout->addWidget(button);

  this->RadioButtonMapper.setMapping(button, methodName);

  QObject::connect(button, SIGNAL(clicked()), &this->RadioButtonMapper, SLOT(map()));

  if (normalizationDetailsWidget)
    {
    int index = this->NormalizationDetailsStackedLayout->addWidget(normalizationDetailsWidget);
    this->MethodToStackedIndexMap[methodName] = index;
    }
}

// --------------------------------------------------------------------------
// voNormalizationWidget methods
#include <QLabel>
// --------------------------------------------------------------------------
voNormalizationWidget::voNormalizationWidget(QWidget* newParent) :
  Superclass(newParent), d_ptr(new voNormalizationWidgetPrivate())
{
  Q_D(voNormalizationWidget);
  d->setupUi(this);

  d->registerNormalizationWidget("No");
  d->registerNormalizationWidget("Log2");

  // Set default
  QRadioButton * button = this->findChild<QRadioButton*>("No");
  Q_ASSERT(button);
  button->setChecked(true);
}

// --------------------------------------------------------------------------
voNormalizationWidget::~voNormalizationWidget()
{
}

// --------------------------------------------------------------------------
void voNormalizationWidget::selectNormalizationMethod(const QString& methodName)
{
  Q_D(voNormalizationWidget);
  d->SelectedMethod = methodName;
  // Update stack layout
  d->NormalizationDetailsStackedLayout->setCurrentIndex(d->MethodToStackedIndexMap.value(methodName));
  emit this->normalizationMethodSelected(methodName);
}

// --------------------------------------------------------------------------
QString voNormalizationWidget::selectedNormalizationMethod()const
{
  Q_D(const voNormalizationWidget);
  return d->SelectedMethod;
}
