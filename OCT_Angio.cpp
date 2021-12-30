#include "OCT_Angio.h"

//Defining Variables
graphicsscene* scene;
QButtonGroup* group_1 = new QButtonGroup;
QButtonGroup* group_2 = new QButtonGroup;
vector< vector<int> > CSV_Values_Layer;
vector< vector<int> > newPointsData;
int value = 0;

//Parent & Sub Directory Declarations
QDir dirData = "E:/Output";

QString SVImage = dirData.absolutePath() + "/SVImage";
QByteArray SV = SVImage.toLocal8Bit();
char* SVDir = SV.data();

QString avgReg = dirData.absolutePath() + "/AvgReg";
QByteArray reg = avgReg.toLocal8Bit();
char* RegDir = reg.data();

QString dataCSV = dirData.absolutePath() + "/Data_CSV";
QByteArray csv = dataCSV.toLocal8Bit();
char* CSVDir = csv.data();

QString CompImg = dirData.absolutePath() + "/CompositeImage";
QByteArray comp = CompImg.toLocal8Bit();
char* CIDir = comp.data();

OCT_Angio::OCT_Angio(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    showFullScreen();
    
    ui.stackWidget_octa->setCurrentIndex(0);
    ui.scroll_octa_single_h_view->installEventFilter(this);
    ui.lbl_octa_single_capillary->installEventFilter(this);
    ui.graphicsView->installEventFilter(this);
    ui.graphicsView_2->installEventFilter(this);
    ui.h_scroll_graphicsview->installEventFilter(this);
    ui.h_scroll_graphicsview_2->installEventFilter(this);
    ui.scroll_octa_single_v_view->installEventFilter(this);
    ui.graphicsView_3->installEventFilter(this);
    ui.graphicsView_4->installEventFilter(this);
    ui.lbl_octa_single->installEventFilter(this);
    ui.graphicsView_5->installEventFilter(this);
    ui.h_scroll_graphicsview_3->installEventFilter(this);
    ui.lbl_octa_single_enface->installEventFilter(this);
    ui.h_scroll_graphicsview_4->installEventFilter(this);
    ui.h_scroll_graphicsview_6->installEventFilter(this);
    ui.graphicsView_6->installEventFilter(this);
    ui.graphicsView_7->installEventFilter(this);
    ui.lbl_octa_single_superficial->installEventFilter(this);

    ui.groupBox_2->close();
    ui.groupBox_4->close();

    addSlotsToGroup();
    intializeProcess();

    connect(ui.pushButton_3, SIGNAL(clicked()), SLOT(close()));
    connect(group_1, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked(int)));
    connect(group_2, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked(int)));
}

