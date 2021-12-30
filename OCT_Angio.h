#pragma once

#define _USE_MATH_DEFINES

#include <QtWidgets/QMainWindow>
#include "ui_OCT_Angio.h"
#include "Header_OCTA_LayerMapping.h"
#include "Header_Hessian.h"
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QObject>
#include <QProcess>
#include <QLabel>
#include <QDir>
#include <QButtonGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <conio.h>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

class OCT_Angio : public QMainWindow
{
    Q_OBJECT

public:
    OCT_Angio(QWidget *parent = Q_NULLPTR);

    //Defining Variables
    int imageID_1 = 1, imageID_2 = 1;
    int R, G, B;
    int idx1_1 = 0, idx1_2 = 2, idx2_1 = 0, idx2_2 = 2;
    int x = 2, y = 2;
    int m1_1 = 0, m1_2 = 0, m2_1 = 0, m2_2 = 0;
    int bright_val, bright_val_enface;
    float contrast_val, contrast_val_enface;
    char imageLoad[128], fileName[128];
    bool edit = false;

    Mat enface, RegEnface;
    vector<Mat> EnfaceAngio_1, EnfaceAngio_2, EnfaceReg_1, EnfaceReg_2;

    QProcess* process = new QProcess();

    QGraphicsScene* Scene_1 = new QGraphicsScene;
    QGraphicsScene* Scene_2 = new QGraphicsScene;
    QGraphicsScene* Scene_3 = new QGraphicsScene;
    QGraphicsScene* Scene_4 = new QGraphicsScene;

    QGraphicsPixmapItem* item;

public slots:
     //Initial Process
    void intializeProcess();
    void addSlotsToGroup();

    //Segmentation & Enface Process
    void drawLines(QGraphicsView*, QGraphicsScene*, int);
    void drawLines_Enface(QGraphicsView*, QGraphicsScene*, Mat, int);
    void segmentation(QGraphicsView*, QGraphicsScene*, int, int, int, int, int);
    void layerColor(int);
    void read_CSV(char*);
    void validatePixMap(QLabel*, Mat);

    //Edit Layers Process
    void editLayers(QGraphicsView*, int);
    void onGroupButtonClicked(int);
    
protected:
    //Mouse Events Process
    bool eventFilter(QObject*, QEvent*);

private slots:
    //Page_0 Widget Components
    void on_pushButton_2_clicked();

    void on_on_combo_box_octa_single_image_currentIndexChanged(int);
    void on_on_combo_box_octa_single_enface_currentIndexChanged(int);
    void on_on_combo_box_octa_single_capillary_currentIndexChanged(int);
    void on_on_combo_box_octa_single_superficial_currentIndexChanged(int);

    void on_comboBox_12_currentIndexChanged(int);
    void on_comboBox_13_currentIndexChanged(int);
    void on_comboBox_15_currentIndexChanged(int);
    void on_comboBox_14_currentIndexChanged(int);

    void on_spinBox_4_valueChanged(int);
    void on_spinBox_5_valueChanged(int);
    void on_spinBox_7_valueChanged(int);
    void on_spinBox_6_valueChanged(int);

    //Page_1 Widget Components
    void on_checkBox_seg_clicked();

    void on_btn_Layer_report_clicked();
    void on_Reset_layer_2_clicked();
    void on_h_scroll_macula_single_layer_ok_2_clicked();
    void on_h_scroll_macula_single_layer_cancel_2_clicked();
    void on_reset_bright_clicked();
    void on_reset_contrast_clicked();
    void on_btn_close_measure_clicked();

    void on_horizontalSlider_bright_valueChanged(int);
    void on_horizontalSlider_contrast_valueChanged(int);

    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_radioButton_4_clicked();
    void on_radioButton_5_clicked();
    void on_radioButton_6_clicked();

    //Page_2 Widget Components
    void on_checkBox_seg_3_clicked();
    void on_checkBox_2_clicked();

    void on_radioButton_41_clicked();
    void on_radioButton_42_clicked();

    void on_horizontalSlider_bright_3_valueChanged(int);
    void on_horizontalSlider_contrast_3_valueChanged(int);

    void on_reset_bright_3_clicked();
    void on_reset_contrast_3_clicked();
    void on_btn_close_measure_2_clicked();

    void on_on_combo_box_octa_single_capillary_3_currentIndexChanged(int);
    void on_comboBox_currentIndexChanged(int);
    void on_comboBox_2_currentIndexChanged(int);

