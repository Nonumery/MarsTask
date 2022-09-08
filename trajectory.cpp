#include "trajectory.h"

bool Trajectory::checkAngles(float* points, int pSize) // проверка - угол между векторами не должен превышать 60 градусов
{
    if (pSize < 3) return false;
    float* p1 = new float[3] {-points[0] + points[3], -points[1] + points[4], -points[2] + points[5]};
    float* p2;
    for(int i = 6; i < pSize*3-2; i+=3)
    {
        p2 = new float[3] {-points[i-3] + points[i], -points[i-2] + points[i+1], -points[i-1] + points[i+2]};
        float p1xp2 = (p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2]);
        float lp1 = sqrt(p1[0]*p1[0]+p1[1]*p1[1]+p1[2]*p1[2]);
        float lp2 = sqrt(p2[0]*p2[0]+p2[1]*p2[1]+p2[2]*p2[2]);
        delete[] p1;
        p1 = p2;
        p2 = nullptr;
        if ((p1xp2/(lp1*lp2))<0.5f) {delete[] p1; return false;}
    }
        delete[] p1;
        return true;
}
bool Trajectory::checkZ(float* points, int pSize) // проверка - значение на оси z не должно быть отрицательным и не должно уменьшаться
{
    if (pSize < 2) return false;
    float nZ = points[2];
    for(int i = 5; i < pSize*3; i+=3)
    {
        if (points[i]<0 || points[i]<nZ) return false;
        nZ = points[i];
    }
    return true;
}

