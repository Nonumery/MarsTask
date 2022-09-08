#include "marsdrilling.h"
#include "ui_marsdrilling.h"
#include <QString>
MarsDrilling::MarsDrilling(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MarsDrilling)
{
    ui->setupUi(this);
    pressed.setColor(QPalette::Button, QColor(Qt::blue));
    unPressed.setColor(QPalette::Button, QColor(Qt::darkYellow));
    ui->pushButton->setPalette(unPressed);
    ui->pushButton_2->setPalette(unPressed);
    ui->pushButton_5->setPalette(unPressed);
    ui->pushButton_3->setPalette(unPressed);
    ui->pushButton_4->setPalette(unPressed);
    ui->pushButton_6->setPalette(unPressed);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_5->setEnabled(false);
    // Определение лямбда-выражения для осуществления выбора файла в диалоговом окне
    auto f= [=]() {filename = QFileDialog::getOpenFileName(this,
                                                               "Select source file",
                                                               ".",
                                                               "Text files (*.ascii);; All files (*.*)");
    };
    // Определение лямбда-выражения для осуществления инициализации траектории
    auto init= [=]() {
        if (!filename.isEmpty()&& !filename.isNull())
        {
            Trajectory* tmp = new Trajectory(filename);

            //trj = new Trajectory(filename);
            if (tmp->CorrectData())
            {
                if (scene!=nullptr)
                {
                    delete scene;
                    scene = nullptr;
                };
                if (trj!=nullptr)
                {
                    delete trj;
                    trj = nullptr;
                }
                trj = tmp;
                tmp = nullptr;
                scene = new Trajectory3D(this, trj);
                ui->gridLayout_2->addWidget(scene);
                scene->show();
                ui->pushButton_2->setEnabled(true);
                ui->pushButton_3->setEnabled(true);
                ui->pushButton_4->setEnabled(true);
                ui->pushButton_5->setEnabled(true);
                ui->pushButton_4->click();
            }
            else
            {
                delete tmp;
            }
        }
    };
    // Определение лямбда-выражения для вывода сообщения-справки
    auto info= [=]() {QMessageBox::information(this, "Help",
    "\nСиним цветом отмечен план траектории\nЧерным цветом - текущая траектория\nЗеленым цветом - горизонты"
        "\nКрасным цветом - тревожные точки (точки, отклонившиеся от плана) и отрезки (отрезки, пересекающие горизонт)"
        "\nДля приближения или отдаления воспользуйтесь колесиком мыши"
        "\nДля поворота зажмите левую клавишу мыши");};

    // Определение действий кнопок при нажатии

    // Чтение выбранного файла и построение траектории
    connect(ui->pushButton, &QPushButton::clicked, ui->pushButton, [=](){ f(); init();ui->pushButton_6->setPalette(unPressed); });
    // Динамическое бурение (последовательное отображение точек плана и траектории)
    connect(ui->pushButton_2, &QPushButton::clicked, ui->pushButton_2, [=](){scene->dynamic();});
    // Вид сверху (XY)
    connect(ui->pushButton_3, &QPushButton::clicked, ui->pushButton_3, [=](){scene->XY();ui->pushButton_3->setPalette(pressed);ui->pushButton_4->setPalette(unPressed);});
    // Вид сбоку (XZ)
    connect(ui->pushButton_4, &QPushButton::clicked, ui->pushButton_4, [=](){scene->XZ();ui->pushButton_4->setPalette(pressed);ui->pushButton_3->setPalette(unPressed);});
    // Вывод справочной информации
    connect(ui->pushButton_5, &QPushButton::clicked, ui->pushButton_5, [=](){info();});
    // Открытие заготовленного файла для представления примера построения траектории
    connect(ui->pushButton_6, &QPushButton::clicked, ui->pushButton_6, [=](){filename = ":/example.ascii"; if(QFile::exists(filename)) {init(); ui->pushButton_6->setPalette(pressed);}});

}


MarsDrilling::~MarsDrilling()
{
    delete scene;
    if (trj!=nullptr) delete trj;
    delete ui;
}

