#ifndef TRAJECTORY_H
#define TRAJECTORY_H
#include <QGLWidget>
#include <QOpenGLWidget>
#include <QVector2D>
#include <QOpenGLFunctions>
#include <QtWidgets>

class Trajectory:public QWidget
{
    QString filename;

    GLfloat* trVertexDoubleArray; // указатель на массив для визуализации линий траектории
    GLfloat* trColorDoubleArray; // указатель на массив для указания цвета линий траектории
    GLubyte* trIndexDoubleArray; // указатель на массив для указания порядка визуализации линий траектории
    GLfloat* trVertexArray; // указатель на массив для визуализации точек траектории
    GLfloat* trColorArray; // указатель на массив для указания цвета точек траектории
    GLubyte* trIndexArray; // указатель на массив для указания порядка визуализации точек траектории
    int trSize; // количество точек траектории

    GLfloat* plVertexArray;  // указатель на массив для визуализации точек плана
    GLfloat* plColorArray; // указатель на массив для указания цвета точек плана
    GLubyte* plIndexArray; // указатель на массив для указания порядка визуализации точек плана
    int plSize; // количество точек плана

    GLfloat* horVertexArray; // указатель на массив для визуализации горизонтов
    GLfloat* horColorArray; // указатель на массив для указания цвета горизонтов
    GLubyte* horIndexArray; // // указатель на массив для указания порядка визуализации точек плана
    int horSize; // количество горизонтов

    double deflection;

    GLfloat extreme[4][2]; // массив точек с крайними значениями (0-минимальное, 1-максимальное) координат отдельно по трем осям (0-x, 1-y, 2-z) и по всем одновременно (4)

    bool correctSize = false; // флаг для указания корректности введенных данных (количество точек и горизонтов совпадает с указанным)
    bool correctData = false; // флаг для указания корректности введенных данных (значения координат введены корректно)
    bool checkAngles(float* points, int pSize); // проверка углов траектории (<=60 градусов)
    bool checkZ(float* points, int pSize); // проверка направления траектории (значения координат на оси Z: z1<=z2)
    bool FindAlertPoints(); // поиск тревожных точек и их указание (красный цвет)
    bool FindAlertLines(); // поиск тревожных отрезков и их указание (красный цвет)
    void MovePoints(float fromCenter); // сдвиг координат по оси Z (сдвиг центра, смена точки обзора)
    void ExtremeValue(); // определение предельных значений координат
public:

    int TrSize() const {return trSize;}
    GLfloat* TrLines() const {return trVertexDoubleArray;}
    GLfloat* TrLinesColor() const {return trColorDoubleArray;}
    GLubyte* TrLinesIndex() const {return trIndexDoubleArray;}
    GLfloat* TrVertex() const {return trVertexArray;}
    GLfloat* TrVertexColor() const {return trColorArray;}
    GLubyte* TrVertexIndex() const {return trIndexArray;}

    int PlSize() const {return plSize;}
    GLfloat* PlVertex() const {return plVertexArray;}
    GLfloat* PlVertexColor() const {return plColorArray;}
    GLubyte* PlVertexIndex() const {return plIndexArray;}

    int HorSize() const {return horSize;}
    GLfloat* HorVertex(int i) const {return horVertexArray+i*12;}
    GLfloat* HorVertexColor(int i) const {return horColorArray+i*16;}
    GLubyte* HorVertexIndex() const {return horIndexArray;}
    GLfloat Extreme(int i, int j) { return extreme[i%4][j%2]; }
    GLfloat MaxAbsXY(); // поиск числа, наибольшего по модулю, среди значений на осях x и y
    GLfloat MaxAbsXZ(); // поиск числа, наибольшего по модулю, среди значений на осях x и z


    Trajectory(QString fn);
    ~Trajectory();
    bool CorrectSize() {return correctSize;}
    bool CorrectData() {return correctData;}

};

#endif // TRAJECTORY_H
