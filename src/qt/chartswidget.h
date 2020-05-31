#ifndef CHARTSWIDGET_H
#define CHARTSWIDGET_H

#include <QWidget>
#include "qt/pwidget.h"
#include "qt/transactionfilterproxy.h"
#include "qt/transactiontablemodel.h"

#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

using namespace QtCharts;

namespace Ui {
class ChartsWidget;
}

enum ChartShowType {
    ALL,
    YEAR,
    MONTH,
    DAY
};

class ChartData {
public:
    ChartData() {}

    QMap<int, std::pair<qint64, qint64>> amountsByCache;
    qreal maxValue = 0;
    qint64 totalPiv = 0;
    qint64 totalZpiv = 0;
    QList<qreal> valuesPiv;
    QList<qreal> valueszPiv;
    QStringList xLabels;
};


class ChartsWidget : public PWidget
{
    Q_OBJECT

public:
    explicit ChartsWidget(ALQOGUI* parent);
    ~ChartsWidget();

    void loadWalletModel() override;
    void loadChart();

    void run(int type) override;
    void onError(QString error, int type) override;

private slots:
    void windowResizeEvent(QResizeEvent *event);
    void changeChartColors();
    void onChartYearChanged(const QString&);
    void onChartMonthChanged(const QString&);
    void onChartArrowClicked(bool goLeft);
    void onChartRefreshed();
    void onTxArrived(const QString& hash, const bool& isCoinStake, const bool& isCSAnyType);

private:
    Ui::ChartsWidget *ui;
    TransactionFilterProxy* filter;

    int nDisplayUnit = -1;
    bool isSync = false;

    int64_t lastRefreshTime = 0;
    std::atomic<bool> isLoading;

    // Chart
    TransactionFilterProxy* stakesFilter = nullptr;
    TransactionTableModel* txModel;
    bool isChartInitialized = false;
    QChartView *chartView = nullptr;
    QBarSeries *series = nullptr;
    QBarSet *set0 = nullptr;
    QBarSet *set1 = nullptr;

    QBarCategoryAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;

    QChart *chart = nullptr;
    bool isChartMin = false;
    ChartShowType chartShow = YEAR;
    int yearFilter = 0;
    int monthFilter = 0;
    int dayStart = 1;
    bool hasZpivStakes = false;

    ChartData* chartData = nullptr;
    bool hasStakes = false;

    void initChart();
    void showHideEmptyChart(bool show, bool loading, bool forceView = false);
    bool refreshChart();
    void tryChartRefresh();
    void updateStakeFilter();
    const QMap<int, std::pair<qint64, qint64>> getAmountBy();
    bool loadChartData(bool withMonthNames);
    void updateAxisX(const QStringList *arg = nullptr);
    void setChartShow(ChartShowType type);
    std::pair<int, int> getChartRange(QMap<int, std::pair<qint64, qint64>> amountsBy);


signals:
    /** Notify that a new transaction appeared */
    void incomingTransaction(const QString& date, int unit, const CAmount& amount, const QString& type, const QString& address);

};

#endif // CHARTSWIDGET_H
