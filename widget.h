#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "CameraGet.h"
#include <QImage>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_B_OpenCam_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
