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
    , selectedShape(nullptr)
{
    currentFile = "";
    ui->setupUi(this);
    ui->label->setMouseTracking(true);

    canvas = new QImage(ui->label->size(), QImage::Format_RGB32);
    canvas->fill(Qt::white);
    ui->label->setPixmap(QPixmap::fromImage(*canvas));

    ui->pushButtonRectangle->setToolTip("Rectangle");
    ui->pushButtonEllipse->setToolTip("Ellipse");
    ui->pushButtonLine->setToolTip("Line");
    ui->pushButtonRoundRectangle->setToolTip("RoundRectangle");
    ui->pushButtonPolygon->setToolTip("Polygon");
    ui->pushButtonPen->setToolTip("Pen");
    ui->pushButtonUndo->setToolTip("Undo");
    ui->pushButtonRedo->setToolTip("Redo");
    ui->pushButtonSelect->setToolTip("Select");

    ui->pushButtonRectangle->setIcon(QIcon(":/new/prefix1/PaintIcons/rect.png"));
    ui->pushButtonEllipse->setIcon(QIcon(":/new/prefix1/PaintIcons/ellipse.png"));
    ui->pushButtonLine->setIcon(QIcon(":/new/prefix1/PaintIcons/line.png"));
    ui->pushButtonRoundRectangle->setIcon(QIcon(":/new/prefix1/PaintIcons/roundrectangle.png"));
    ui->pushButtonPolygon->setIcon(QIcon(":/new/prefix1/PaintIcons/polygon.png"));
    ui->pushButtonPen->setIcon(QIcon(":/new/prefix1/PaintIcons/icons8-pen-40.png"));
    ui->pushButtonUndo->setIcon(QIcon(":/new/prefix1/PaintIcons/Undo.png"));
    ui->pushButtonRedo->setIcon(QIcon(":/new/prefix1/PaintIcons/Redo.png"));

    connect(ui->pushButtonRectangle, &QPushButton::clicked, this, &MainWindow::on_pushButtonRectangle_clicked);
    connect(ui->pushButtonEllipse, &QPushButton::clicked, this, &MainWindow::on_pushButtonEllipse_clicked);
    connect(ui->pushButtonLine, &QPushButton::clicked, this, &MainWindow::on_pushButtonLine_clicked);
    connect(ui->pushButtonRoundRectangle, &QPushButton::clicked, this, &MainWindow::on_pushButtonRoundRectangle_clicked);
    connect(ui->pushButtonPolygon, &QPushButton::clicked, this, &MainWindow::on_pushButtonPolygon_clicked);
    connect(ui->pushButtonUndo, &QPushButton::clicked, this, &MainWindow::on_actionUndo_triggered);
    connect(ui->pushButtonRedo, &QPushButton::clicked, this, &MainWindow::on_actionRedo_triggered);

    //connect(ui->pushButtonPen, &QPushButton::clicked, this, &MainWindow::on_pushButtonPen_clicked);

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
    QPoint clickPos = ui->label->mapFrom(this, event->pos());
    if (event->button() == Qt::LeftButton && ui->label->rect().contains(clickPos))
    {
        if (currentShape == Select)
        {
            for (ShapeInfo &shapeInfo : shapes)
            {
                if (shapeInfo.rect.contains(clickPos))
                {
                    selectedShape = &shapeInfo;
                    offset = clickPos - shapeInfo.rect.topLeft();
                    movingShape = true;

//                    selectedShape->rect;
                    clearSelectedArea();

                    update();
                    return;
                }
            }
            selectedShape = nullptr;
            update();
        }
        else
        {
            drawing = true;
            startPosition = clickPos;
            endPosition = startPosition;
            update();
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint movePos = ui->label->mapFrom(this, event->pos());
    if (drawing && ui->label->rect().contains(movePos))
    {
        endPosition = movePos;
        qDebug() << "Mouse move at:" << movePos;
        update();
    }
    else if (movingShape &&  selectedShape && ui->label->rect().contains(movePos))
    {
        selectedShape->rect.moveTo(movePos - offset );
        //selectedShape->rect.translated(movePos - offset);
        qDebug() << "Shape moved to:" << selectedShape->rect;
        update();
    }
}

void MainWindow::clearSelectedArea()
{
    QPainter painter(canvas);

    painter.fillRect(selectedShape->rect.adjusted(0, 0, 1, 1), Qt::white);
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint releasePos = ui->label->mapFrom(this, event->pos());//mapFromParent(event->pos());

    if (drawing && event->button() == Qt::LeftButton)
    {
        drawing = false;
        endPosition = releasePos;

        undoStack.push_back(*canvas);
        while(!redoStack.empty())
        {
            redoStack.pop();
        }

        QPainter painter(canvas);
        painter.setPen(pen);

        ShapeInfo newShape;
        newShape.isSelected = false;

        if (currentShape == Rectangle)
        {
            painter.drawRect(QRect(startPosition, endPosition));
            newShape.shape = Rectangle;
            newShape.rect = QRect(startPosition, endPosition);
        }
        else if (currentShape == Ellipse)
        {
            painter.drawEllipse(QRect(startPosition, endPosition));
            newShape.shape = Ellipse;
            newShape.rect = QRect(startPosition, endPosition);
        }
        else if (currentShape == Line)
        {
            painter.drawLine(startPosition, endPosition);
            newShape.shape = Line;
            newShape.rect = QRect(startPosition, endPosition);
        }
        else if (currentShape == RoundRectangle)
        {
            painter.drawRoundRect(QRect(startPosition, endPosition));
            newShape.shape = RoundRectangle;
            newShape.rect = QRect(startPosition, endPosition);
        }
        else if (currentShape == Polygon)
        {
            if (!polygon.isEmpty())
            {
                painter.drawPolygon(polygon);
                newShape.shape = Polygon;
                newShape.rect = polygon.boundingRect();
            }
        }

        shapes.push_back(newShape);
        ui->label->setPixmap(QPixmap::fromImage(*canvas));
        update();
    }
    else if (movingShape && event->button() == Qt::LeftButton)
    {
        movingShape = false;
        undoStack.push_back(*canvas);
        while(!redoStack.empty())
        {
            redoStack.pop();
        }

        QPainter painter(canvas);
        painter.setPen(pen);

        if (selectedShape->shape == Rectangle)
        {
            painter.drawRect(selectedShape->rect);
        }
        else if (selectedShape->shape == Ellipse)
        {
            painter.drawEllipse(selectedShape->rect);
        }
        else if (selectedShape->shape == Line)
        {
            painter.drawLine(selectedShape->rect.topLeft(), selectedShape->rect.bottomRight());
        }
        else if (selectedShape->shape == RoundRectangle)
        {
            painter.drawRoundRect(selectedShape->rect);
        }
        else if (selectedShape->shape == Polygon)
        {
            painter.drawPolygon(QPolygon(selectedShape->rect));
        }

        ui->label->setPixmap(QPixmap::fromImage(*canvas));
        update();
    }
}

void MainWindow::on_pushButtonSelect_clicked()
{
    currentShape = Select;
    selectedShape = nullptr;
    movingShape = false;

}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{

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
            // Handle polygon drawing logic if necessary
        }
        ui->label->setPixmap(QPixmap::fromImage(tempCanvas));
    }
    if (selectedShape)
    {
        QPen highlightPen(Qt::blue);
        highlightPen.setStyle(Qt::DotLine);
        highlightPen.setWidth(2);

        QPixmap pixmap = ui->label->pixmap()->copy();
        QPainter labelPainter(&pixmap);
        labelPainter.setPen(highlightPen);
        labelPainter.drawRect(selectedShape->rect);
        ui->label->setPixmap(pixmap);
    }
}


