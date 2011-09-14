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
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <qmath.h>
#include <QLayout>
#include <QPixmap>
#include <QVariant>

// Visomics includes
#include "voStartupView.h"

// --------------------------------------------------------------------------
class voStartupViewPrivate
{
  Q_DECLARE_PUBLIC(voStartupView);
protected:
  voStartupView* const q_ptr;
public:
  voStartupViewPrivate(voStartupView& object);

  void init();

  void updateBackground();

  QGraphicsView* View;
};

// --------------------------------------------------------------------------
// voStartupViewPrivate methods

// --------------------------------------------------------------------------
voStartupViewPrivate::voStartupViewPrivate(voStartupView& object)
  :q_ptr(&object)
{
  this->View = 0;
}

// --------------------------------------------------------------------------
void voStartupViewPrivate::init()
{
  Q_Q(voStartupView);
  QGraphicsScene* scene = new QGraphicsScene(q);

  this->View = new QGraphicsView;
  this->View->setScene(scene);
  this->View->setCacheMode(QGraphicsView::CacheBackground);

  QGraphicsPixmapItem * pixmapItem = scene->addPixmap(QPixmap(":/Icons/VisomicsLogo.png"));
  pixmapItem->setTransformationMode(Qt::SmoothTransformation);
  pixmapItem->scale(0.6, 0.6);

  QVBoxLayout * verticalLayout = new QVBoxLayout(q);
  verticalLayout->setSpacing(0);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  verticalLayout->addWidget(this->View);
}

// --------------------------------------------------------------------------
void voStartupViewPrivate::updateBackground()
{
  Q_Q(voStartupView);

  qreal w = q->width();
  qreal h = q->height();
  QRadialGradient gradient(QPointF(w, h), qSqrt(w*w + h*h));

  QColor kitwareBlue("#1b4e9c");
  gradient.setColorAt(0, Qt::white);
  gradient.setColorAt(1, kitwareBlue.lighter(250));

  this->View->setBackgroundBrush(gradient);
}

// --------------------------------------------------------------------------
// voStartupView methods

// --------------------------------------------------------------------------
voStartupView::voStartupView(QWidget * newParent):
  Superclass(newParent), d_ptr(new voStartupViewPrivate(*this))
{
  Q_D(voStartupView);
  d->init();
}

// --------------------------------------------------------------------------
voStartupView::~voStartupView()
{
}

//-----------------------------------------------------------------------------
void voStartupView::resizeEvent(QResizeEvent* event)
{
  Q_D(voStartupView);
  d->updateBackground();
  this->Superclass::resizeEvent(event);
}
