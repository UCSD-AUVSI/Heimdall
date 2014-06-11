#include "viewcontroller.h"
#include <QScrollArea>
#include <QWidget>



ViewController::ViewController() :
    view(0),
    model(0)
{
}

//ViewController::ViewController(MainTab *view, Model *model) :
//    view(view),
//    model(model)
//{
//}

ViewController::~ViewController()
{
    delete mainWin;
    delete listScrollTabs;
}

/**
 * @brief ViewController::setUpBigDog
 *      Set up a MainWindow to display everything on and create
 *      the necessary QTabWidget to hold the tabs to be displayed
 *      on Big Dog.
 */
void ViewController::setUpBigDog()
{
    // used as the main window to display everthing on
    mainWin = new MainWindow();

    // might be useful if you have more than one tab
    listScrollTabs = new std::list<QScrollArea*>();

    // the widget that contains the tabs
    tabWidget = new QTabWidget(mainWin);

    // Sets up the first tab to be scrollable
    QScrollArea *firstScroll = new QScrollArea();
    listScrollTabs->push_back(firstScroll);
    firstScroll->setBackgroundRole(QPalette::Dark);
    firstScroll->setWidgetResizable(true); // scrollArea to update dynamically
    MainTab * firstTab = (new MainTab())->setupAsFirstTab(); // sets up firstTab with the appropriate columns
    view = firstTab; // set the view of the viewcontroller to be this tab
    firstScroll->setWidget(firstTab); // put firstTab in the scrollArea
    tabWidget->addTab(firstScroll, QWidget::tr("Main")); // add the scrollArea to the tabWidget

    // Sets up the second tab to be scrollable
//    QScrollArea *secondScroll = new QScrollArea();
//    listScrollTabs->push_back(secondScroll);
//    secondScroll->setBackgroundRole(QPalette::Light);
//    secondScroll->setWidgetResizable(true);
//    MainTab * secondTab = new MainTab();
//    secondScroll->setWidget(secondTab);
//    tabWidget->addTab(secondScroll, QWidget::tr("Second Tab"));

    // Display tabWidget on the MainWindow
    mainWin->setCentralWidget(tabWidget);

    mainWin->showMaximized();
}

void ViewController::setView(MainTab *view)
{
    this->view = view;
}

void ViewController::setModel(Model *model)
{
    this->model = model;
}

MainTab* ViewController::getView()
{
    return view;
}

Model* ViewController::getModel()
{
    return model;
}

/**
 * @brief ViewController::addRow
 *      Delegate the addition of the row to the view.
 * @param lst
 *      Pass the list of QLabels to the view.
 */
void ViewController::addRow(std::list<QLabel*> *lst)
{
    if (!view)
        return;
    view->addRow(lst);
}

