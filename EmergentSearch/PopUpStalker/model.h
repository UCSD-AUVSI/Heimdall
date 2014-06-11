#ifndef MODEL_H
#define MODEL_H

#include "viewcontroller.h"
#include "util.h"
#include <set>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <condition_variable>

#include <QObject>
#include <QFileInfo>
#include <QDir>
#include <QLabel>

class ViewController;

/**
 * @brief The Model class
 *      This class should contain all the backend stuff for the Big Dog gui.
 *      The idea is that this will function as the Model in the MVC pattern.
 */
class Model : public QObject
{
    Q_OBJECT

public:
    Model();
//    Model(ViewController *vc);

    void setViewController(ViewController *vc);
    ViewController* getViewController();

//    void recurseDir(QDir &dir, ViewController *vc);

    /**
     * @brief Model::fileChecker
     *      Recursively goes through dir and looks for .png files.
     *      When one is found, it emits a signal that an image is found.
     * @param dir
     *      QDir object representing the directory to be recursed.
     * @param vc
     *      ViewController object to pass to the emitted signal
     *      for adding the image to the gui.
     */
    void fileChecker(QDir dir, ViewController *vc);

    std::mutex *setMutex;
    std::unordered_set<std::string> *targetFiles;

signals:

    // Signal to be sent with those arguments. No implementation needed.
    void imageFound(QFileInfo file, ViewController *vc, QString ext);
    void queueAdd(QString absFP, QString fName);

public slots:


    void addToNextQ(QString fp, QString fName);

    void nextClicked();
    void prevClicked();




private:
    ViewController *vc;
    std::mutex *qMutex;

    std::unique_lock<std::mutex> *nextQLock;
    std::condition_variable *qFull; // Condition if nextQ is full
    std::condition_variable *qSize; // Condition to sync nextQ's size


    int prev;
    std::vector<StalkerLabels*> *curr;
    std::vector<StalkerLabels*> *nextQ;
    std::vector<StalkerLabels*> *prev1;
    std::vector<StalkerLabels*> *prev2;
    std::vector<StalkerLabels*> *prev3;
    std::vector<StalkerLabels*> *prev4;
    std::vector<StalkerLabels*> *prev5;
};

#endif // MODEL_H
