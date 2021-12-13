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
    bool openFile = true;
    auto fileName = QFileDialog::getOpenFileName(this,
                                 tr("Open File"),
                                 "../untitled/models/",
                                 "All Files (*.*);;OFF file (*.off)");
    auto fileNameSplitByDot = fileName.split(".");
    if(fileNameSplitByDot.length() <= 1) {
        openFile = false;
    }
    auto fileNameExtension = fileNameSplitByDot[1];
    if(fileName.length() == 0 or fileNameExtension != "raw") {
        openFile = false;
    }

    if(openFile) {
        windowState.clear();
        ui->openGLWidget->setModelFile(fileName);
        ui->openGLWidget->modelAvailable = true;
        ui->openGLWidget->repaint();
        ui->scrollAreaWidgetContents->setEnabled(true);
        ui->groupBox_2->setEnabled(false);
    }

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
    if(isoSurface->shouldDisplay) ui->enabledCheckBox->setCheckState(Qt::Checked);
    else ui->enabledCheckBox->setCheckState(Qt::Unchecked);
}

void MainWindow::computeRunTimeISO(int position)
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
    if(toggleContinuousISO){
        auto tm = chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - tp).count()/25000;
        //cout<<tm<<"\n";
        if(tm>10){

            computeRunTimeISO(position);
            tp = chrono::steady_clock::now();
        }
    }
}

void MainWindow::on_ISOValue_sliderPressed()
{
    tp = chrono::steady_clock::now();
//    ui->label->setText("Pressed");
}

void MainWindow::on_ISOValue_sliderReleased()
{
    tp = chrono::steady_clock::time_point();
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
    int nextISOSurfaceID = windowState.nextISOSurfaceID++;

    ui->ISOSurfacesList->addItem("ISO Surface "+QString::number(nextISOSurfaceID));

    auto &rawModel = ui->openGLWidget->rawModel;

    rawModel.addISOSurface(nextISOSurfaceID);

    // set newly added item as selected
    auto it = ui->ISOSurfacesList->item(ui->ISOSurfacesList->count()-1);

    ui->ISOSurfacesList->setCurrentItem(it);

    windowState.setCurISOSurfaceItem(it);

    ui->label->setText(it->text());

    int id = windowState.getCurISOSurfaceID();

    cout<< id<<'\n';

    updateProperties(id);

    ui->groupBox_2->setEnabled(true);
    ui->openGLWidget->repaint();
}



void MainWindow::on_deleteISOButton_clicked()
{
    int id = windowState.getCurISOSurfaceID();

    if(id == -1)
    {
        cout<<"Delete is -1\n";
        return;
    }

    ui->openGLWidget->rawModel.removeISOSurface(id);

    int currentRow = ui->ISOSurfacesList->currentRow();

    auto it = ui->ISOSurfacesList->takeItem(currentRow);

    delete it;

    if(ui->ISOSurfacesList->count()>0)
    {
        currentRow = (currentRow)% (ui->ISOSurfacesList->count());

        auto it = ui->ISOSurfacesList->item(currentRow);

        ui->ISOSurfacesList->setCurrentRow(currentRow);

        windowState.setCurISOSurfaceItem(it);

        ui->label->setText(it->text());

        int id = windowState.getCurISOSurfaceID();

        updateProperties(id);
        ui->groupBox_2->setEnabled(true);
    }
    else {
        windowState.setCurISOSurfaceItem(NULL);
        ui->groupBox_2->setEnabled(false);
    }

    ui->openGLWidget->repaint();
}


void MainWindow::on_enabledCheckBox_clicked()
{
    int id = windowState.getCurISOSurfaceID();

    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(id);

    if(isoSurface==0) return;

    isoSurface->toggleShouldDisplay();

    ui->openGLWidget->repaint();
}



void MainWindow::on_colorMapCheckBox_clicked()
{
    int id = windowState.getCurISOSurfaceID();

    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(id);

    if(isoSurface==0) return;

    isoSurface->toggleColorMap();

    ui->openGLWidget->repaint();
}


void MainWindow::on_continuousISOCheckBox_clicked()
{
    int id = windowState.getCurISOSurfaceID();

    auto isoSurface = ui->openGLWidget->rawModel.getISOSurface(id);

    if(isoSurface==0) return;

    this->toggleContinuousISO = !(this->toggleContinuousISO);
}
