#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/**
 * @brief The MainWindow class
 *      Acts as the overall window that all of the other widgets
 *      are added to in building the gui.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @brief MainWindow::setupAsMainWindow
     *      Creates clickable actions added to a menu and toolbar
     */
    void setupAsMainWindow();


//    void closeEvent(QCloseEvent *event);

public slots:

    /**
     * @brief MainWindow::newFile
     *      Slot method to be triggered by a signal.
     */
    void newFile();

    /**
     * @brief MainWindow::open
     *      Slot method to be triggered by a signal.
     */
    void open();

private:

};

#endif // MAINWINDOW_H
