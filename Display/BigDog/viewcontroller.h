#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "maintab.h"
#include "model.h"
#include "mainwindow.h"
#include <QTableWidget>
#include <QScrollArea>
#include <QLabel>

class MainTab;
class Model;

/**
 * @brief The ViewController class
 *      The idea is that this will act as the Controller of the MVC pattern in which
 *      it will handle communication between the view and the model.
 *      **Note:
 *          The way that the view is setup right now kind of violates the MVC pattern
 *          in that this really has 2 view that it keeps track of namely a Model
 *          and a MainWindow. It may be worthwhile to refactor this later.
 */
class ViewController
{
public:
    ViewController();
//    ViewController(MainTab *view, Model *model);
    ~ViewController();
    void setView(MainTab *view);
    void setModel(Model *model);
    MainTab* getView();
    Model* getModel();

    /**
     * @brief ViewController::setUpBigDog
     *      Set up a MainWindow to display everything on and create
     *      the necessary QTabWidget to hold the tabs to be displayed
     *      on Big Dog.
     */
    void setUpBigDog();

    /**
     * @brief ViewController::addRow
     *      Delegate the addition of the row to the view.
     * @param lst
     *      Pass the list of QLabels to the view.
     */
    void addRow(std::list<QLabel*> *lst);

private:
    MainTab *view;
    Model *model;
    MainWindow *mainWin;
    QTabWidget *tabWidget;
    std::list<QScrollArea*> *listScrollTabs;
};

#endif // VIEWCONTROLLER_H