Trajectory::Trajectory(QString fn)
{
    filename = fn;
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QString d = f.readAll();
    f.close();

    try {
        bool trInput = true;
        bool plInput = true;
        bool horInput = true;
        QStringList list = d.split("\n", QString::SkipEmptyParts);
        int nStr = 0;
        // Чтение первой строки (число точек траектории, плана, число горизонтов)
        QStringList v = list[nStr++].trimmed().split(" ");
        if (v.size()!=3) throw 1; // некорректный ввод количества точек или горизонтов
        trSize = v[0].toInt(&trInput);
        plSize = v[1].toInt(&plInput);
        horSize = v[2].toInt(&horInput);
        if(!trInput||!plInput||!horInput) throw 1; // ошибка формата записи количества точек и горизонтов
        if (list.size()!=(trSize+plSize+horSize+2)
                || trSize < 2 || trSize > 100
                || plSize < 2 || plSize > 100
                || horSize < 0 || horSize > 10) throw 1; // недопустимое количество точек или горизонтов

        // Чтение второй строки (отклонение)
        deflection = list[nStr++].toFloat(&trInput);
        if (!trInput||deflection<0.00f||deflection>10000.00f) throw 0; // невозможное отклонение

        // Проверка содержимого файла (по количеству данных о координатах в строках)
        for (int i = 0; i < trSize+plSize; i++)
        {
            v = list[i+2].trimmed().split(" ", QString::SkipEmptyParts);
            if (v.size()!=3) throw 2; // некорректный ввод координат точек траектории и плана (количество координат)
        }
        for (int i = trSize+plSize; i < trSize+plSize+horSize; i++)
        {
            v = list[i+2].trimmed().split(" ", QString::SkipEmptyParts);
            if (v.size()!=4) throw 3/*QException()*/; // 3 - некорректный ввод координат горизонтов (количество координат)
        }

        correctSize = true; // подтверждение соответствия количества координат с заданным числом точек и горизонтов

        // Выделение памяти для хранения координат вершин траектории
        trVertexDoubleArray = new GLfloat[trSize*3*2];
        trColorDoubleArray = new GLfloat[trSize*3*2];
        trIndexDoubleArray = new GLubyte[trSize*2];
        for (int i = 0; i < trSize*3*2; i++)
        {
            trColorDoubleArray[i] = 0.0f; // запись цвета по умолчанию
        }



        trVertexArray = new GLfloat[trSize*3];
        trColorArray = new GLfloat[trSize*3];
        for (int i = 0; i < trSize*3; i++)
        {
            trColorArray[i] = 0.0f;
        }
        trIndexArray = new GLubyte[trSize];

        plVertexArray = new GLfloat[plSize*3];
        plColorArray = new GLfloat[plSize*3];
        for (int i = 0; i < plSize*3; i++)
            plColorArray[i] = 0.0f;
        plIndexArray = new GLubyte[plSize];

        horVertexArray = new GLfloat[horSize*2*3 *2];
        horColorArray = new GLfloat[horSize*2*4 *2];
        for (int i = 0; i < horSize*12; i++)
            horColorArray[i] = 0.0f;
        horIndexArray = new GLubyte[horSize*2*2];






        // Чтение координат точек траектории и их запись
        for (int i = 0; i < trSize*3; i+=3)
        {
            v = list[nStr++].trimmed().split(" ", QString::SkipEmptyParts);
            for (int j = 0; j < 3; j++)
            {
                trVertexArray[i+j] = v[j].toFloat(&trInput);
                if(!trInput||(trVertexArray[i+j]>10000.00f||trVertexArray[i+j]<-10000.00f)) throw 4;
                trVertexDoubleArray[(i*2+j)] = v[j].toFloat(&trInput);
                trVertexDoubleArray[i*2+j+3] = v[j].toFloat(&trInput);
                //
            }
            trIndexArray[i/3] = i/3;
            //
            trIndexDoubleArray[i*2/3] = i*2/3;
            trIndexDoubleArray[i*2/3+1] = i*2/3+1;
            //
        }

        // Чтение координат точек плана и их запись
        for (int i = 0; i < plSize*3; i+=3)
        {
            v = list[nStr++].trimmed().split(" ", QString::SkipEmptyParts);
            for (int j = 0; j < 3; j++)
            {
                plVertexArray[i+j] = v[j].toFloat(&plInput);
                if(!plInput||(plVertexArray[i+j]>10000.00f||plVertexArray[i+j]<-10000.00f)) throw 4;

            }
            plColorArray[i+2] = 1.0f;
            plIndexArray[i/3] = i/3;
        }



        // Чтение координат точек горизонта и их запись
        for (int i = 0; i < horSize*2*3 *2; i+=12)
        {
            v = list[nStr++].trimmed().split(" ", QString::SkipEmptyParts);
            for (int j = 0; j < 2; j++)
            {
                horVertexArray[i+j*6] = v[j*2].toFloat(&horInput);
                if(!horInput||(horVertexArray[i+j*6]>10000.00f||horVertexArray[i+j*6]<-10000.00f)) throw 4;
                horVertexArray[i+j*6+1] = 0.0f;
                horVertexArray[i+j*6+2] = v[j*2+1].toFloat(&horInput);
                if(!horInput||(horVertexArray[i+j*6+2]>10000.00f||horVertexArray[i+j*6+2]<-10000.00f)) throw 4;
                horColorArray[i/3*4+j*8+1] = 1.0f;
                horColorArray[i/3*4+j*8+3] = 0.2f;

                horVertexArray[i+j*6+3] = horVertexArray[i+j*6];
                horVertexArray[i+j*6+4] = 0.0f/*+minY*/;
                horVertexArray[i+j*6+5] = horVertexArray[i+j*6+2];
                horColorArray[i/3*4+j*8+5] = 1.0f;
                horColorArray[i/3*4+j*8+7] = 0.2f;
            }

            horIndexArray[i/6*2] = i/6*2;
            horIndexArray[i/6*2+1] = i/6*2+1;
            horIndexArray[i/6*2+2] = i/6*2+3;
            horIndexArray[i/6*2+3] = i/6*2+2;




        }
        // Определение предельных значений координат для центрирования отображения траектории на экране и ее достаточного приближения
        ExtremeValue();
        for (int i = 0; i < horSize*2*3 *2; i+=12)
        {
            for (int j = 0; j < 2; j++)
            {
                horVertexArray[i+j*6+1] = 0.0f+extreme[1][1];
                horVertexArray[i+j*6+4] = 0.0f+extreme[1][0];
            }

        }

        if (!checkAngles(trVertexArray, trSize) || !checkZ(trVertexArray, trSize)) throw 5; // несоответствие требованиям построения траектории (угол больше 60 градусов или происходит движение вверх (по оси Z))
        if (!checkAngles(plVertexArray, plSize) || !checkZ(plVertexArray, plSize)) throw 6; // несоответствие требованиям построения траектории плана (угол больше 60 градусов или происходит движение вверх (по оси Z))

        correctData=true; // подтверждение корректности ввода данных

        // Поиск тревожных точек и отрезков
        FindAlertPoints();
        FindAlertLines();
        // Центрирование отображения траектории (сдвиг по оси Z)
        MovePoints((extreme[3][1]+extreme[3][0])/2);

    }
    catch (int i)
    {
        switch(i)
        {
            case 0: QMessageBox::information(this, "Wrong input", "Некорректный ввод значения отклонения"); break;
            case 1: QMessageBox::information(this, "Wrong input", "Некорректный ввод количества точек или горизонтов"); break;
            case 2: QMessageBox::information(this, "Wrong input", "Некорректный ввод координат точек траектории и/или плана (необрабатываемое число координат)"); break;
            case 3: QMessageBox::information(this, "Wrong input", "Некорректный ввод координат горизонтов (необрабатываемое число координат)"); break;
            case 4: QMessageBox::information(this, "Wrong input", "Некорректный ввод значений координат точек траектории, плана или горизонта (выход за диапазон допустимых значений или неверный формат значений (-10000≤a.b≤10000))"); break;
            case 5: QMessageBox::information(this, "Requirements are not met", "Несоответствие требованиям построения траектории (угол больше 60 градусов или происходит движение вверх (по оси Z))"); break;
            case 6: QMessageBox::information(this, "Requirements are not met", "Несоответствие требованиям построения плана (угол больше 60 градусов или происходит движение вверх (по оси Z))"); break;
            default: QMessageBox::information(this, "Wrong input?", "There are some issues in file content"); break;
        }
    }
    catch(QUnhandledException e)
    {
        QMessageBox::information(this, "???", "Uknown error");
    }

    catch (QException e)
    {
        QMessageBox::information(this, "Wrong input", "");

    }


}
void Trajectory::ExtremeValue()
{
    for(int i = 0; i < 3;i++)
    {
        extreme[i][0]=trVertexArray[i];
        extreme[i][1]=trVertexArray[i];
    }
    for(int i = 1; i < trSize*3; i++)
    {
        if (trVertexArray[i]<extreme[i%3][0]) extreme[i%3][0] = trVertexArray[i];
        if (trVertexArray[i]>extreme[i%3][1]) extreme[i%3][1] = trVertexArray[i];
    }
    for(int i = 0; i < plSize*3; i++)
    {
        if (plVertexArray[i]<extreme[i%3][0]) extreme[i%3][0] = plVertexArray[i];
        if (plVertexArray[i]>extreme[i%3][1]) extreme[i%3][1] = plVertexArray[i];
    }
    for(int i = 0; i < horSize*3*4; i++)
    {
        if (horVertexArray[i]<extreme[i%3][0]) extreme[i%3][0] = horVertexArray[i];
        if (horVertexArray[i]>extreme[i%3][1]) extreme[i%3][1] = horVertexArray[i];
    }
    extreme[3][0] = ((extreme[0][0]<extreme[1][0])?(extreme[0][0]<extreme[2][0]?extreme[0][0]:extreme[2][0]):(extreme[1][0]<extreme[2][0]?extreme[1][0]:extreme[2][0]));
    extreme[3][1] = ((extreme[0][1]>extreme[1][1])?(extreme[0][1]>extreme[2][1]?extreme[0][1]:extreme[2][1]):(extreme[1][1]>extreme[2][1]?extreme[1][1]:extreme[2][1]));
}

