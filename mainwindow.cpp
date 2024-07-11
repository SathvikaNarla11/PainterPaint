#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QColorDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    currentFile = "";
    ui->setupUi(this);
    ui->label->setMouseTracking(true);

    canvas = new QImage(ui->label->size(), QImage::Format_RGB32);
    canvas->fill(Qt::white);
    ui->label->setPixmap(QPixmap::fromImage(*canvas));

    ui->pushButtonRectangle->setIcon(QIcon(":/new/prefix1/PaintIcons/rect.png"));
    ui->pushButtonEllipse->setIcon(QIcon(":/new/prefix1/PaintIcons/ellipse.png"));
    ui->pushButtonLine->setIcon(QIcon(":/new/prefix1/PaintIcons/line.png"));
    ui->pushButtonRoundRectangle->setIcon(QIcon(":/new/prefix1/PaintIcons/roundrectangle.png"));
    ui->pushButtonPolygon->setIcon(QIcon(":/new/prefix1/PaintIcons/polygon.png"));
    ui->pushButtonPen->setIcon(QIcon(":/new/prefix1/PaintIcons/icons8-pen-40.png"));

    connect(ui->pushButtonRectangle, &QPushButton::clicked, this, &MainWindow::on_pushButtonRectangle_clicked);
    connect(ui->pushButtonEllipse, &QPushButton::clicked, this, &MainWindow::on_pushButtonEllipse_clicked);
    connect(ui->pushButtonLine, &QPushButton::clicked, this, &MainWindow::on_pushButtonLine_clicked);
    connect(ui->pushButtonRoundRectangle, &QPushButton::clicked, this, &MainWindow::on_pushButtonRoundRectangle_clicked);
    connect(ui->pushButtonPolygon, &QPushButton::clicked, this, &MainWindow::on_pushButtonPolygon_clicked);

    connect(ui->actionSave_As, SIGNAL(triggered()), this,  SLOT(SAVEAS()));
//    connect(ui->pushButtonPen, &QPushButton::clicked, this, &MainWindow::on_pushButtonPen_clicked);

}

QColor newColor;
QPen pen;
void MainWindow::on_pushButtonPen_clicked()
{
    //    currentShape = Rectangle;
    newColor = QColorDialog::getColor();
    pen.setColor(newColor);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete canvas;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton  && ui->label->rect().contains(ui->label->mapFrom(this, event->pos()))) {
        drawing = true;
        startPosition = ui->label->mapFrom(this, event->pos());
        endPosition = startPosition;
        update();
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (drawing && ui->label->rect().contains(ui->label->mapFrom(this, event->pos())))
    {
        endPosition = ui->label->mapFrom(this, event->pos());
        update();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(drawing && event->button() == Qt::LeftButton)
    {
        drawing = false;
        endPosition = ui->label->mapFrom(this, event->pos());
        qDebug()<<startPosition<<" "<<endPosition;
        QPainter painter(canvas);

        painter.setPen(pen);

        if (currentShape == Rectangle)
        {
            painter.drawRect(QRect(startPosition, endPosition));
        }
        else if (currentShape == Ellipse)
        {
            painter.drawEllipse(QRect(startPosition, endPosition));
        }
        else if (currentShape == Line)
        {
            painter.drawLine(startPosition, endPosition);
        }
        else if (currentShape == RoundRectangle)
        {
            painter.drawRoundRect(QRect(startPosition, endPosition));
        }
        else if (currentShape == Polygon)
        {
            if (!polygon.isEmpty()) {
                painter.drawPolygon(polygon);
            }
            //            painter.drawEllipse(QRect(startPosition, endPosition));
        }

        ui->label->setPixmap(QPixmap::fromImage(*canvas));
        update();
    }
}

void MainWindow::on_pushButtonRectangle_clicked()
{
    currentShape = Rectangle;

}

void MainWindow::on_pushButtonEllipse_clicked()
{
    currentShape = Ellipse;
}
void MainWindow::on_pushButtonLine_clicked()
{
    currentShape = Line;

}
void MainWindow::on_pushButtonRoundRectangle_clicked()
{
    currentShape = RoundRectangle;

}
void MainWindow::on_pushButtonPolygon_clicked()
{
    currentShape = Polygon;

}


void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    QPainter painter(this);

    painter.drawImage(ui->label->pos(), *canvas);
    if (drawing)
    {
        tempCanvas = *canvas;
        QPainter tempPainter(&tempCanvas);
        painter.setPen(pen);
        tempPainter.setPen(pen);
        if (currentShape == Rectangle)
        {
            tempPainter.drawRect(QRect(startPosition, endPosition));
        }
        else if (currentShape == Ellipse)
        {
            tempPainter.drawEllipse(QRect(startPosition, endPosition));
        }
        else if (currentShape == Line)
        {
            tempPainter.drawLine(startPosition, endPosition);
        }
        else if (currentShape == RoundRectangle)
        {
            tempPainter.drawRoundRect(QRect(startPosition, endPosition));
        }
        else if (currentShape == Polygon)
        {
//            tempPainter.drawEllipse(QRect(startPosition, endPosition));
        }

        ui->label->setPixmap(QPixmap::fromImage(tempCanvas));
    }
}

void MainWindow::on_actionOpen_New_Window_triggered()
{
    nw = new MainWindow(this);
    nw->show();
}

void MainWindow::on_actionSave_triggered()
{
    qDebug()<<"File name out save: "<<currentFile;
//    if (currentFile == "")
//    {
//        on_actionSave_As_triggered();
//    }
//    else
//    {
//        QFile(file_name);
//        tempCanvas.save(file_name);

//    }
}

void MainWindow::SAVEAS()
{

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As File"), QDir::currentPath());
    if (!fileName.isEmpty())
    {
        canvas->save(fileName);
        drawing = false;
    }
}

void MainWindow::on_actionOpen_triggered()
{

    QString file_name = QFileDialog::getOpenFileName(this,"SaveFiles","C:/Users/DT_Server/Desktop/QTtaskefilesSaved");

    if (!file_name.isEmpty())
    {
        canvas->load(file_name);
        update();

    }
}










