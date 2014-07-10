#include "modelpanel.h"
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDesktopWidget>

ModelPanel::ModelPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware):
  QWidget(parent),
  model(model),
  generalSettings(generalSettings),
  firmware(firmware),
  lock(false)
{
}

ModelPanel::~ModelPanel()
{
}

void ModelPanel::update()
{
}

bool ModelPanel::twocolumns(int numelements, int widthlimit)
{
  QDesktopWidget widget;
  QRect mainScreenSize = widget.screenGeometry(widget.primaryScreen());
  if (numelements>16 && numelements<33) {
    return (mainScreenSize.width() > widthlimit)&(mainScreenSize.height() > 700);
  } else {
    return false;
  }
}


void ModelPanel::addLabel(QGridLayout * gridLayout, QString text, int col, bool minimize)
{
  QLabel *label = new QLabel(this);
  label->setFrameShape(QFrame::Panel);
  label->setFrameShadow(QFrame::Raised);
  label->setMidLineWidth(0);
  label->setAlignment(Qt::AlignCenter);
  label->setMargin(5);
  label->setText(text);
  if (!minimize)
    label->setMinimumWidth(60);
  else 
    label->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  gridLayout->addWidget(label, 0, col, 1, 1);
}

void ModelPanel::addEmptyLabel(QGridLayout * gridLayout, int col)
{
  QLabel *label = new QLabel(this);
  label->setText("");
  gridLayout->addWidget(label, 0, col, 1, 1);
}

void ModelPanel::addHSpring(QGridLayout * gridLayout, int col, int row)
{
    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    gridLayout->addItem(spacer, row, col);
}

void ModelPanel::addVSpring(QGridLayout * gridLayout, int col, int row)
{
    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
    gridLayout->addItem(spacer, row, col);
}

void ModelPanel::addDoubleSpring(QGridLayout * gridLayout, int col, int row)
{
    QSpacerItem * spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    gridLayout->addItem(spacer, row, col);
}

bool ModelPanel::eventFilter(QObject *object, QEvent * event)
{
  QWidget * widget = qobject_cast<QWidget*>(object);
  if (widget) {
    if (event->type() == QEvent::Wheel) {
      if (widget->focusPolicy() == Qt::WheelFocus) {
        event->accept();
        return false;
      }
      else {
        event->ignore();
        return true;
      }
    }
    else if (event->type() == QEvent::FocusIn) {
      widget->setFocusPolicy(Qt::WheelFocus);
    }
    else if (event->type() == QEvent::FocusOut) {
      widget->setFocusPolicy(Qt::StrongFocus);
    }
  }
  return QWidget::eventFilter(object, event);
}

void ModelPanel::disableMouseScrolling()
{
  Q_FOREACH(QComboBox * cb, findChildren<QComboBox*>()) {
    cb->installEventFilter(this);
    cb->setFocusPolicy(Qt::StrongFocus);
  }

  Q_FOREACH(QAbstractSpinBox * sb, findChildren<QAbstractSpinBox*>()) {
    sb->installEventFilter(this);
    sb->setFocusPolicy(Qt::StrongFocus);
  }

  Q_FOREACH(QSlider * slider, findChildren<QSlider*>()) {
    slider->installEventFilter(this);
    slider->setFocusPolicy(Qt::StrongFocus);
  }
}
