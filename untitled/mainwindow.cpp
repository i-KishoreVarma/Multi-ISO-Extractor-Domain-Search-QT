#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QtDebug>
#include <QListWidgetItem>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_File_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                 tr("Open File"),
                                 "",
                                 "All Files (*.*);;OFF file (*.off)");
    ui->openGLWidget->setModelFile(fileName);
    ui->openGLWidget->modelAvailable = true;
    ui->openGLWidget->repaint();
    ui->scrollAreaWidgetContents->setEnabled(true);
}

void MainWindow::on_ISOSurfacesList_itemClicked(QListWidgetItem *item)
{
    windowState.setCurISOSurfaceItem(item);
    ui->label->setText(item->text());
    int id = windowState.getCurISOSurfaceID();
    updateProperties(id);
}

void MainWindow::updateProperties(int i)
{
    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(i);
    if(isoSurface==NULL) return;
    auto minmaxInfo = isoSurface->getMinMax();
    ui->ISOValue->setRange(minmaxInfo.first,minmaxInfo.second);
    ui->ISOValue->setValue(isoSurface->getISOvalue());
    ui->opacityValue->setRange(0,100);
    ui->opacityValue->setValue(isoSurface->getOpacity());
}

void MainWindow::updateU(int position)
{
    int id = windowState.getCurISOSurfaceID();
    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(id);
    if(isoSurface==0) return;
    auto val = ui->ISOValue->value();
    ui->label->setText("Started  "+QString::number(val));
    isoSurface->setISOValue(position,true,true);
    ui->openGLWidget->repaint();
}
void MainWindow::on_ISOValue_sliderMoved(int position)
{
    ui->label->setText(QString::number(position));

    QTimer::singleShot(300, [=]() { updateU(position); } );
    /*
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, [=]() {
        updateU(position);
   });
    timer->start(5000);*/
}

void MainWindow::on_ISOValue_sliderPressed()
{
//    ui->label->setText("Pressed");
}

void MainWindow::on_ISOValue_sliderReleased()
{
    int id = windowState.getCurISOSurfaceID();
    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(id);
    if(isoSurface==0) return;

    auto val = ui->ISOValue->value();
    ui->label->setText("Started  "+QString::number(val));
    isoSurface->setISOValue(val,true,false);
    ui->label->setText("Done "+QString::number(val));
    ui->openGLWidget->repaint();
}


void MainWindow::on_opacityValue_sliderReleased()
{
    int id = windowState.getCurISOSurfaceID();
    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(id);
    if(isoSurface==NULL) return;
    auto position = ui->opacityValue->value();
    isoSurface->setOpacity(position);
    ui->label->setText("Opacity "+QString::number(position));
    ui->openGLWidget->repaint();
}



void MainWindow::on_addISOButton_clicked()
{
    ui->ISOSurfacesList->addItem("ISO Surface "+QString::number(windowState.nextISOSurfaceID));
    auto &rawModel = ui->openGLWidget->rawModel;
    rawModel.addISOSurface(windowState.nextISOSurfaceID++);
}


void MainWindow::on_deleteISOButton_clicked()
{
    int id = windowState.getCurISOSurfaceID();
    ui->openGLWidget->rawModel.removeISOSurface(id);
    auto it = ui->ISOSurfacesList->takeItem(ui->ISOSurfacesList->currentRow());
    delete it;
    windowState.setCurISOSurfaceItem(NULL);
}

