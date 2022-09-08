#ifndef TRAJECTORY3D_H
#define TRAJECTORY3D_H
#include "trajectory.h"



class Trajectory3D : public QGLWidget, public QOpenGLFunctions
{
   private:
    // имя файла
    Trajectory* trj;
    int trSizeDynamic;
    int plSizeDynamic;
      GLfloat xRot;
      GLfloat yRot;
      GLfloat zRot;
      GLfloat zTra;
      GLfloat nSca;
      QTimer* timer1; // таймер для динамического отображения плана
      QTimer* timer2; // таймер для динамического отображения траектории
      QPoint ptrMousePosition;
      void scale_plus();
      void scale_minus();

      void drawFigure(); // отрисовка плана, траектории и горизонтов

   protected:
      void initializeGL(); // инициализация сцены
      void resizeGL(int nWidth, int nHeight);//корректирование отображения при изменении размеров сцены
      void paintGL(); // отрисовка всех элементов сцены
      void mousePressEvent(QMouseEvent* pe); // фиксирование (определение) местоположения мыши при ее зажатии
      void mouseMoveEvent(QMouseEvent* pe); // поворот сцены при движении мыши с зажатой клавишей
      void wheelEvent(QWheelEvent* pe); // приближение/отдаление отображения при прокрутке колеса

   public:
      Trajectory3D(QWidget* parent = 0, Trajectory* tr = nullptr);
      ~Trajectory3D();
      void XY(); // поворот отображения для вида сверху
      void XZ(); // поворот отображения для вида сбоку
      void dynamic(); // динамическая отрисовка плана и траектории
};
#endif // TRAJECTORY3D_H