GLfloat Trajectory::MaxAbsXY()
{
    float temp = (((abs(extreme[0][0]))>(abs(extreme[0][1])))?(abs(extreme[0][0])):(abs(extreme[0][1])));
    float temp2 = (((abs(extreme[1][0]))>(abs(extreme[1][1])))?(abs(extreme[1][0])):(abs(extreme[1][1])));
    return ((temp)>(temp2)?temp:temp2);
}

GLfloat Trajectory::MaxAbsXZ()
{
    float temp = (((abs(extreme[0][0]))>(abs(extreme[0][1])))?(abs(extreme[0][0])):(abs(extreme[0][1])));
    float temp2 = (((abs(extreme[2][0]))>(abs(extreme[2][1])))?(abs(extreme[2][0])):(abs(extreme[2][1])));
    return ((temp)>(temp2)?temp:temp2);
}
Trajectory::~Trajectory()
{
    if (correctSize)
    {

        delete[] trVertexDoubleArray;
        delete[] trColorDoubleArray;
        delete[] trIndexDoubleArray;

        delete[] trVertexArray;
        delete[] trColorArray;
        delete[] plVertexArray;
        delete[] plColorArray;
        delete[] horVertexArray;
        delete[] horColorArray;
        delete[] trIndexArray;
        delete[] plIndexArray;
        delete[] horIndexArray;

    }
}


float Deflection(float* p, float* A, float* B) // определение отклонения текущей точки траектории от плана
{
    float vect[3] {B[0]-A[0], B[1]-A[1], B[2]-A[2]};
    float p_vect[3] {p[0]-A[0], p[1]-A[1], p[2]-A[2]};
    float vXp_v[3] = {p_vect[1]*vect[2]-p_vect[2]*vect[1], -(p_vect[0]*vect[2]-p_vect[2]*vect[0]), p_vect[0]*vect[1]-p_vect[1]*vect[0]};
    return sqrt(vXp_v[0]*vXp_v[0]+vXp_v[1]*vXp_v[1]+vXp_v[2]*vXp_v[2])/sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]);
}

