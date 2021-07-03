#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "oldcode/ScopedPaHandler.h"
#include "oldcode/Octaves.h"
#include <QDial>
#include <QLCDNumber>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    synth = new PolySynth();
    ScopedPaHandler paInit;
    if( paInit.result() != paNoError ){
        std::cout << "error" << std::endl;
    }
    else{
        if (synth->open(Pa_GetDefaultOutputDevice())){
            synth->start();
        }
    }

    QDial* a_dial = this->findChild<QDial*>("attack_dial");
    QDial* d_dial = this->findChild<QDial*>("decay_dial");
    QDial* s_dial = this->findChild<QDial*>("sustain_dial");
    QDial* r_dial = this->findChild<QDial*>("release_dial");

    QLCDNumber* a_lcd = this->findChild<QLCDNumber*>("attack_lcd");
    QLCDNumber* d_lcd = this->findChild<QLCDNumber*>("decay_lcd");
    QLCDNumber* s_lcd = this->findChild<QLCDNumber*>("sustain_lcd");
    QLCDNumber* r_lcd = this->findChild<QLCDNumber*>("release_lcd");

    QObject::connect(a_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(d_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(s_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(r_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    synth->stop();
    delete synth;
}
#include <QKeyEvent>
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->isAutoRepeat()){return;}
    float* thisoctave = octaves[onOctave];
    switch(event->key()){

    // white keys
    case Qt::Key_A:
        synth->startTone(thisoctave[C], Qt::Key_A);
        break;
    case Qt::Key_S:
        synth->startTone(thisoctave[D], Qt::Key_S);
        break;
    case Qt::Key_D:
        synth->startTone(thisoctave[E], Qt::Key_D);
        break;
    case Qt::Key_F:
        synth->startTone(thisoctave[F], Qt::Key_F);
        break;
    case Qt::Key_G:
        synth->startTone(thisoctave[G], Qt::Key_G);
        break;
    case Qt::Key_H:
        synth->startTone(thisoctave[A], Qt::Key_H);
        break;
    case Qt::Key_J:
        synth->startTone(thisoctave[B], Qt::Key_J);
        break;
    case Qt::Key_K:
        synth->startTone(thisoctave[HC], Qt::Key_K);
        break;
    ///////////////////////////////////////////////// black keys
    case Qt::Key_W:
        synth->startTone(thisoctave[CS], Qt::Key_W);
        break;
    case Qt::Key_E:
        synth->startTone(thisoctave[DS], Qt::Key_E);
        break;
    case Qt::Key_T:
        synth->startTone(thisoctave[FS], Qt::Key_T);
        break;
    case Qt::Key_Y:
        synth->startTone(thisoctave[GS], Qt::Key_Y);
        break;
    case Qt::Key_U:
        synth->startTone(thisoctave[AS], Qt::Key_U);
        break;
    ///////////////////////////////////////////////// change octave keys
    case Qt::Key_Z:
        onOctave--;
        if (onOctave < 0) {
            onOctave = 0;
        }
        synth->clearOscillators();
        std::cout<<onOctave<<std::endl;
        break;
    case Qt::Key_X:
        onOctave++;
        if (onOctave >= NUM_OCTAVES) {
            onOctave = NUM_OCTAVES-1;
        }
        synth->clearOscillators();
        std::cout<<onOctave<<std::endl;
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()){return;}
    synth->stopTone(event->key());
}
#define DIAL_MAX 500.0
void MainWindow::adsrChanged(int val){
    synth->clearOscillators();
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    double scaled_val;
    QLCDNumber* lcd = nullptr;
    if(object_name == "attack_dial"){
        scaled_val = a_lower + ((double)val / DIAL_MAX)*(a_upper-a_lower);
        lcd = this->findChild<QLCDNumber*>("attack_lcd");
        synth->adsr_state.attack = scaled_val;
    }
    else if(object_name == "decay_dial"){
        scaled_val = d_lower + ((double)val / DIAL_MAX)*(d_upper-d_lower);
        lcd = this->findChild<QLCDNumber*>("decay_lcd");
        synth->adsr_state.decay = scaled_val;
    }
    else if(object_name == "sustain_dial"){
        scaled_val = s_lower + ((double)val / DIAL_MAX)*(s_upper-s_lower);
        lcd = this->findChild<QLCDNumber*>("sustain_lcd");
        synth->adsr_state.sustain = scaled_val;
    }
    else if(object_name == "release_dial"){
        scaled_val = r_lower + ((double)val / DIAL_MAX)*(r_upper-r_lower);
        lcd = this->findChild<QLCDNumber*>("release_lcd");
        synth->adsr_state.release = scaled_val;
    }
    else{
        scaled_val = 0;
    }
    std::cout << object_name <<" "<< scaled_val << std::endl;
    if(lcd != nullptr){
        lcd->display(scaled_val);

    }
}