void MainWindow::on_actionOpen_New_Window_triggered()
{
    nw = new MainWindow(this);
    nw->show();
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFile=="")
    {
        on_actionSave_As_triggered();
    }
    else
    {
        canvas->save(currentFile);
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save File"),"C:/Users/DT_Server/Desktop/QTtaskefilesSaved/paint",tr("Images (*.png *.xpm *.jpg)"));

    if (!file_name.isEmpty())
    {
        canvas->save(file_name);
        currentFile = file_name;
    }
}


void MainWindow::on_actionOpen_triggered()
{

    QString file_name = QFileDialog::getOpenFileName(this,"SaveFiles","C:/Users/DT_Server/Desktop/QTtaskefilesSaved/paint");
    if (!file_name.isEmpty())
    {
        currentFile = file_name;
        canvas->load(currentFile);
        ui->label->setPixmap(QPixmap::fromImage(*canvas));
    }
}


void MainWindow::on_actionUndo_triggered()
{
    if(!undoStack.empty())
    {
        redoStack.push(*canvas);
        *canvas = undoStack.top();
        undoStack.pop();
        ui->label->setPixmap(QPixmap::fromImage(*canvas));
    }
}

void MainWindow::on_actionRedo_triggered()
{
    if(!redoStack.empty())
    {
        undoStack.push(*canvas);
        *canvas = redoStack.top();
        redoStack.pop();
        ui->label->setPixmap(QPixmap::fromImage(*canvas));
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