//Initial Process
void OCT_Angio::intializeProcess()
{
    qDebug() << "Start";

    /*process->start("E:/OCT_Angiography/OCTA_Build/OCT_Angiography.exe", { "E:/OCT_Angiography/Mat_Input",dirData.absolutePath()});
    qDebug() << process->state();
    process->waitForFinished(-1);*/

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    segmentation(ui.graphicsView, Scene_1, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
    segmentation(ui.graphicsView_2, Scene_2, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    validatePixMap(ui.lbl_octa_single_capillary, EnfaceAngio_1[0]);
    validatePixMap(ui.lbl_octa_single_superficial, EnfaceAngio_2[0]);

    validatePixMap(ui.lbl_octa_single, EnfaceReg_1[0]);
    validatePixMap(ui.lbl_octa_single_enface, EnfaceReg_2[0]);

    qDebug() << "End";
}

void OCT_Angio::addSlotsToGroup()
{
    group_1->addButton(ui.radioButton_29, 0);
    group_1->addButton(ui.radioButton_30, 1);
    group_1->addButton(ui.radioButton_31, 2);
    group_1->addButton(ui.radioButton_32, 3);
    group_1->addButton(ui.radioButton_33, 4);
    group_1->addButton(ui.radioButton_34, 5);
    group_1->addButton(ui.radioButton_35, 6);
    group_1->addButton(ui.radioButton_36, 7);

    group_2->addButton(ui.radioButton_19, 0);
    group_2->addButton(ui.radioButton_20, 1);
    group_2->addButton(ui.radioButton_21, 2);
    group_2->addButton(ui.radioButton_22, 3);
    group_2->addButton(ui.radioButton_23, 4);
    group_2->addButton(ui.radioButton_24, 5);
    group_2->addButton(ui.radioButton_25, 6);
    group_2->addButton(ui.radioButton_26, 7);
}

//Segmentation & Enface Process
void OCT_Angio::drawLines(QGraphicsView* graphics, QGraphicsScene* scene, int number)
{
    scene->clear();
    QImage image;

    if (ui.stackWidget_octa->currentIndex() == 1)
    {
        image = QImage((uchar*)EnfaceReg_1[0].data, EnfaceReg_1[0].cols, EnfaceReg_1[0].rows, EnfaceReg_1[0].step, QImage::Format_Grayscale8);
    }
    else
    {
        image = QImage((uchar*)EnfaceReg_2[0].data, EnfaceReg_2[0].cols, EnfaceReg_2[0].rows, EnfaceReg_2[0].step, QImage::Format_Grayscale8);
    }

    QPixmap pixmap = QPixmap::fromImage(image);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen((QColor::QColor(64, 255, 255)), 2);

    qreal arrowSize = 7;
    painter.setPen(pen);
    painter.setBrush(QColor::QColor(64, 255, 255));

    QPoint start(0, number);
    QPoint end(299, number);

    QLineF line(end, start);

    double angle = atan2(-line.dy(), line.dx());

    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;

    painter.drawLine(line);
    painter.drawPolygon(arrowHead);

    scene->addPixmap(pixmap.scaled(graphics->width(), graphics->height()));
    graphics->setScene(scene);
}

void OCT_Angio::drawLines_Enface(QGraphicsView* graphics, QGraphicsScene* scene, Mat enfaceImage_Input, int number)
{
    Mat enfaceImage;
    QImage octimg_new;
    QPixmap pixmap;

    if (ui.stackWidget_octa->currentIndex() == 2)
    {
        if (ui.radioButton_42->isChecked())
        {
            enfaceImage_Input = OCT_A_LayerMapping_Filter_MIP(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
        }

        enfaceImage_Input.convertTo(enfaceImage, CV_8U, contrast_val_enface, bright_val_enface);

        if (ui.checkBox_2->isChecked())
        {
            applyColorMap(enfaceImage, enfaceImage, COLORMAP_JET);
            cvtColor(enfaceImage, enfaceImage, COLOR_BGR2RGB);
            octimg_new = QImage((uchar*)enfaceImage.data, enfaceImage.cols, enfaceImage.rows, enfaceImage.step, QImage::Format_RGB888);
        }
        else
        {
            octimg_new = QImage((uchar*)enfaceImage.data, enfaceImage.cols, enfaceImage.rows, enfaceImage.step, QImage::Format_Grayscale8);
        }

        pixmap = QPixmap::fromImage(octimg_new);

        if (ui.checkBox_seg_3->isChecked())
        {
            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QPen pen((QColor::QColor(64, 255, 255)), 2);

            qreal arrowSize = 7;
            painter.setPen(pen);
            painter.setBrush(QColor::QColor(64, 255, 255));

            QPoint start(0, number);
            QPoint end(299, number);

            QLineF line(end, start);

            double angle = atan2(-line.dy(), line.dx());

            QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
            QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

            QPolygonF arrowHead;
            arrowHead.clear();
            arrowHead << line.p1() << arrowP1 << arrowP2;

            painter.drawLine(line);
            painter.drawPolygon(arrowHead);
        }
    }

    else if (ui.stackWidget_octa->currentIndex() == 5)
    {
        if (ui.radioButton_45->isChecked())
        {
            enfaceImage_Input = OCT_A_LayerMapping_Filter_MIP(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
        }

        enfaceImage_Input.convertTo(enfaceImage, CV_8U, contrast_val_enface, bright_val_enface);

        if (ui.checkBox_5->isChecked())
        {
            applyColorMap(enfaceImage, enfaceImage, COLORMAP_JET);
            //imshow("EnfaceImage", enfaceImage);
            cvtColor(enfaceImage, enfaceImage, COLOR_BGR2RGB);
            octimg_new = QImage((uchar*)enfaceImage.data, enfaceImage.cols, enfaceImage.rows, enfaceImage.step, QImage::Format_RGB888);
        }
        else
        {
            //cvtColor(enfaceImage, enfaceImage, COLOR_BGR2RGB);
            octimg_new = QImage((uchar*)enfaceImage.data, enfaceImage.cols, enfaceImage.rows, enfaceImage.step, QImage::Format_Grayscale8);
        }

        pixmap = QPixmap::fromImage(octimg_new);

        if (ui.checkBox_seg_5->isChecked())
        {
            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QPen pen((QColor::QColor(64, 255, 255)), 2);

            qreal arrowSize = 7;
            painter.setPen(pen);
            painter.setBrush(QColor::QColor(64, 255, 255));

            QPoint start(0, number);
            QPoint end(299, number);

            QLineF line(end, start);

            double angle = atan2(-line.dy(), line.dx());

            QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
            QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

            QPolygonF arrowHead;
            arrowHead.clear();
            arrowHead << line.p1() << arrowP1 << arrowP2;

            painter.drawLine(line);
            painter.drawPolygon(arrowHead);
        }
    }

    else
    {
        enfaceImage = enfaceImage_Input;

        octimg_new = QImage((uchar*)enfaceImage.data, enfaceImage.cols, enfaceImage.rows, enfaceImage.step, QImage::Format_Grayscale8);

        pixmap = QPixmap::fromImage(octimg_new);

        if (ui.checkBox_seg_4->isChecked())
        {
            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QPen pen((QColor::QColor(64, 255, 255)), 2);

            qreal arrowSize = 7;
            painter.setPen(pen);
            painter.setBrush(QColor::QColor(64, 255, 255));

            QPoint start(0, number);
            QPoint end(299, number);

            QLineF line(end, start);

            double angle = atan2(-line.dy(), line.dx());

            QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
            QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

            QPolygonF arrowHead;
            arrowHead.clear();
            arrowHead << line.p1() << arrowP1 << arrowP2;

            painter.drawLine(line);
            painter.drawPolygon(arrowHead);
        }

    }

    scene->addPixmap(pixmap);
    scene->addPixmap(pixmap.scaled(graphics->width(), graphics->height()));
    graphics->setScene(scene);
}

void OCT_Angio::segmentation(QGraphicsView* gps, QGraphicsScene* scn, int Layer1, int Layer2, int n, int p1, int p2)
{
    ui.label->setNum(imageID_1);
    ui.label_4->setNum(imageID_2);

    char imageName[128];
    char csvName[128];
    Mat im1_gray;

    sprintf(imageName, "%s//%04d.tif", CIDir, n);
    im1_gray = imread(imageName);

    sprintf(csvName, "%s/Data%04d.csv", CSVDir, n);
    read_CSV(csvName);

    layerColor(Layer1);
    for (int bound = 0; bound < 500; bound++)
    {
        line(im1_gray, Point(bound, (int)CSV_Values_Layer[Layer1][bound] + p1), Point(bound + 1, (int)CSV_Values_Layer[Layer1][bound + 1] + p1), Scalar(B, G, R), 2);
    }

    layerColor(Layer2);
    for (int bound = 0; bound < 500; bound++)
    {
        line(im1_gray, Point(bound, (int)CSV_Values_Layer[Layer2][bound] + p2), Point(bound + 1, (int)CSV_Values_Layer[Layer2][bound + 1] + p2), Scalar(B, G, R), 2);
    }

    cvtColor(im1_gray, im1_gray, COLOR_BGR2RGB);

    QImage segimg = QImage((uchar*)im1_gray.data, im1_gray.cols, im1_gray.rows, im1_gray.step, QImage::Format_RGB888);
    segimg = segimg.scaled(gps->width(), gps->height());

    scn->addPixmap(QPixmap::fromImage(segimg));
    scn->setSceneRect(segimg.rect());
    gps->setScene(scn);

}

void OCT_Angio::read_CSV(char* csv_file)
{
    CSV_Values_Layer.clear();
    newPointsData.clear();

    ifstream inputfile(csv_file);
    string current_line;

    while (getline(inputfile, current_line))
    {
        vector<int> values;
        stringstream temp(current_line);
        string single_value;

        while (getline(temp, single_value, ','))
        {
            values.push_back(atoi(single_value.c_str()));
        }

        CSV_Values_Layer.push_back(values);
        newPointsData.push_back(values);
    }
}

//void OCT_Angio::read_CSV(char* csv_file)
//{
//    QFile thicknessCSVFileName(csv_file);
//    thicknessCSVFileName.open(QIODevice::ReadOnly | QIODevice::Text);
//    QTextStream textStream(&thicknessCSVFileName);
//    QString row;
//    QStringList thicknessValueList;
//    int count = 0;
//    int rowNumber = 0;
//
//    while (!textStream.atEnd())
//    {
//        row = textStream.readLine();
//        thicknessValueList = row.split(",");
//
//        for (int i = 0; i < 500; i++)
//        {
//            QString s = QString(thicknessValueList.at(i));
//            CSV_Values_Layer[count][i] = s.toInt();
//            newPointsData[count][i] = s.toInt();
//        }
//
//        count++;
//    }
//}

void OCT_Angio::layerColor(int AnalyzeLayer)
{
    switch (AnalyzeLayer)
    {
    case 0:
    {
        R = 255;
        B = 0;
        G = 0;
        break;
    }
    case 1:
    {
        R = 255;
        G = 0;
        B = 255;
        break;
    }
    case 2:
    {
        R = 0;
        G = 255;
        B = 0;
        break;
    }
    case 3:
    {
        R = 128;
        G = 255;
        B = 255;
        break;
    }
    case 4:
    {
        R = 192;
        G = 128;
        G = 255;
        break;
    }
    case 5:
    {
        R = 255;
        G = 255;
        B = 128;
        break;
    }
    case 6:
    {
        R = 128;
        G = 255;
        B = 64;
        break;
    }
    case 7:
    {
        R = 0;
        G = 0;
        B = 255;
        break;
    }
    default:

        break;
    }
}

void OCT_Angio::validatePixMap(QLabel* lbl, Mat image)
{
    QImage img = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8);
    img = img.scaled(lbl->size());
    lbl->setPixmap(QPixmap::fromImage(img));
}

//Edit Layer Process
void OCT_Angio::editLayers(QGraphicsView* gps, int n)
{
    Mat image_seg;

    item = new QGraphicsPixmapItem();
    scene = new graphicsscene();
    //scene->clear();

    sprintf(imageLoad, "%s//%04d.tif", CIDir, n);
    image_seg = imread(imageLoad);

    sprintf(fileName, "%s/Data%04d.csv", CSVDir, n);
    read_CSV(fileName);
    
    image_seg.convertTo(image_seg, CV_8U, contrast_val, bright_val);

    if (ui.stackWidget_octa->currentIndex() == 1)
    {
        if (ui.radioButton->isChecked())
        {
            cvtColor(image_seg, image_seg, COLOR_BGR2GRAY);
        }
        else if (ui.radioButton_2->isChecked())
        {
            bitwise_not(image_seg, image_seg);
        }
        else if (ui.radioButton_3->isChecked())
        {
            applyColorMap(image_seg, image_seg, COLORMAP_JET);
        }

        if (ui.radioButton->isChecked() == false)
        {
            cvtColor(image_seg, image_seg, COLOR_BGR2RGB);
        }
        else
        {
            cvtColor(image_seg, image_seg, COLOR_GRAY2RGB);
        }

        QImage editimg = QImage((uchar*)image_seg.data, image_seg.cols, image_seg.rows, image_seg.step, QImage::Format_RGB888);

        item->setPixmap(QPixmap::fromImage(editimg));
        scene->addItem(item);

        if (ui.checkBox_seg->isChecked() || edit == true)
        {
            scene->graphicsLineItem();
        }

        if (edit == true)
        {
            scene->HighlightLayer(value);
        }
        
        gps->setScene(scene);

        if (ui.radioButton_4->isChecked())
        {
            /*QMatrix mat;
            mat.scale(1.9, 1.5);
            gps->setMatrix(mat);*/
            QRectF bounds = scene->itemsBoundingRect();
            bounds.setWidth(bounds.width() * 1);
            bounds.setHeight(bounds.height() * 0.8);
            //gps->setAlignment(Qt::AlignCenter);
            gps->fitInView(bounds);
            //gps->centerOn(20, 20);

        }
        else if (ui.radioButton_5->isChecked())
        {
            gps->fitInView(scene->sceneRect());
        }
        else if (ui.radioButton_6->isChecked())
        {
            //QMatrix mat;
            //mat.scale(1, 1);
            //gps->setMatrix(mat);
            QRectF bounds = scene->itemsBoundingRect();
            bounds.setWidth(bounds.width() * 1);
            bounds.setHeight(bounds.height() * 1);
            gps->fitInView(bounds, Qt::KeepAspectRatio);
        }
        else
        {
            gps->fitInView(scene->sceneRect());
        }
    }

    else
    {
        if (ui.radioButton_10->isChecked())
        {
            cvtColor(image_seg, image_seg, COLOR_BGR2GRAY);
        }
        else if (ui.radioButton_11->isChecked())
        {
            bitwise_not(image_seg, image_seg);
        }
        else if (ui.radioButton_12->isChecked())
        {
            applyColorMap(image_seg, image_seg, COLORMAP_JET);
        }

        if (ui.radioButton_10->isChecked() == false)
        {
            cvtColor(image_seg, image_seg, COLOR_BGR2RGB);
        }
        else
        {
            cvtColor(image_seg, image_seg, COLOR_GRAY2RGB);
        }

        QImage editimg = QImage((uchar*)image_seg.data, image_seg.cols, image_seg.rows, image_seg.step, QImage::Format_RGB888);

        item->setPixmap(QPixmap::fromImage(editimg));
        scene->addItem(item);

        if (ui.checkBox_seg_2->isChecked() || edit == true)
        {
            scene->graphicsLineItem();
        }

        if (edit == true)
        {
            scene->HighlightLayer(value);
        }

        gps->setScene(scene);

        if (ui.radioButton_7->isChecked())
        {
            QMatrix mat;
            mat.scale(1.9, 1.5);
            gps->setMatrix(mat);
        }
        else if (ui.radioButton_8->isChecked())
        {
            gps->fitInView(scene->sceneRect());
        }
        else if (ui.radioButton_9->isChecked())
        {
            QMatrix mat;
            mat.scale(1, 1);
            gps->setMatrix(mat);
        }
        else
        {
            gps->fitInView(scene->sceneRect());
        }
    }
}

void graphicsscene::graphicsLineItem()
{
    vect1.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem1 = new QGraphicsLineItem(i, CSV_Values_Layer[0][i], (i + 25), CSV_Values_Layer[0][(i + 25)]);
        else
            lineItem1 = new  QGraphicsLineItem(i, CSV_Values_Layer[0][i], (i + 25), CSV_Values_Layer[0][(i + 24)]);
        lineItem1->setPen(QPen(QColor(255, 0, 0), 1, Qt::SolidLine));
        lineItem1->setVisible(true);
        lineItem1->setData(i, "LineBoundary0");
        scene->addItem(lineItem1);
        vect1.append(lineItem1);
    }

    vect2.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem2 = new QGraphicsLineItem(i, CSV_Values_Layer[1][i], (i + 25), CSV_Values_Layer[1][(i + 25)]);
        else
            lineItem2 = new  QGraphicsLineItem(i, CSV_Values_Layer[1][i], (i + 25), CSV_Values_Layer[1][(i + 24)]);
        lineItem2->setPen(QPen(QColor(255, 0, 255), 1, Qt::SolidLine));
        lineItem2->setVisible(true);
        lineItem2->setData(i, "LineBoundary1");
        scene->addItem(lineItem2);
        vect2.append(lineItem2);
    }

    vect3.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem3 = new QGraphicsLineItem(i, CSV_Values_Layer[2][i], (i + 25), CSV_Values_Layer[2][(i + 25)]);
        else
            lineItem3 = new  QGraphicsLineItem(i, CSV_Values_Layer[2][i], (i + 25), CSV_Values_Layer[2][(i + 24)]);
        lineItem3->setPen(QPen(QColor(0, 255, 0), 1, Qt::SolidLine));
        lineItem3->setData(i, "LineBoundary2");
        scene->addItem(lineItem3);
        vect3.append(lineItem3);
    }

    vect4.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem4 = new QGraphicsLineItem(i, CSV_Values_Layer[3][i], (i + 25), CSV_Values_Layer[3][(i + 25)]);
        else
            lineItem4 = new  QGraphicsLineItem(i, CSV_Values_Layer[3][i], (i + 25), CSV_Values_Layer[3][(i + 24)]);
        lineItem4->setPen(QPen(QColor(128, 255, 255), 1, Qt::SolidLine));
        lineItem4->setData(i, "LineBoundary3");
        scene->addItem(lineItem4);
        vect4.append(lineItem4);
    }

    vect5.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem5 = new QGraphicsLineItem(i, CSV_Values_Layer[4][i], (i + 25), CSV_Values_Layer[4][(i + 25)]);
        else
            lineItem5 = new  QGraphicsLineItem(i, CSV_Values_Layer[4][i], (i + 25), CSV_Values_Layer[4][(i + 24)]);
        lineItem5->setPen(QPen(QColor(192, 128, 255), 1, Qt::SolidLine));
        lineItem5->setData(i, "LineBoundary4");
        scene->addItem(lineItem5);
        vect5.append(lineItem5);
    }

    vect6.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem6 = new QGraphicsLineItem(i, CSV_Values_Layer[5][i], (i + 25), CSV_Values_Layer[5][(i + 25)]);
        else
            lineItem6 = new  QGraphicsLineItem(i, CSV_Values_Layer[5][i], (i + 25), CSV_Values_Layer[5][(i + 24)]);
        lineItem6->setPen(QPen(QColor(255, 255, 128), 1, Qt::SolidLine));
        lineItem6->setData(i, "LineBoundary5");
        scene->addItem(lineItem6);
        vect6.append(lineItem6);
    }

    vect7.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem7 = new QGraphicsLineItem(i, CSV_Values_Layer[6][i], (i + 25), CSV_Values_Layer[6][(i + 25)]);
        else
            lineItem7 = new  QGraphicsLineItem(i, CSV_Values_Layer[6][i], (i + 25), CSV_Values_Layer[6][(i + 24)]);
        lineItem7->setPen(QPen(QColor(128, 255, 64), 1, Qt::SolidLine));
        lineItem7->setData(i, "LineBoundary6");
        scene->addItem(lineItem7);
        vect7.append(lineItem7);
    }

    vect8.clear();
    for (int i = 0; i < 500; i += 25)
    {
        if ((i + 25) < 475)
            lineItem8 = new QGraphicsLineItem(i, CSV_Values_Layer[7][i], (i + 25), CSV_Values_Layer[7][(i + 25)]);
        else
            lineItem8 = new  QGraphicsLineItem(i, CSV_Values_Layer[7][i], (i + 25), CSV_Values_Layer[7][(i + 24)]);
        lineItem8->setPen(QPen(QColor(0, 0, 255), 1, Qt::SolidLine));
        lineItem8->setData(i, "LineBoundary7");
        scene->addItem(lineItem8);
        vect8.append(lineItem8);
    }
}

