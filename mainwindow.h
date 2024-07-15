#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QVector>
#include <QRect>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionOpen_New_Window_triggered();

    void on_pushButtonRectangle_clicked();
    void on_pushButtonEllipse_clicked();
    void on_pushButtonLine_clicked();
    void on_pushButtonRoundRectangle_clicked();
    void on_pushButtonPolygon_clicked();
    void on_pushButtonPen_clicked();
    void updateCanvas();

    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_pushButtonSelect_clicked();
    void clearSelectedArea();


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    MainWindow *nw;
    QString currentFile;
    bool drawing;
    bool movingShape = false;
    QPoint startPosition;
    QPoint endPosition;
    QImage *canvas;
    QImage tempCanvas;
    QPoint offset;
    QPoint movePos;



    enum Shape { None, Rectangle, Ellipse, Line, RoundRectangle, Polygon, Select };
    Shape currentShape;

    struct ShapeInfo
    {
        Shape shape;
        QRect rect;
        bool isSelected;
        QPoint start;

    };


    QVector<ShapeInfo> shapes;
    ShapeInfo* selectedShape;

    QPolygon polygon;
    QStack<QImage> undoStack;
    QStack<QImage> redoStack;
};
#endif
// MAINWINDOW_H
//        bool operator==(const ShapeInfo& other) const {
//            return (shape == other.shape && rect == other.rect && isSelected == other.isSelected);
//        }
