#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //return a.exec();
    while(true){
        a.processEvents();
        w.synth.cleanupNotes();
    }
    return 0;
}