void OCT_Angio::onGroupButtonClicked(int val)
{
    scene->removeEllipse(value);
    scene->HighlightLayer(val);
}

void graphicsscene::HighlightLayer(int i1)
{
    value = i1;

    switch (i1)
    {
    case 0:
        ellipse1.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem1 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem1->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem1->setBrush(QBrush(QColor(255, 0, 0), Qt::SolidPattern));
            EllipseItem1->setVisible(true);
            EllipseItem1->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem1->setData(i, i1);
            scene->addItem(EllipseItem1);
            ellipse1.append(EllipseItem1);
        }
        break;

    case 1:
        ellipse2.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem2 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem2->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem2->setBrush(QBrush(QColor(255, 0, 255), Qt::SolidPattern));
            EllipseItem2->setVisible(true);
            EllipseItem2->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem2->setData(i, i1);
            scene->addItem(EllipseItem2);
            ellipse2.append(EllipseItem2);
        }
        break;

    case 2:
        ellipse3.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem3 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem3->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem3->setBrush(QBrush(QColor(0, 255, 0), Qt::SolidPattern));
            EllipseItem3->setVisible(true);
            EllipseItem3->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem3->setData(i, i1);
            scene->addItem(EllipseItem3);
            ellipse3.append(EllipseItem3);
        }
        break;

    case 3:
        ellipse4.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem4 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem4->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem4->setBrush(QBrush(QColor(128, 255, 255), Qt::SolidPattern));
            EllipseItem4->setVisible(true);
            EllipseItem4->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem4->setData(i, i1);
            scene->addItem(EllipseItem4);
            ellipse4.append(EllipseItem4);
        }
        break;

    case 4:
        ellipse5.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem5 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem5->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem5->setBrush(QBrush(QColor(192, 128, 255), Qt::SolidPattern));
            EllipseItem5->setVisible(true);
            EllipseItem5->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem5->setData(i, i1);
            scene->addItem(EllipseItem5);
            ellipse5.append(EllipseItem5);
        }
        break;

    case 5:
        ellipse6.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem6 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem6->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem6->setBrush(QBrush(QColor(255, 255, 128), Qt::SolidPattern));
            EllipseItem6->setVisible(true);
            EllipseItem6->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem6->setData(i, i1);
            scene->addItem(EllipseItem6);
            ellipse6.append(EllipseItem6);
        }
        break;

    case 6:
        ellipse7.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem7 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem7->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem7->setBrush(QBrush(QColor(128, 255, 64), Qt::SolidPattern));
            EllipseItem7->setVisible(true);
            EllipseItem7->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem7->setData(i, i1);
            scene->addItem(EllipseItem7);
            ellipse7.append(EllipseItem7);
        }
        break;

    case 7:
        ellipse8.clear();
        for (int i = 0; i < 500; i += 25)
        {
            EllipseItem8 = new QGraphicsEllipseItem(i - 5, CSV_Values_Layer[i1][i] - 5, 10, 10);
            EllipseItem8->setPen(QPen(QColor(242, 235, 226), 1.5, Qt::SolidLine));
            EllipseItem8->setBrush(QBrush(QColor(0, 0, 255), Qt::SolidPattern));
            EllipseItem8->setVisible(true);
            EllipseItem8->setFlags(QGraphicsEllipseItem::ItemIsSelectable | QGraphicsEllipseItem::ItemIsMovable);
            EllipseItem8->setData(i, i1);
            scene->addItem(EllipseItem8);
            ellipse8.append(EllipseItem8);
        }
        break;
    }
}

