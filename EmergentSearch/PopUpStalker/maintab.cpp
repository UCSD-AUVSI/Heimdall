#include "maintab.h"

#include <QString>
#include <QLabel>

MainTab::MainTab(QWidget *parent) : QWidget(parent),
    mainLayout(new QGridLayout(this))
{
    // set the spacing for the gridLayout
    //    mainLayout->setHorizontalSpacing(30);
    //    mainLayout->setVerticalSpacing(20);
}

/**
 * @brief MainTab::setupAsFirstTab
 *      Sets up specifically how we would want the first
 *      tab of BigDog to look
 * @return
 *      A pointer to itself
 */
MainTab* MainTab::setupAsFirstTab()
{
    // Set a default display to PopUpStalker
    this->addColumn("Name 1", 0);
    this->addColumn("Name 2");
    this->addColumn("Name 3");
    this->addColumn("Name 4");
    this->addColumn("Name 5");

    std::list<QLabel*> * lst = new std::list<QLabel*>();
    QLabel *r1 = new QLabel("Lat 1");
    QLabel *r2 = new QLabel("Lat 2");
    QLabel *r3 = new QLabel("Lat 3");
    QLabel *r4 = new QLabel("Lat 4");
    QLabel *r5 = new QLabel("Lat 5");
    lst->push_back(r1);
    lst->push_back(r2);
    lst->push_back(r3);
    lst->push_back(r4);
    lst->push_back(r5);
    this->setRowInfo(lst,1);
    lst->clear();


    r1 = new QLabel("Lon 1");
    r2 = new QLabel("Lon 2");
    r3 = new QLabel("Lon 3");
    r4 = new QLabel("Lon 4");
    r5 = new QLabel("Lon 5");
    lst->push_back(r1);
    lst->push_back(r2);
    lst->push_back(r3);
    lst->push_back(r4);
    lst->push_back(r5);
    this->setRowInfo(lst,2);
    lst->clear();


    r1 = new QLabel("Pixmap 1");
    r2 = new QLabel("Pixmap 2");
    r3 = new QLabel("Pixmap 3");
    r4 = new QLabel("Pixmap 4");
    r5 = new QLabel("Pixmap 5");
  
    lst->push_back(r1);
    lst->push_back(r2);
    lst->push_back(r3);
    lst->push_back(r4);
    lst->push_back(r5);
    this->setRowInfo(lst,3);

    delete lst;


    return this;
}

/**
 * @brief MainTab::addColumn
 *      Adds a column to mainLayout using a QLabel
 * @param label
 *      The string to be displayed in QLabel
 */
void MainTab::addColumn(const char* label)
{
    int cols = mainLayout->columnCount();
    addColumn(label, cols);
}

/**
 * @brief MainTab::addColumn
 *      Adds a column to mainLayout using a QLabel
 *      to a specific column
 * @param label
 *      The string to be displayed in QLabel
 * @param col
 *      The column to add the QLabel to
 */
void MainTab::addColumn(const char* label, int col)
{
    QLabel * newLabel = new QLabel(label);
    QFont f( "Arial", 20, QFont::Bold);
    newLabel->setFont( f);

    mainLayout -> addWidget(newLabel, 0, col);
}

/**
 * @brief MainTab::addRow
 *      Adds a row to mainLayout using the given
 *      list of QLabels
 * @param labels
 *      The list of QLabels to go into the row
 */
void MainTab::addRow(std::list<QLabel*> *labels)
{
    int rows = mainLayout->rowCount();
    if (rows < 0)
        rows = 0;
    int i = 0;
    for (std::list<QLabel*>::iterator start = labels->begin();
            start != labels->end(); start++)
    {
        mainLayout->addWidget( *start , rows, i);
        i++;
    }
}

// Used pretty much only for default screen now
void MainTab::setRowInfo(std::list<QLabel*> *labels, int row)
{
    if (row < 0)
        row = 0;

    int i = 0;
    for (std::list<QLabel*>::iterator start = labels->begin();
            start != labels->end(); start++)
    {
        mainLayout->addWidget( *start , row, i);
        i++;
    }
}

// Used pretty much only for default screen now
void MainTab::setGridInfo(QWidget *label, int row, int col)
{
    if (row < 0)
        row = 0;

    if (col < 0)
        col = 0;

    mainLayout->addWidget( label , row, col);

}

// Used pretty much only for default screen now
void MainTab::setNames(std::list<QLabel*> *labels)
{
    setRowInfo(labels, 0);
}

// Used pretty much only for default screen now
void MainTab::setLats(std::list<QLabel*> *labels)
{
    setRowInfo(labels, 1);
}

// Used pretty much only for default screen now
void MainTab::setLongs(std::list<QLabel*> *labels)
{
    setRowInfo(labels, 2);
}

// Used pretty much only for default screen now
void MainTab::setImgs(std::list<QLabel*> *labels)
{
    setRowInfo(labels, 3);
}

// Add up to 10 StalkerLabels to the gui
void MainTab::setStalkerLabels(std::vector<StalkerLabels*> *labels)
{
    int i = 0;
    std::vector<StalkerLabels*>::iterator it;
    for (it = labels->begin();
            it != labels->end() && i < 5; it++)
    {
        StalkerLabels *tmp = *it;
        mainLayout->addWidget( tmp->getName() , 0 , i);
        mainLayout->addWidget( tmp->getLat() , 1 , i);
        mainLayout->addWidget( tmp->getLon() , 2 , i);
        mainLayout->addWidget( tmp->getPMap() , 3 , i);

        i++;
    }

    for (i = 0; it != labels->end() && i < 5; it++)
    {
        StalkerLabels *tmp = *it;
        mainLayout->addWidget( tmp->getName() , 4 , i);
        mainLayout->addWidget( tmp->getLat() , 5 , i);
        mainLayout->addWidget( tmp->getLon() , 6 , i);
        mainLayout->addWidget( tmp->getPMap() , 7 , i);

        i++;
    }
}

// Sets the grid layout of this widget to be the one passed in
void MainTab::setGridLayout(QGridLayout *gLayout)
{
    if (mainLayout)
        delete this->layout();
    mainLayout = gLayout;
    this->setLayout(gLayout);
}


QGridLayout * MainTab::getGridLayout()
{
    return mainLayout;
}


ViewController* MainTab::getViewController()
{
    return vc;
}


void MainTab::setViewController(ViewController* vc)
{
    this->vc = vc;
}




//void MainTab::mousePressEvent(QMouseEvent *event)
//{
//    QWidget::mousePressEvent(event);
//    if (event->button() == Qt::LeftButton)
//    {
//        printf("Clicked!\n");
//    }
//}