    void on_spinBox_valueChanged(int);
    void on_spinBox_2_valueChanged(int);

    //Page_3 Widget Components
    void on_btn_close_measure_4_clicked();

    //Page_4 Widget Components
    void on_checkBox_seg_4_clicked();

    void on_on_combo_box_octa_single_capillary_4_currentIndexChanged(int);
    void on_comboBox_4_currentIndexChanged(int);
    void on_comboBox_3_currentIndexChanged(int);

    void on_spinBox_3_valueChanged(int);
    void on_spinBox_8_valueChanged(int);

    void on_btn_close_measure_3_clicked();

    //Page_5 Widget Components
    void on_checkBox_seg_5_clicked();
    void on_checkBox_5_clicked();

    void on_radioButton_44_clicked();
    void on_radioButton_45_clicked();

    void on_horizontalSlider_bright_4_valueChanged(int);
    void on_horizontalSlider_contrast_4_valueChanged(int);

    void on_reset_bright_4_clicked();
    void on_reset_contrast_4_clicked();
    void on_btn_close_measure_6_clicked();

    void on_on_combo_box_octa_single_capillary_5_currentIndexChanged(int);
    void on_comboBox_5_currentIndexChanged(int);
    void on_comboBox_6_currentIndexChanged(int);

    void on_spinBox_9_valueChanged(int);
    void on_spinBox_10_valueChanged(int);

    //Page_6 Widget Components
    void on_checkBox_seg_2_clicked();

    void on_btn_Layer_report_2_clicked();
    void on_Reset_layer_clicked();
    void on_h_scroll_macula_single_layer_ok_clicked();
    void on_h_scroll_macula_single_layer_cancel_clicked();
    void on_reset_bright_2_clicked();
    void on_reset_contrast_2_clicked();
    void on_btn_close_measure_5_clicked();

    void on_horizontalSlider_bright_2_valueChanged(int);
    void on_horizontalSlider_contrast_2_valueChanged(int);

    void on_radioButton_10_clicked();
    void on_radioButton_11_clicked();
    void on_radioButton_12_clicked();
    void on_radioButton_7_clicked();
    void on_radioButton_8_clicked();
    void on_radioButton_9_clicked();

private:
    Ui::OCT_AngioClass ui;
};


class graphicsscene : public QGraphicsScene
{
    Q_OBJECT

public:
    //Edit Layer Process
    void graphicsLineItem();
    void removeEllipse(int);
    void HighlightLayer(int);

    //Defining Graphics Items
    QList<QGraphicsLineItem*> vect1;
    QList<QGraphicsLineItem*> vect2;
    QList<QGraphicsLineItem*> vect3;
    QList<QGraphicsLineItem*> vect4;
    QList<QGraphicsLineItem*> vect5;
    QList<QGraphicsLineItem*> vect6;
    QList<QGraphicsLineItem*> vect7;
    QList<QGraphicsLineItem*> vect8;

    QList < QGraphicsEllipseItem*> ellipse1;
    QList < QGraphicsEllipseItem*> ellipse2;
    QList < QGraphicsEllipseItem*> ellipse3;
    QList < QGraphicsEllipseItem*> ellipse4;
    QList < QGraphicsEllipseItem*> ellipse5;
    QList < QGraphicsEllipseItem*> ellipse6;
    QList < QGraphicsEllipseItem*> ellipse7;
    QList < QGraphicsEllipseItem*> ellipse8;

protected:
    //Mouse Move Event Process
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

private:
    //Defining Graphics Item
    QGraphicsPixmapItem* item1;
    QGraphicsLineItem* lineItem1;
    QGraphicsLineItem* lineItem2;
    QGraphicsLineItem* lineItem3;
    QGraphicsLineItem* lineItem4;
    QGraphicsLineItem* lineItem5;
    QGraphicsLineItem* lineItem6;
    QGraphicsLineItem* lineItem7;
    QGraphicsLineItem* lineItem8;

    QLine* line;

    QGraphicsEllipseItem* EllipseItem1;
    QGraphicsEllipseItem* EllipseItem2;
    QGraphicsEllipseItem* EllipseItem3;
    QGraphicsEllipseItem* EllipseItem4;
    QGraphicsEllipseItem* EllipseItem5;
    QGraphicsEllipseItem* EllipseItem6;
    QGraphicsEllipseItem* EllipseItem7;
    QGraphicsEllipseItem* EllipseItem8;
};