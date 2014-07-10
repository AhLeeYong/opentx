#ifndef MODELPANEL_H
#define MODELPANEL_H

#include <QWidget>
#include <QGridLayout>
#include "eeprominterface.h"

class ModelPanel : public QWidget
{
    Q_OBJECT

  friend class QUnsignedAutoComboBox;

  public:
    ModelPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, FirmwareInterface * firmware);
    virtual ~ModelPanel();

  signals:
    void modified();

  public slots:
    virtual void update();

  protected:
    ModelData & model;
    GeneralSettings & generalSettings;
    FirmwareInterface * firmware;
    bool lock;
    void addLabel(QGridLayout * gridLayout, QString text, int col, bool mimimize=false);
    void addEmptyLabel(QGridLayout * gridLayout, int col);
    void addHSpring(QGridLayout *, int col, int row);
    void addVSpring(QGridLayout *, int col, int row);
    void addDoubleSpring(QGridLayout *, int col, int row);
    virtual bool eventFilter(QObject *obj, QEvent *event);
    void disableMouseScrolling();
    bool twocolumns(int numelements, int widthlimit);
};

#endif // CHANNELS_H
