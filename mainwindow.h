#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "oldcode/PolySynth.h"
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    PolySynth       synth;
private:
    int             onOctave = 3;
    Ui::MainWindow* ui;

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
    // detune
    int             detune_dial_min = -200;
    int             detune_dial_max = 200;
    const float     detune_magnitude = 50.0;
    // phase offset
    int             phase_min = 0;
    int             phase_max = 99;
private:
    void            setupWidgets();
    void            startSynth();
    void            populateControlsMap();
private:
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;
private slots:
    void            adsrChanged(int val);
    void            enabledOscillatorsChanged(int b);
    void            oscillatorAmplitudeChanged(int val);
    void            oscillatorShapeChanged(int index);
    void            oscillatorDetuneChanged(int val);
    void            oscillatorPhaseChanged(int offset);
private:
    std::map<std::string,OscillatorDescription*> controls_map;
    std::map<std::string,std::string>            lcd_map;
};
#endif // MAINWINDOW_H
