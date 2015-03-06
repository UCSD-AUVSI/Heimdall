#ifndef UTIL_H
#define UTIL_H

#include <QMainWindow>
#include <QPushButton>
#include <QCoreApplication>
#include <QTextStream>
#include <QFile>
#include <QMouseEvent>
#include <QLineEdit>
#include <QLabel>
#include <QDir>
#include <set>
#include <QString>
#include <queue>
#include <QObject>

// Global vars
//QString inputDir = ".\\psin";

QString getInputDir();
//{
//    return inputDir;
//}


/////////////////////////////////////////////////////////////


class GuiButton : public QPushButton
{

public:
    GuiButton(QWidget * parent = 0);
    GuiButton(const QString & text, QWidget * parent = 0);
    ~GuiButton();
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void addQLineEdit(QLineEdit *ledit);
    void setFunptr(void (* func)());

private:
    std::list<QLineEdit*> *textBoxes;
    void (*funptr)();
};

/////////////////////////////////////////////////////////////

class TargetRec : public QWidget
{
    Q_OBJECT

public:
    TargetRec();
    void recurseTargets(QDir &dir, QLabel *lbl);
    void recurseDir(QDir &dir, std::set<QString> *targetFolders);

signals:
    void infoConfirmed(QLabel *lbl, QString fileStr);

public slots:
    void nextImage(QLabel *lbl, QString fileStr);

private:
    bool imageConfirmed;
//    Q_DISABLE_COPY(TargetRec)
//    QLabel *picLabel;

};

/////////////////////////////////////////////////////////////

class StalkerLabels
{

public:
    StalkerLabels();
    StalkerLabels(QString name, QString lat, QString lon,
                  QString pMapPath);
    ~StalkerLabels();
    QLabel* getName();
    QLabel* getLat();
    QLabel* getLon();
    QLabel* getPMap();
    QString getAbsPath();


private:
    QLabel *name;
    QLabel *lat;
    QLabel *lon;
    QLabel *pMap;
    QString absPath;

};


#endif // UTIL_H
