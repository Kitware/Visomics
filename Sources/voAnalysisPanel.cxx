
// Visomics includes
#include "ui_voAnalysisPanel.h"
#include "voAnalysisPanel.h"
#include "voAnalysis.h"
#include "voView.h"

// STD includes
#include <iostream>

// --------------------------------------------------------------------------
voAnalysisPanel::voAnalysisPanel()
{
  this->ui = new Ui_voAnalysisPanel;
  this->ui->setupUi(this);
}

// --------------------------------------------------------------------------
voAnalysisPanel::~voAnalysisPanel()
{
}

// --------------------------------------------------------------------------
void voAnalysisPanel::SetAnalysis( voAnalysis * a )
{
  //First, clear if there are any elements in the comboBox
  this->ui->viewBox->clear();

  //Add the new views
  QStringList views = a->views();
  this->ui->viewBox->addItems(views);

  //remove already existing widgets
  while( this->ui->content->count() > 0 )
    {
    this->ui->content->removeWidget( this->ui->content->widget( 0 ));
    delete this->ui->content->widget( 0 );
    }

  //Add the new widgets 
  for (int i = 0; i < views.size(); ++i)
    {
    this->ui->content->addWidget(a->view(views[i])->widget());
    }
  
  a->updateViews();

  this->Analysis = a;

  this->connect(this->ui->viewBox, SIGNAL(currentIndexChanged(int)), this->ui->content, SLOT(setCurrentIndex(int)));
}

// --------------------------------------------------------------------------
voView* voAnalysisPanel::currentView()
{
  std::cout << "currentView: " << this->ui->viewBox->currentText().toStdString() << std::endl;
  return this->Analysis->view(this->ui->viewBox->currentText());
}