void graphicsscene::removeEllipse(int val1)
{
    switch (val1)
    {
    case 0:
        for (int i = 0; i < ellipse1.count(); i++)
        {
            scene->removeItem(ellipse1[i]);
        }

        break;

    case 1:
        for (int i = 0; i < ellipse2.count(); i++)
        {
            scene->removeItem(ellipse2[i]);
        }

        break;

    case 2:
        for (int i = 0; i < ellipse3.count(); i++)
        {
            scene->removeItem(ellipse3[i]);
        }

        break;

    case 3:
        for (int i = 0; i < ellipse4.count(); i++)
        {
            scene->removeItem(ellipse4[i]);
        }

        break;

    case 4:
        for (int i = 0; i < ellipse5.count(); i++)
        {
            scene->removeItem(ellipse5[i]);
        }

        break;

    case 5:
        for (int i = 0; i < ellipse6.count(); i++)
        {
            scene->removeItem(ellipse6[i]);
        }

        break;

    case 6:
        for (int i = 0; i < ellipse7.count(); i++)
        {
            scene->removeItem(ellipse7[i]);
        }

        break;

    case 7:
        for (int i = 0; i < ellipse8.count(); i++)
        {
            scene->removeItem(ellipse8[i]);
        }

        break;

    default:

        break;
    }
}

