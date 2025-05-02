#include "testcharts.h"
#include "ui_testcharts.h"
#include <QFileDialog>
#include <QTextStream>


TestCharts::TestCharts(QWidget *parent) : QWidget(parent), ui(new Ui::TestCharts){
    ui->setupUi(this);
    topCurve     = new QLineSeries();
    bottomCurve1 = new QLineSeries();
    bottomCurve2 = new QLineSeries();
    chart = new QChart();
    chart->legend()->hide();
    chart->removeAllSeries();
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartView->setChart(chart);

    QPen topCurvePen(Qt::blue);
    topCurvePen.setWidth(3);
    topCurve->setPen(topCurvePen);

    QPen bottomCurvePen(Qt::green);
    bottomCurvePen.setWidth(3);
    bottomCurve1->setPen(bottomCurvePen);
    bottomCurvePen.setStyle(Qt::DotLine);
    bottomCurve2->setPen(bottomCurvePen);

    connect(ui->pbLoadFirstCurve,  &QPushButton::clicked, this, &TestCharts::loadCurve);
    connect(ui->pbLoadSecondCurve, &QPushButton::clicked, this, &TestCharts::loadCurve);
    connect(ui->pbSwitchPosition,  &QPushButton::clicked, [this] { if ((!topCurveFileName.isEmpty()) &&
                                                                       (!bottomCurveFileName.isEmpty())){
                                                                   chart->removeSeries(topCurve);
                                                                   chart->removeSeries(bottomCurve1);
                                                                   chart->removeSeries(bottomCurve2);
                                                                   auto fileName = topCurveFileName;
                                                                   topCurveFileName = bottomCurveFileName;
                                                                   bottomCurveFileName = fileName;
                                                                   loadTopCurve(topCurveFileName);
                                                                   loadBottomCurve(bottomCurveFileName);}});
}

TestCharts::~TestCharts(){
    delete ui;
}

void TestCharts::loadCurve(void){
    if (topPolygon.isEmpty()) loadTopCurve("");
    else                      loadBottomCurve("");
}

void TestCharts::loadTopCurve(QString fileName){
    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("*.txt"));
    if (fileName.isEmpty()) return;
    topCurveFileName = fileName;
    QFile fileTopCurve(topCurveFileName);
    if (fileTopCurve.open(QIODevice::ReadOnly | QIODevice::Text)){
        topPolygon.clear();
        topCurve->clear();
        QTextStream stream(&fileTopCurve);
        auto data = stream.readLine().split("	");
        auto point = QPointF(data[0].replace(',','.').toFloat(), data[1].replace(',','.').toFloat());
        auto leftBottomPoint = point;
        minimum = point;
        maximum = point;
        topCurve->append(point);
        topPolygon << point;
        while (!stream.atEnd()){
            data = stream.readLine().split("	");
            point = QPointF(data[0].replace(',','.').toFloat(), data[1].replace(',','.').toFloat());
            if (point.y() < leftBottomPoint.y()) leftBottomPoint.setY(point.y());
            if (point.x() < minimum.x()) minimum.setX(point.x());
            if (point.y() < minimum.y()) minimum.setY(point.y());
            if (point.x() > maximum.x()) maximum.setX(point.x());
            if (point.y() > maximum.y()) maximum.setY(point.y());
            topCurve->append(point);
            topPolygon << point;
        }
        topPolygon << QPointF(point.x(), leftBottomPoint.y());
        topPolygon << leftBottomPoint;
        fileTopCurve.close();
    }
    update();
}

void TestCharts::loadBottomCurve(QString fileName){
    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("*.txt"));
    if (fileName.isEmpty()) return;
    bottomCurveFileName = fileName;
    bool dotted = false;
    QFile fileBottomCurve(bottomCurveFileName);
    if (fileBottomCurve.open(QIODevice::ReadOnly | QIODevice::Text)){
        bottomCurve1->clear();
        bottomCurve2->clear();
        QTextStream stream(&fileBottomCurve);
        Position firstPosition   = POSITION_UNKNOWN;
        Position currentPosition = POSITION_UNKNOWN;
        while (!stream.atEnd()){
            auto data = stream.readLine().split("	");
            auto point = QPointF(data[0].replace(',','.').toFloat(), data[1].replace(',','.').toFloat());
            if (point.x() < minimum.x()) minimum.setX(point.x());
            if (point.y() < minimum.y()) minimum.setY(point.y());
            if (point.x() > maximum.x()) maximum.setX(point.x());
            if (point.y() > maximum.y()) maximum.setY(point.y());
            currentPosition = topPolygon.containsPoint(point, Qt::OddEvenFill) ? POSITION_INSIDE : POSITION_OUTSIDE;

            if ((point.x() >= topPolygon.last().x()) && (firstPosition == POSITION_UNKNOWN))
                firstPosition = currentPosition;

            if (dotted) bottomCurve2->append(point);
            else        bottomCurve1->append(point);
            if ((firstPosition != POSITION_UNKNOWN) && (currentPosition != firstPosition)){
                dotted = true;
                bottomCurve2->append(point);
            }
        }
        fileBottomCurve.close();
    }
    update();
}

void TestCharts::update(){
    if (!chart->series().contains(bottomCurve1)) chart->addSeries(bottomCurve1);
    if (!chart->series().contains(bottomCurve2)) chart->addSeries(bottomCurve2);
    if (!chart->series().contains(topCurve))     chart->addSeries(topCurve);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setRange(minimum.x() * 0.99, maximum.x() * 1.01);
    chart->axes(Qt::Vertical).back()  ->setRange(minimum.y() * 0.99, maximum.y() * 1.01);
}