bool Trajectory::FindAlertPoints()
{
    bool alerts = false;
    int i = 0;
    float curDeflection;
    while(i<trSize)
    {
        curDeflection = Deflection(&(trVertexArray[i*3]), &(plVertexArray[0]), &(plVertexArray[3]));
        int j = 1;
        while((j+1 < plSize) && (curDeflection > deflection)) // проверяются все точки со всеми линиями плана (следует оптимизировать)
        {
            curDeflection = Deflection(&(trVertexArray[i*3]), &(plVertexArray[j*3]), &(plVertexArray[j*3+3]));
            j++;
        }
        if (curDeflection > deflection) {trColorArray[i*3] = 1.0f;/*trColorPointsArray[i*3+1] = 1.0f;*/ alerts = true;} // установка красного цвета
        i++;
    }
    return alerts;
}

float min(float x1, float x2) {return x1>x2?x2:x1;}
float max(float x1, float x2) {return x1<x2?x2:x1;}
bool Cross(GLfloat* t1, GLfloat* t2, GLfloat* h1, GLfloat* h2) // определение пересечения двух линий (отрезков)
{
    float p1[2], p2[2], p3[2], p4[2];
    if (t1[0]>t2[0])
    {
        p1[0] = t2[0];
        p1[1] = t2[2];
        p2[0] = t1[0];
        p2[1] = t1[2];
    }
    else
    {
        p1[0] = t1[0];
        p1[1] = t1[2];
        p2[0] = t2[0];
        p2[1] = t2[2];
    }
    if (h1[0]>h2[0])
    {
        p3[0] = h2[0];
        p3[1] = h2[2];
        p4[0] = h1[0];
        p4[1] = h1[2];
    }
    else
    {
        p3[0] = h1[0];
        p3[1] = h1[2];
        p4[0] = h2[0];
        p4[1] = h2[2];
    }

    float x1 = min(p1[0], p2[0]);
    float x2 = max(p1[0], p2[0]);
    float y1 = min(p1[1], p2[1]);
    float y2 = max(p1[1], p2[1]);
    float x3 = min(p3[0], p4[0]);
    float x4 = max(p3[0], p4[0]);
    float y3 = min(p3[1], p4[1]);
    float y4 = max(p3[1], p4[1]);
    if ((x2>=x3&&x1<=x4&&y2>=y3&&y1<=y4) // пересечение прямоугольников, образованных заданными отрезками
            && ((p3[0]-p1[0])*(p2[1]-p1[1])-(p2[0]-p1[0])*(p3[1]-p1[1])) *
            ((p4[0]-p1[0])*(p2[1]-p1[1])-(p2[0]-p1[0])*(p4[1]-p1[1])) <=0 // определение знаков произведений векторов
            && ((p1[0]-p3[0])*(p4[1]-p3[1])-(p4[0]-p3[0])*(p1[1]-p3[1])) *
            ((p2[0]-p3[0])*(p4[1]-p3[1])-(p4[0]-p3[0])*(p2[1]-p3[1])) <=0) return true;
    return false;


}


bool Trajectory::FindAlertLines()
{
    bool alerts = false;
    int i = 0;
    while(i+1<trSize)
    {
        int j = 0;
        while(j<horSize)
        {
            if(Cross(&(trVertexArray[i*3]), &(trVertexArray[i*3+3]), &(horVertexArray[j*12]), &(horVertexArray[j*12+6])))
            {
                trColorArray[i*3] = 1.0f;
                trColorArray[i*3+3] = 1.0f;
                  trColorDoubleArray[i*3*2+3] = 1.0f; // красный цвет
                  trColorDoubleArray[(i+1)*3*2] = 1.0f;
                alerts = true;
                break;
            }
            j++;

        }
        i++;
    }
    return alerts;
}

void Trajectory::MovePoints(float fromCenter)
{
    for (int i = 0; i < trSize; i++)
    {
        trVertexDoubleArray[i*6+2] -= fromCenter;
        trVertexDoubleArray[i*6+5] -= fromCenter;
        trVertexArray[i*3+2] -= fromCenter;
    }
    for (int i = 0; i < plSize; i++)
    {
        plVertexArray[i*3+2] -= fromCenter;
    }
    for (int i = 0; i < horSize; i++)
    {
        horVertexArray[i*12+2] -= fromCenter;
        horVertexArray[i*12+5] -= fromCenter;
        horVertexArray[i*12+8] -= fromCenter;
        horVertexArray[i*12+11] -= fromCenter;
    }

}
