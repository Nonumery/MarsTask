#include "trajectory3d.h"

Trajectory3D::Trajectory3D(QWidget* parent, Trajectory* tr) : QGLWidget(parent)
{
   trj = tr;
   trSizeDynamic = trj->TrSize();
   plSizeDynamic = trj->PlSize();
   XZ();
   timer1 = new QTimer(this);
   timer2 = new QTimer(this);
   connect(timer1, &QTimer::timeout, this, [=]() {if (plSizeDynamic < trj->PlSize()) {plSizeDynamic++; timer1->start(250);} else if(trSizeDynamic < trj->TrSize()) {/*trSizeDynamic++;*/ timer2->start(250);} update(); });
   connect(timer2, &QTimer::timeout, this, [=]() {if(trSizeDynamic < trj->TrSize()) {trSizeDynamic++; timer2->start(250);} else {timer1->stop(); timer2->stop();} update();});
}

Trajectory3D::~Trajectory3D()
{


    delete timer1;
    delete timer2;

}

void Trajectory3D::initializeGL()
{
   initializeOpenGLFunctions();
    glPointSize(8.0f);
       glLineWidth(2.0f);
       glColor4f(0.6725f, 0, 0.6725f, 0.25f);
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       glEnable(GL_BLEND);
       glEnable(GL_POINT_SMOOTH);
       glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
       glEnable(GL_LINE_SMOOTH);
       glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
       glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
       glEnable(GL_DEPTH_TEST);
       glEnableClientState(GL_VERTEX_ARRAY);
       glEnableClientState(GL_COLOR_ARRAY);
}

void Trajectory3D::resizeGL(int nWidth, int nHeight)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   GLfloat ratio=(GLfloat)nHeight/(GLfloat)nWidth;

   if (nWidth>=nHeight)
      glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -20.0, 20.0);
   else
      glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -20.0, 20.0);

   glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

void Trajectory3D::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glScalef(nSca, nSca, nSca);
   glTranslatef(0.0f, zTra, 0.0f);
   glRotatef(xRot, 1.0f, 0.0f, 0.0f);
   glRotatef(yRot, 0.0f, 1.0f, 0.0f);
   glRotatef(zRot, 0.0f, 0.0f, 1.0f);


   drawFigure();
}

void Trajectory3D::mousePressEvent(QMouseEvent* pe)
{
   ptrMousePosition = pe->pos();
}

void Trajectory3D::mouseMoveEvent(QMouseEvent* pe)
{
   xRot += 180/nSca*(GLfloat)(pe->y()-ptrMousePosition.y())/height();
   zRot += 180/nSca*(GLfloat)(pe->x()-ptrMousePosition.x())/width();

   ptrMousePosition = pe->pos();

   update();
}

void Trajectory3D::wheelEvent(QWheelEvent* pe)
{
   if ((pe->delta())>0) scale_plus(); else if ((pe->delta())<0) scale_minus();

   update();
}


void Trajectory3D::scale_plus()
{
   nSca = nSca*1.1;
}

void Trajectory3D::scale_minus()
{
   nSca = nSca/1.1;
}


void Trajectory3D::XY()
{
   xRot=180; yRot=0; zRot=0; zTra=0;
   nSca = 1/(trj->MaxAbsXY()*1.0);

   update();
}

void Trajectory3D::XZ()
{
   xRot=90; yRot=0; zRot=0; zTra=0;
   nSca = 1/(trj->MaxAbsXZ()*1.0);

   update();
}

void Trajectory3D::dynamic()
{
    trSizeDynamic = 0;
    plSizeDynamic = 0;
    timer1->stop();
    timer2->stop();
    update();
    timer1->start(1000);
}




void Trajectory3D::drawFigure()
{
    glVertexPointer(3, GL_FLOAT, 0, trj->TrLines());
    glColorPointer(3, GL_FLOAT, 0, trj->TrLinesColor());
    glDrawElements(GL_LINE_STRIP, trSizeDynamic*2, GL_UNSIGNED_BYTE, trj->TrLinesIndex());


    glVertexPointer(3, GL_FLOAT, 0, trj->TrVertex());
    glColorPointer(3, GL_FLOAT, 0, trj->TrVertexColor());
    glDrawElements(GL_POINTS, trSizeDynamic, GL_UNSIGNED_BYTE, trj->TrVertexIndex());


    glVertexPointer(3, GL_FLOAT, 0, trj->PlVertex());
    glColorPointer(3, GL_FLOAT, 0, trj->PlVertexColor());
    glDrawElements(GL_LINE_STRIP, plSizeDynamic, GL_UNSIGNED_BYTE, trj->PlVertexIndex());


    glDrawElements(GL_POINTS, plSizeDynamic, GL_UNSIGNED_BYTE, trj->PlVertexIndex());
    int hSize = 0;
    while (hSize<trj->HorSize())
    {
        glVertexPointer(3, GL_FLOAT, 0, trj->HorVertex(hSize));
        glColorPointer(4, GL_FLOAT, 0, trj->HorVertexColor(hSize));
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, trj->HorVertexIndex());
        glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_BYTE, trj->HorVertexIndex());
        hSize++;
    }


}

