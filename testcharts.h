#ifndef TESTCHARTS_H
#define TESTCHARTS_H

#include <QWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class TestCharts; }
QT_END_NAMESPACE

class TestCharts : public QWidget
{
    Q_OBJECT

public:
    TestCharts(QWidget *parent = nullptr);
    ~TestCharts();
public slots:
    void loadCurve(void);
    void loadTopCurve(QString fileName);
    void loadBottomCurve(QString fileName);
    void update(void);
private:
    enum Position {POSITION_UNKNOWN, POSITION_OUTSIDE, POSITION_INSIDE};
    Ui::TestCharts *ui;
    QChart *chart;
    QLineSeries *topCurve;
    QLineSeries *bottomCurve1;
    QLineSeries *bottomCurve2;
    QPolygonF topPolygon;
    QString topCurveFileName;
    QString bottomCurveFileName;
    QPointF minimum;
    QPointF maximum;
};
#endif // TESTCHARTS_H