//Mouse Move Event Process
void graphicsscene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF mousePosition = event->scenePos();
    QGraphicsItem* pItem = QGraphicsScene::itemAt(QPointF(mousePosition.x(), mousePosition.y()), QTransform());

    if (pItem == NULL)
    {
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }

    if (pItem->isSelected() == false)
    {
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }

    QList<QGraphicsItem*> items = selectedItems();
    
    for (int i = 0; i < items.count(); i++)
    {
        for (int j = 0; j < 500; j += 25)
        {
            if (!items[i]->data(j).toString().isEmpty())
            {
                //qDebug() << " j : " << j << items[i]->data(j).toString();
                int x = items[i]->boundingRect().x() + items[i]->pos().x();
                int y = items[i]->boundingRect().y() + items[i]->pos().y();

                if (items[i]->data(j) == 0)
                    //   if(items[i] == 0)
                {
                    items[i]->setPos(items[i]->pos());
                    //qDebug() << "items" << endl;
                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[0][j];
                    int l1x2 = vect1[j / 25]->line().x2();
                    int l1y2 = vect1[j / 25]->line().y2();

                    // qDebug() << "j : " << l1x1 << l1y1 << l1x2 << l1y2;

                    int l2x1 = vect1[(j - 1) / 25]->line().x1();
                    int l2y1 = vect1[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[0][j];
                    //qDebug() << "j1 : " << l2x1;

                    vect1[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect1[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);

                    newPointsData[0][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                else if (items[i]->data(j) == 1)
                {
                    //cout << "Second Layer" << endl;
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[1][j];
                    int l1x2 = vect2[j / 25]->line().x2();
                    int l1y2 = vect2[j / 25]->line().y2();

                    int l2x1 = vect2[(j - 1) / 25]->line().x1();
                    int l2y1 = vect2[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[1][j];

                    vect2[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect2[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[1][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                else if (items[i]->data(j) == 2)
                {
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[2][j];
                    int l1x2 = vect3[j / 25]->line().x2();
                    int l1y2 = vect3[j / 25]->line().y2();

                    int l2x1 = vect3[(j - 1) / 25]->line().x1();
                    int l2y1 = vect3[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[2][j];

                    vect3[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect3[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[2][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                else if (items[i]->data(j) == 3)
                {
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[3][j];
                    int l1x2 = vect4[j / 25]->line().x2();
                    int l1y2 = vect4[j / 25]->line().y2();

                    int l2x1 = vect4[(j - 1) / 25]->line().x1();
                    int l2y1 = vect4[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[3][j];

                    vect4[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect4[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[3][j] = l1y1 + items[i]->pos().y(); // Right Side
                }


                else if (items[i]->data(j) == 4)
                {
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[4][j];
                    int l1x2 = vect5[j / 25]->line().x2();
                    int l1y2 = vect5[j / 25]->line().y2();

                    int l2x1 = vect5[(j - 1) / 25]->line().x1();
                    int l2y1 = vect5[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[4][j];

                    vect5[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect5[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[4][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                else if (items[i]->data(j) == 5)
                {
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[5][j];
                    int l1x2 = vect6[j / 25]->line().x2();
                    int l1y2 = vect6[j / 25]->line().y2();

                    int l2x1 = vect6[(j - 1) / 25]->line().x1();
                    int l2y1 = vect6[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[5][j];

                    vect6[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect6[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[5][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                else if (items[i]->data(j) == 6)
                {
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[6][j];
                    int l1x2 = vect7[j / 25]->line().x2();
                    int l1y2 = vect7[j / 25]->line().y2();

                    int l2x1 = vect7[(j - 1) / 25]->line().x1();
                    int l2y1 = vect7[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[6][j];

                    vect7[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect7[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[6][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                else if (items[i]->data(j) == 7)
                {
                    //cout << "Last Layer" << endl;
                    items[i]->setPos(items[i]->pos());

                    int l1x1 = j;
                    int l1y1 = CSV_Values_Layer[7][j];
                    int l1x2 = vect8[j / 25]->line().x2();
                    int l1y2 = vect8[j / 25]->line().y2();

                    int l2x1 = vect8[(j - 1) / 25]->line().x1();
                    int l2y1 = vect8[(j - 1) / 25]->line().y1();
                    int l2x2 = j;
                    int l2y2 = CSV_Values_Layer[7][j];

                    vect8[(j - 1) / 25]->setLine(l2x1, l2y1, l2x2 + items[i]->pos().x(), l2y2 + items[i]->pos().y());
                    vect8[j / 25]->setLine(l1x1 + items[i]->pos().x(), l1y1 + items[i]->pos().y(), l1x2, l1y2);
                    newPointsData[7][j] = l1y1 + items[i]->pos().y(); // Right Side
                }

                QGraphicsScene::mouseMoveEvent(event);
            }
        }
    }
}

//Mouse Event Process
bool OCT_Angio::eventFilter(QObject* obj, QEvent* ev)
{
    QWheelEvent* wt = static_cast <QWheelEvent*> (ev);

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView)
    {
        if (wt->delta() > 0)
        {
            if (imageID_1 < 128)
            {
                x = x + 2;
                imageID_1++;
            }
        }
        else
        {
            if (imageID_1 > 1)
            {
                imageID_1--;
                x = x - 2;
            }
        }
        segmentation(ui.graphicsView, Scene_1, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView_2)
    {
        if (wt->delta() > 0)
        {
            if (imageID_2 < 128)
                y = y + 2;
            imageID_2++;
        }
        else
        {
            if (imageID_2 > 1)
                imageID_2--;
            y = y - 2;
        }
        segmentation(ui.graphicsView_2, Scene_2, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.h_scroll_graphicsview)
    {
        if (wt->delta() > 0)
        {
            if (imageID_1 < 128)
            {
                imageID_1++;
                x = x + 2;
            }
        }
        else
        {
            if (imageID_1 > 1)
            {
                imageID_1--;
                x = x - 2;
            }
        }
        editLayers(ui.h_scroll_graphicsview, imageID_1);
        drawLines(ui.graphicsView_3, Scene_4, x);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.h_scroll_graphicsview_2)
    {
        if (wt->delta() > 0)
        {
            if (imageID_2 < 128)
            {
                imageID_2++;
                y = y + 2;
            }
        }
        else
        {
            if (imageID_2 > 1)
            {
                imageID_2--;
                y = y - 2;
            }
        }
        editLayers(ui.h_scroll_graphicsview_2, imageID_2);
        drawLines(ui.graphicsView_4, Scene_4, y);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView_3)
    {
        if (wt->delta() > 0)
        {
            if (x < 254)
            {
                x = x + 2;
                imageID_1++;
            }
        }
        else
        {
            if (x > 2)
            {
                x = x - 2;
                imageID_1--;
            }
        }
        drawLines(ui.graphicsView_3, Scene_4, x);
        editLayers(ui.h_scroll_graphicsview, imageID_1);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView_4)
    {
        if (wt->delta() > 0)
        {
            if (y < 254)
            {
                y = y + 2;
                imageID_2++;
            }
        }
        else
        {
            if (y > 2)
            {
                y = y - 2;
                imageID_2--;
            }
        }
        drawLines(ui.graphicsView_4, Scene_4, y);
        editLayers(ui.h_scroll_graphicsview_2, imageID_2);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView_5)
    {
        if (wt->delta() > 0)
        {
            if (imageID_1 < 128)
            {
                x = x + 2;
                imageID_1++;
            }
        }
        else
        {
            if (imageID_1 > 1)
            {
                imageID_1--;
                x = x - 2;
            }
        }
        segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
        drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView_7)
    {
        if (wt->delta() > 0)
        {
            if (imageID_2 < 128)
            {
                y = y + 2;
                imageID_2++;
            }
        }
        else
        {
            if (imageID_2 > 1)
            {
                imageID_2--;
                y = y - 2;
            }
        }
        segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
        drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.h_scroll_graphicsview_3)
    {
        if (wt->delta() > 0)
        {
            if (x < 254)
            {
                x = x + 2;
                imageID_1++;
            }
        }
        else
        {
            if (x > 2)
            {
                imageID_1--;
                x = x - 2;
            }
        }

        segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
        drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.h_scroll_graphicsview_6)
    {
        if (wt->delta() > 0)
        {
            if (y < 254)
            {
                y = y + 2;
                imageID_2++;
            }
        }
        else
        {
            if (y > 2)
            {
                imageID_2--;
                y = y - 2;
            }
        }

        segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
        drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
    }

    if (ev->type() == QEvent::Wheel && obj == ui.graphicsView_6)
    {
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            if (wt->delta() > 0)
            {
                if (imageID_1 < 128)
                {
                    x = x + 2;
                    imageID_1++;
                }
            }
            else
            {
                if (imageID_1 > 1)
                {
                    imageID_1--;
                    x = x - 2;
                }
            }

            segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
            drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
        }

        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            if (wt->delta() > 0)
            {
                if (imageID_2 < 128)
                {
                    y = y + 2;
                    imageID_2++;
                }
            }
            else
            {
                if (imageID_2 > 1)
                {
                    imageID_2--;
                    y = y - 2;
                }
            }

            segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
            drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
        }
    }

    if (ev->type() == QEvent::Wheel && obj == ui.h_scroll_graphicsview_4)
    {
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            if (wt->delta() > 0)
            {
                if (x < 254)
                {
                    x = x + 2;
                    imageID_1++;
                }
            }
            else
            {
                if (x > 2)
                {
                    imageID_1--;
                    x = x - 2;
                }
            }

            segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
            drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
        }
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            if (wt->delta() > 0)
            {
                if (y < 254)
                {
                    y = y + 2;
                    imageID_2++;
                }
            }
            else
            {
                if (y > 2)
                {
                    imageID_2--;
                    y = y - 2;
                }
            }

            segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
            drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
        }
    }

    if (ev->type() == QEvent::MouseButtonDblClick && obj == ui.scroll_octa_single_h_view)
    {
        ui.stackWidget_octa->setCurrentIndex(1);

        ui.horizontalSlider_bright->setSliderPosition(20);
        ui.horizontalSlider_contrast->setSliderPosition(10);

        Scene_4->clear();

        editLayers(ui.h_scroll_graphicsview, imageID_1);
        drawLines(ui.graphicsView_3, Scene_4, x);
    }

    if (ev->type() == QEvent::MouseButtonDblClick && obj == ui.lbl_octa_single_capillary)
    {
        ui.stackWidget_octa->setCurrentIndex(2);

        ui.comboBox->setCurrentIndex(idx1_1);
        ui.comboBox_2->setCurrentIndex(idx1_2);

        ui.spinBox->setValue(m1_1);
        ui.spinBox_2->setValue(m1_2);

        ui.horizontalSlider_bright_3->setSliderPosition(2);
        ui.horizontalSlider_contrast_3->setSliderPosition(10);

        Scene_3->clear();
        Scene_4->clear();

        enface = EnfaceAngio_1[0];
        drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
        segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
    }

    if (ev->type() == QEvent::MouseButtonDblClick && obj == ui.lbl_octa_single)
    {
        ui.stackWidget_octa->setCurrentIndex(3);

        Scene_3->clear();
        Scene_4->clear();

        QImage img;

        switch (ui.on_combo_box_octa_single_image->currentIndex())
        {
        case 0:
        {
            img = QImage((uchar*)EnfaceAngio_1[0].data, EnfaceAngio_1[0].cols, EnfaceAngio_1[0].rows, EnfaceAngio_1[0].step, QImage::Format_Grayscale8);

            break;
        }
        case 1:
        {
            img = QImage((uchar*)EnfaceAngio_2[0].data, EnfaceAngio_2[0].cols, EnfaceAngio_2[0].rows, EnfaceAngio_2[0].step, QImage::Format_Grayscale8);

            break;
        }
        case 2:
        {
            img = QImage((uchar*)EnfaceReg_1[0].data, EnfaceReg_1[0].cols, EnfaceReg_1[0].rows, EnfaceReg_1[0].step, QImage::Format_Grayscale8);

            break;
        }
        case 3:
        {
            img = QImage((uchar*)EnfaceReg_2[0].data, EnfaceReg_2[0].cols, EnfaceReg_2[0].rows, EnfaceReg_2[0].step, QImage::Format_Grayscale8);

            break;
        }
        default:

            break;
        }
        QPixmap pix = QPixmap::fromImage(img);
        Scene_3->addPixmap(pix.scaled(ui.h_scroll_graphicsview_5->width(), ui.h_scroll_graphicsview_5->height()));

        ui.h_scroll_graphicsview_5->setScene(Scene_3);
    }

    if (ev->type() == QEvent::MouseButtonDblClick && obj == ui.lbl_octa_single_enface)
    {
        Scene_3->clear();
        Scene_4->clear();

        ui.stackWidget_octa->setCurrentIndex(4);

        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            RegEnface = EnfaceReg_1[0];

            ui.comboBox_4->setCurrentIndex(idx1_1);
            ui.comboBox_3->setCurrentIndex(idx1_2);

            ui.spinBox_3->setValue(m1_1);
            ui.spinBox_8->setValue(m1_2);

            segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);
            drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
        }
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            RegEnface = EnfaceReg_2[0];

            ui.comboBox_4->setCurrentIndex(idx2_1);
            ui.comboBox_3->setCurrentIndex(idx2_2);

            ui.spinBox_3->setValue(m2_1);
            ui.spinBox_8->setValue(m2_2);

            segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
            drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
        }
    }

    if (ev->type() == QEvent::MouseButtonDblClick && obj == ui.lbl_octa_single_superficial)
    {
        ui.stackWidget_octa->setCurrentIndex(5);

        ui.comboBox_5->setCurrentIndex(idx2_1);
        ui.comboBox_6->setCurrentIndex(idx2_2);

        ui.spinBox_9->setValue(m2_1);
        ui.spinBox_10->setValue(m2_2);

        ui.horizontalSlider_bright_4->setSliderPosition(2);
        ui.horizontalSlider_contrast_4->setSliderPosition(10);

        Scene_3->clear();
        Scene_4->clear();

        enface = EnfaceAngio_2[0];
        drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
        segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);
    }

    if (ev->type() == QEvent::MouseButtonDblClick && obj == ui.scroll_octa_single_v_view)
    {
        ui.stackWidget_octa->setCurrentIndex(6);

        ui.horizontalSlider_bright_2->setSliderPosition(20);
        ui.horizontalSlider_contrast_2->setSliderPosition(10);

        Scene_4->clear();

        editLayers(ui.h_scroll_graphicsview_2, imageID_2);
        drawLines(ui.graphicsView_4, Scene_4, y);
    }
    return false;
}

//Page_0 Widget Components
void OCT_Angio::on_pushButton_2_clicked()
{
    qDebug() << "Denoising";

    validatePixMap(ui.lbl_octa_denoising_1, EnfaceAngio_1[1]);
    validatePixMap(ui.lbl_octa_denoising_2, EnfaceAngio_2[1]);
}

void OCT_Angio::on_on_combo_box_octa_single_image_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        validatePixMap(ui.lbl_octa_single, EnfaceAngio_1[0]);

        break;
    }
    case 1:
    {
        validatePixMap(ui.lbl_octa_single, EnfaceAngio_2[0]);

        break;
    }
    case 2:
    {
        validatePixMap(ui.lbl_octa_single, EnfaceReg_1[0]);

        break;
    }
    case 3:
    {
        validatePixMap(ui.lbl_octa_single, EnfaceReg_2[0]);

        break;
    }
    default:

        break;
    }
}

void OCT_Angio::on_on_combo_box_octa_single_enface_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        validatePixMap(ui.lbl_octa_single_enface, EnfaceReg_1[0]);

        break;
    }
    case 1:
    {
        validatePixMap(ui.lbl_octa_single_enface, EnfaceReg_2[0]);

        break;
    }
    default:

        break;
    }
}

void OCT_Angio::on_on_combo_box_octa_single_capillary_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        idx1_1 = 0, idx1_2 = 2;
        ui.comboBox_12->setCurrentIndex(idx1_1);
        ui.comboBox_13->setCurrentIndex(idx1_2);
        break;
    }
    case 1:
    {
        idx1_1 = 2, idx1_2 = 3;
        ui.comboBox_12->setCurrentIndex(idx1_1);
        ui.comboBox_13->setCurrentIndex(idx1_2);

        break;
    }
    case 2:
    {
        idx1_1 = 4, idx1_2 = 6;
        ui.comboBox_12->setCurrentIndex(idx1_1);
        ui.comboBox_13->setCurrentIndex(idx1_2);

        break;
    }
    case 3:
    {
        idx1_1 = 4, idx1_2 = 7;
        ui.comboBox_12->setCurrentIndex(idx1_1);
        ui.comboBox_13->setCurrentIndex(idx1_2);

        break;
    }
    default:

        break;
    }
}

void OCT_Angio::on_on_combo_box_octa_single_superficial_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        idx2_1 = 0, idx2_2 = 2;
        ui.comboBox_15->setCurrentIndex(idx2_1);
        ui.comboBox_14->setCurrentIndex(idx2_2);

        break;
    }
    case 1:
    {
        idx2_1 = 2, idx2_2 = 3;
        ui.comboBox_15->setCurrentIndex(idx2_1);
        ui.comboBox_14->setCurrentIndex(idx2_2);

        break;
    }
    case 2:
    {
        idx2_1 = 4, idx2_2 = 6;
        ui.comboBox_15->setCurrentIndex(idx2_1);
        ui.comboBox_14->setCurrentIndex(idx2_2);

        break;
    }
    case 3:
    {
        idx2_1 = 4, idx2_2 = 7;
        ui.comboBox_15->setCurrentIndex(idx2_1);
        ui.comboBox_14->setCurrentIndex(idx2_2);

        break;
    }
    default:

        break;
    }
}


void OCT_Angio::on_comboBox_12_currentIndexChanged(int index1_1)
{
    idx1_1 = index1_1;
    segmentation(ui.graphicsView, Scene_1, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    validatePixMap(ui.lbl_octa_single_capillary, EnfaceAngio_1[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_comboBox_13_currentIndexChanged(int index1_2)
{
    idx1_2 = index1_2;
    segmentation(ui.graphicsView, Scene_1, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    validatePixMap(ui.lbl_octa_single_capillary, EnfaceAngio_1[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_comboBox_15_currentIndexChanged(int index2_1)
{
    idx2_1 = index2_1;
    segmentation(ui.graphicsView_2, Scene_2, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    validatePixMap(ui.lbl_octa_single_superficial, EnfaceAngio_2[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_comboBox_14_currentIndexChanged(int index2_2)
{
    idx2_2 = index2_2;
    segmentation(ui.graphicsView_2, Scene_2, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    validatePixMap(ui.lbl_octa_single_superficial, EnfaceAngio_2[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_spinBox_4_valueChanged(int i1_1)
{
    m1_1 = i1_1;

    segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    validatePixMap(ui.lbl_octa_single_capillary, EnfaceAngio_1[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_spinBox_5_valueChanged(int i1_2)
{
    m1_2 = i1_2;

    segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    validatePixMap(ui.lbl_octa_single_capillary, EnfaceAngio_1[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_spinBox_7_valueChanged(int i2_1)
{
    m2_1 = i2_1;

    segmentation(ui.graphicsView_2, Scene_2, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    validatePixMap(ui.lbl_octa_single_superficial, EnfaceAngio_2[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

void OCT_Angio::on_spinBox_6_valueChanged(int i2_2)
{
    m2_2 = i2_2;

    segmentation(ui.graphicsView_2, Scene_2, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    validatePixMap(ui.lbl_octa_single_superficial, EnfaceAngio_2[0]);
    on_on_combo_box_octa_single_image_currentIndexChanged(ui.on_combo_box_octa_single_image->currentIndex());
    on_on_combo_box_octa_single_enface_currentIndexChanged(ui.on_combo_box_octa_single_enface->currentIndex());
}

//Page_1 Widget Components
void OCT_Angio::on_checkBox_seg_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_btn_Layer_report_clicked()
{
    ui.groupBox_4->setGeometry(QRect(1470, 225, 311, 361));
    ui.groupBox_4->show();

    ui.radioButton_29->setChecked(true);

    edit = true;

    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_Reset_layer_2_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_h_scroll_macula_single_layer_ok_2_clicked()
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file);
    QString separator(",");

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < newPointsData[j].size(); i++)
        {
            stream << newPointsData[j].at(i) << ",";
        }
        stream << "\n";
    }

    stream.flush();
    file.close();

    if (group_1->checkedButton())
    {
        group_1->setExclusive(false);
        group_1->checkedButton()->setChecked(false);
        group_1->setExclusive(true);
    }

    ui.groupBox_4->close();

    edit = false;
    scene->clear();
    value = 0;

    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_h_scroll_macula_single_layer_cancel_2_clicked()
{
    value = 0;

    if (group_1->checkedButton())
    {
        group_1->setExclusive(false);
        group_1->checkedButton()->setChecked(false);
        group_1->setExclusive(true);
    }

    ui.groupBox_4->close();

    edit = false;
    scene->clear();

    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_reset_bright_clicked()
{
    ui.horizontalSlider_bright->setSliderPosition(20);
    //editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_reset_contrast_clicked()
{
    ui.horizontalSlider_contrast->setSliderPosition(10);
    //editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_btn_close_measure_clicked()
{
    if (edit == true)
    {
        on_h_scroll_macula_single_layer_cancel_2_clicked();
    }

    on_comboBox_12_currentIndexChanged(idx1_1);
    ui.stackWidget_octa->setCurrentIndex(0);
}

void OCT_Angio::on_horizontalSlider_bright_valueChanged(int value)
{
    //qDebug() << "Brightness";
    bright_val = value;
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_horizontalSlider_contrast_valueChanged(int value)
{
    //qDebug() << "Contrast";
    contrast_val = (float)value / 10;
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_radioButton_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_radioButton_2_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_radioButton_3_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_radioButton_4_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_radioButton_5_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

void OCT_Angio::on_radioButton_6_clicked()
{
    editLayers(ui.h_scroll_graphicsview, imageID_1);
}

//Page_2 Widget Components
void OCT_Angio::on_checkBox_seg_3_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_checkBox_2_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_radioButton_41_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_radioButton_42_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_horizontalSlider_bright_3_valueChanged(int value)
{
    //qDebug() << "Brightness";
    bright_val_enface = value;
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_horizontalSlider_contrast_3_valueChanged(int value)
{
    //qDebug() << "Contrast";
    contrast_val_enface = (float)value / 10;
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_reset_bright_3_clicked()
{
    ui.horizontalSlider_bright_3->setSliderPosition(2);
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_reset_contrast_3_clicked()
{
    ui.horizontalSlider_contrast_3->setSliderPosition(10);
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_btn_close_measure_2_clicked()
{
    ui.stackWidget_octa->setCurrentIndex(0);

    ui.comboBox_12->setCurrentIndex(idx1_1);
    ui.comboBox_13->setCurrentIndex(idx1_2);

    ui.spinBox_4->setValue(m1_1);
    ui.spinBox_5->setValue(m1_2);
}

void OCT_Angio::on_on_combo_box_octa_single_capillary_3_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        idx1_1 = 0, idx1_2 = 2;
        ui.comboBox->setCurrentIndex(idx1_1);
        ui.comboBox_2->setCurrentIndex(idx1_2);

        break;
    }
    case 1:
    {
        idx1_1 = 2, idx1_2 = 3;
        ui.comboBox->setCurrentIndex(idx1_1);
        ui.comboBox_2->setCurrentIndex(idx1_2);

        break;
    }
    case 2:
    {
        idx1_1 = 4, idx1_2 = 6;
        ui.comboBox->setCurrentIndex(idx1_1);
        ui.comboBox_2->setCurrentIndex(idx1_2);

        break;
    }
    case 3:
    {
        idx1_1 = 4, idx1_2 = 7;
        ui.comboBox->setCurrentIndex(idx1_1);
        ui.comboBox_2->setCurrentIndex(idx1_2);

        break;
    }
    default:

        break;
    }
}

void OCT_Angio::on_comboBox_currentIndexChanged(int index1)
{
    idx1_1 = index1;
    segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    enface = EnfaceAngio_1[0];
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_comboBox_2_currentIndexChanged(int index2)
{
    idx1_2 = index2;
    segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    enface = EnfaceAngio_1[0];
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_spinBox_valueChanged(int spin_value)
{
    m1_1 = spin_value;
    segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    enface = EnfaceAngio_1[0];
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

void OCT_Angio::on_spinBox_2_valueChanged(int spin_value)
{
    m1_2 = spin_value;
    segmentation(ui.graphicsView_5, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

    EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
    EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

    enface = EnfaceAngio_1[0];
    drawLines_Enface(ui.h_scroll_graphicsview_3, Scene_3, enface, x);
}

//Page_3 Widget Components
void OCT_Angio::on_btn_close_measure_4_clicked()
{
    ui.stackWidget_octa->setCurrentIndex(0);
}

//Page_4 Widget Components
void OCT_Angio::on_checkBox_seg_4_clicked()
{
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
    {
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
    }
    else if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
    {
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
    }
}

void OCT_Angio::on_on_combo_box_octa_single_capillary_4_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            idx1_1 = 0, idx1_2 = 2;
            ui.comboBox_4->setCurrentIndex(idx1_1);
            ui.comboBox_3->setCurrentIndex(idx1_2);
        }
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            idx2_1 = 0, idx2_2 = 2;
            ui.comboBox_4->setCurrentIndex(idx2_1);
            ui.comboBox_3->setCurrentIndex(idx2_2);
        }

        break;
    }
    case 1:
    {
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            idx1_1 = 2, idx1_2 = 3;
            ui.comboBox_4->setCurrentIndex(idx1_1);
            ui.comboBox_3->setCurrentIndex(idx1_2);
        }
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            idx2_1 = 2, idx2_2 = 3;
            ui.comboBox_4->setCurrentIndex(idx2_1);
            ui.comboBox_3->setCurrentIndex(idx2_2);
        }

        break;
    }
    case 2:
    {
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            idx1_1 = 4, idx1_2 = 6;
            ui.comboBox_4->setCurrentIndex(idx1_1);
            ui.comboBox_3->setCurrentIndex(idx1_2);
        }
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            idx2_1 = 4, idx2_2 = 6;
            ui.comboBox_4->setCurrentIndex(idx2_1);
            ui.comboBox_3->setCurrentIndex(idx2_2);
        }

        break;
    }
    case 3:
    {
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
        {
            idx1_1 = 4, idx1_2 = 7;
            ui.comboBox_4->setCurrentIndex(idx1_1);
            ui.comboBox_3->setCurrentIndex(idx1_2);
        }
        if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
        {
            idx2_1 = 4, idx2_2 = 7;
            ui.comboBox_4->setCurrentIndex(idx2_1);
            ui.comboBox_3->setCurrentIndex(idx2_2);
        }

        break;
    }
    default:

        break;
    }
}

void OCT_Angio::on_comboBox_4_currentIndexChanged(int index1)
{
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
    {
        idx1_1 = index1;
        segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

        EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
        EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

        RegEnface = EnfaceReg_1[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
    }
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
    {
        idx2_1 = index1;
        segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

        EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
        EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

        RegEnface = EnfaceReg_2[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
    }
}

void OCT_Angio::on_comboBox_3_currentIndexChanged(int index2)
{
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
    {
        idx1_2 = index2;
        segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

        EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
        EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

        RegEnface = EnfaceReg_1[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
    }
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
    {
        idx2_2 = index2;
        segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

        EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
        EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

        RegEnface = EnfaceReg_2[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
    }
}

void OCT_Angio::on_spinBox_3_valueChanged(int i1)
{
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
    {
        m1_1 = i1;
        segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

        EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
        EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

        RegEnface = EnfaceReg_1[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
    }
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
    {
        m2_1 = i1;
        segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

        EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
        EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

        RegEnface = EnfaceReg_2[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
    }
}

void OCT_Angio::on_spinBox_8_valueChanged(int i2)
{
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
    {
        m1_2 = i2;
        segmentation(ui.graphicsView_6, Scene_4, idx1_1, idx1_2, imageID_1, m1_1, m1_2);

        EnfaceAngio_1 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx1_1, idx1_2, m1_1, m1_2);
        EnfaceReg_1 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx1_1, idx1_2, m1_1, m1_2);

        RegEnface = EnfaceReg_1[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, x);
    }
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
    {
        m2_2 = i2;
        segmentation(ui.graphicsView_6, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

        EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
        EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

        RegEnface = EnfaceReg_2[0];
        drawLines_Enface(ui.h_scroll_graphicsview_4, Scene_3, RegEnface, y);
    }
}

void OCT_Angio::on_btn_close_measure_3_clicked()
{
    ui.stackWidget_octa->setCurrentIndex(0);

    if (ui.on_combo_box_octa_single_enface->currentIndex() == 0)
    {
        ui.comboBox_12->setCurrentIndex(idx1_1);
        ui.comboBox_13->setCurrentIndex(idx1_2);

        ui.spinBox_4->setValue(m1_1);
        ui.spinBox_5->setValue(m1_2);
    }
    if (ui.on_combo_box_octa_single_enface->currentIndex() == 1)
    {
        ui.comboBox_15->setCurrentIndex(idx2_1);
        ui.comboBox_14->setCurrentIndex(idx2_2);

        ui.spinBox_7->setValue(m2_1);
        ui.spinBox_6->setValue(m2_2);
    }
}

//Page_5 Widget Components
void OCT_Angio::on_checkBox_seg_5_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_checkBox_5_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_radioButton_44_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, x);
}

void OCT_Angio::on_radioButton_45_clicked()
{
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, x);
}

void OCT_Angio::on_horizontalSlider_bright_4_valueChanged(int value)
{
    //qDebug() << "Brightness";
    bright_val_enface = value;
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_horizontalSlider_contrast_4_valueChanged(int value)
{
    //qDebug() << "Contrast";
    contrast_val_enface = (float)value / 10;
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_reset_bright_4_clicked()
{
    ui.horizontalSlider_bright_4->setSliderPosition(2);
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_reset_contrast_4_clicked()
{
    ui.horizontalSlider_contrast_4->setSliderPosition(10);
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_btn_close_measure_6_clicked()
{
    ui.stackWidget_octa->setCurrentIndex(0);


    ui.comboBox_15->setCurrentIndex(idx2_1);
    ui.comboBox_14->setCurrentIndex(idx2_2);

    ui.spinBox_7->setValue(m2_1);
    ui.spinBox_6->setValue(m2_2);
}

void OCT_Angio::on_on_combo_box_octa_single_capillary_5_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        idx2_1 = 0, idx2_2 = 2;
        ui.comboBox_5->setCurrentIndex(idx2_1);
        ui.comboBox_6->setCurrentIndex(idx2_2);

        break;
    }
    case 1:
    {
        idx2_1 = 2, idx2_2 = 3;
        ui.comboBox_5->setCurrentIndex(idx2_1);
        ui.comboBox_6->setCurrentIndex(idx2_2);

        break;
    }
    case 2:
    {
        idx2_1 = 4, idx2_2 = 6;
        ui.comboBox_5->setCurrentIndex(idx2_1);
        ui.comboBox_6->setCurrentIndex(idx2_2);

        break;
    }
    case 3:
    {
        idx2_1 = 4, idx2_2 = 7;
        ui.comboBox_5->setCurrentIndex(idx2_1);
        ui.comboBox_6->setCurrentIndex(idx2_2);

        break;
    }
    default:

        break;
    }
}

void OCT_Angio::on_comboBox_5_currentIndexChanged(int index1)
{
    idx2_1 = index1;
    segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    enface = EnfaceAngio_2[0];
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_comboBox_6_currentIndexChanged(int index2)
{
    idx2_2 = index2;
    segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    enface = EnfaceAngio_2[0];
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_spinBox_9_valueChanged(int spin_value)
{
    m2_1 = spin_value;
    segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    enface = EnfaceAngio_2[0];
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

void OCT_Angio::on_spinBox_10_valueChanged(int spin_value)
{
    m2_2 = spin_value;
    segmentation(ui.graphicsView_7, Scene_4, idx2_1, idx2_2, imageID_2, m2_1, m2_2);

    EnfaceAngio_2 = OCT_A_LayerMapping_Filter(CSVDir, SVDir, idx2_1, idx2_2, m2_1, m2_2);
    EnfaceReg_2 = OCT_A_LayerMapping_Filter(CSVDir, RegDir, idx2_1, idx2_2, m2_1, m2_2);

    enface = EnfaceAngio_2[0];
    drawLines_Enface(ui.h_scroll_graphicsview_6, Scene_3, enface, y);
}

//Page_6 Widget Components
void OCT_Angio::on_checkBox_seg_2_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_btn_Layer_report_2_clicked()
{
    ui.groupBox_2->setGeometry(QRect(1470, 225, 311, 361));
    ui.groupBox_2->show();

    ui.radioButton_19->setChecked(true);

    edit = true;

    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_Reset_layer_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_h_scroll_macula_single_layer_ok_clicked()
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file);
    QString separator(",");

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < newPointsData[j].size(); i++)
        {
            stream << newPointsData[j].at(i) << ",";
        }
        stream << "\n";
    }

    stream.flush();
    file.close();

    if (group_2->checkedButton())
    {
        group_2->setExclusive(false);
        group_2->checkedButton()->setChecked(false);
        group_2->setExclusive(true);
    }

    ui.groupBox_2->close();

    edit = false;
    scene->clear();
    value = 0;

    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_h_scroll_macula_single_layer_cancel_clicked()
{
    value = 0;

    if (group_2->checkedButton())
    {
        group_2->setExclusive(false);
        group_2->checkedButton()->setChecked(false);
        group_2->setExclusive(true);
    }

    ui.groupBox_2->close();

    edit = false;
    scene->clear();

    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_reset_bright_2_clicked()
{
    ui.horizontalSlider_bright_2->setSliderPosition(20);
    //editLayers(ui.h_scroll_graphicsview, imageID_2);
}

void OCT_Angio::on_reset_contrast_2_clicked()
{
    ui.horizontalSlider_contrast_2->setSliderPosition(10);
    //editLayers(ui.h_scroll_graphicsview, imageID_2);
}

void OCT_Angio::on_btn_close_measure_5_clicked()
{
    if (edit == true)
    {
        on_h_scroll_macula_single_layer_cancel_clicked();
    }

    on_comboBox_15_currentIndexChanged(idx2_1);
    ui.stackWidget_octa->setCurrentIndex(0);
}

void OCT_Angio::on_horizontalSlider_bright_2_valueChanged(int value)
{
    //qDebug() << "Brightness";
    bright_val = value;
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_horizontalSlider_contrast_2_valueChanged(int value)
{
    //qDebug() << "Contrast";
    contrast_val = (float)value / 10;
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_radioButton_10_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_radioButton_11_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_radioButton_12_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_radioButton_7_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_radioButton_8_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}

void OCT_Angio::on_radioButton_9_clicked()
{
    editLayers(ui.h_scroll_graphicsview_2, imageID_2);
}