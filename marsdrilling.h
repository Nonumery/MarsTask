#ifndef MARSDRILLING_H
#define MARSDRILLING_H

#include <QMainWindow>
#include <QString>
#include <QtWidgets>
#include "trajectory3d.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MarsDrilling; }
QT_END_NAMESPACE

class MarsDrilling : public QMainWindow
{
    Q_OBJECT
    Trajectory* trj = nullptr; // траектория
    Trajectory3D* scene = nullptr; // сцена для траектории
    QPalette pressed; // объект для изменения цвета нажатых кнопок
    QPalette unPressed; // объект для изменения цвета нажатых кнопок
    QString filename; // имя файла с данными о траектории

public:
    MarsDrilling(QWidget *parent = nullptr);
    ~MarsDrilling();

private:
    Ui::MarsDrilling *ui;
};
#endif // MARSDRILLING_H
