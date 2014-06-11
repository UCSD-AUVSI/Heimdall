#ifndef MAINTAB_H
#define MAINTAB_H

#include "viewcontroller.h"
#include "util.h"
#include <QWidget>
#include <QGridLayout>
#include <QMouseEvent>
#include <QLabel>

class ViewController;

/**
 * @brief The MainTab class
 *      The widget that contains the layout and all the
 *      gui stuff for a particular tab
 */
class MainTab : public QWidget {
    Q_OBJECT

public:
    MainTab(QWidget *parent = 0);

    /**
     * @brief MainTab::addColumn
     *      Adds a column to mainLayout using a QLabel
     * @param label
     *      The string to be displayed in QLabel
     */
    void addColumn(const char* label);

    /**
     * @brief MainTab::addColumn
     *      Adds a column to mainLayout using a QLabel
     *      to a specific column
     * @param label
     *      The string to be displayed in QLabel
     * @param col
     *      The column to add the QLabel to
     */
    void addColumn(const char* label, int col);

    /**
     * @brief MainTab::addRow
     *      Adds a row to mainLayout using the given
     *      list of QLabels
     * @param labels
     *      The list of QLabels to go into the row
     */
    void addRow(std::list<QLabel*> *labels);

    void setRowInfo(std::list<QLabel*> *labels, int row);

    void setGridInfo(QWidget *label, int row, int col);

    void setNames(std::list<QLabel*> *labels);

    void setLats(std::list<QLabel*> *labels);

    void setLongs(std::list<QLabel*> *labels);

    void setImgs(std::list<QLabel*> *labels);

    void setStalkerLabels(std::vector<StalkerLabels*> *labels);



    /**
     * @brief MainTab::setupAsFirstTab
     *      Sets up specifically how we would want the first
     *      tab of BigDog to look
     * @return
     *      A pointer to itself
     */
    MainTab* setupAsFirstTab();

    void setGridLayout(QGridLayout *gLayout);
    QGridLayout *getGridLayout();
    ViewController* getViewController();
    void setViewController(ViewController* vc);
//    void mousePressEvent(QMouseEvent *event);


private:
    QGridLayout *mainLayout;
    ViewController *vc;
};

#endif // MAINTAB_H
