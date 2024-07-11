#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void SAVEAS();
    void on_actionOpen_New_Window_triggered();

    void on_pushButtonRectangle_clicked();
    void on_pushButtonEllipse_clicked();
    void on_pushButtonLine_clicked();
    void on_pushButtonRoundRectangle_clicked();
    void on_pushButtonPolygon_clicked();
    void on_pushButtonPen_clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    MainWindow *nw;
    QString currentFile;
    bool drawing;
    QPoint startPosition;
    QPoint endPosition;
    QImage *canvas;
    QImage tempCanvas;

    enum Shape { None, Rectangle, Ellipse, Line, RoundRectangle, Polygon };
    Shape currentShape;
    QPolygon polygon;
};
#endif // MAINWINDOW_H
