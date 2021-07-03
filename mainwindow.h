#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "oldcode/PolySynth.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    int             onOctave = 3;
    Ui::MainWindow* ui;
    PolySynth*      synth;
    // a
    double          a_lower = 0.000001;
    double          a_upper = 2;
    // d
    double          d_lower = 0.000001;
    double          d_upper = 2;
    // s
    double          s_lower = 0.000001;
    double          s_upper = 1;
    // r
    double          r_lower = 0.000001;
    double          r_upper = 2;
private:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
private slots:
    void adsrChanged(int val);
};
#endif // MAINWINDOW_H
