#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "WindowState.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    WindowState windowState;
    void updateProperties(int i);

private slots:
    void on_actionOpen_File_triggered();

    void on_ISOValue_sliderMoved(int position);

    void on_ISOSurfacesList_itemClicked(QListWidgetItem *item);

    void on_ISOValue_sliderPressed();

    void on_ISOValue_sliderReleased();

    void on_opacityValue_sliderReleased();


    void on_addISOButton_clicked();

    void on_deleteISOButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
