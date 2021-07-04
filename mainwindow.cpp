#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "oldcode/ScopedPaHandler.h"
#include "oldcode/Octaves.h"
#include <QDial>
#include <QLCDNumber>
#include <QSlider>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    startSynth();
    setupWidgets();
    populateControlsMap();


}
void MainWindow::populateControlsMap(){
    OscillatorDescription* o_ptr = &synth.oscillators_state[0];
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o1_cb",          o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o1_combo",       o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o1_slider",      o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o1_offset_dial", o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o1_detune_dial", o_ptr));
    o_ptr++;
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o2_cb",          o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o2_combo",       o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o2_slider",      o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o2_offset_dial", o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o2_detune_dial", o_ptr));
    o_ptr++;
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o3_cb",          o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o3_combo",       o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o3_slider",      o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o3_offset_dial", o_ptr));
    controls_map.insert(std::pair<std::string,OscillatorDescription*>("o3_detune_dial", o_ptr));
}
void MainWindow::startSynth(){
    ScopedPaHandler paInit;
    if( paInit.result() != paNoError ){
        std::cout << "error" << std::endl;
    }
    else{
        if (synth.open(Pa_GetDefaultOutputDevice())){
            synth.start();
        }
    }
}
void MainWindow::setupWidgets(){
    QDial* a_dial = this->findChild<QDial*>("attack_dial");
    QDial* d_dial = this->findChild<QDial*>("decay_dial");
    QDial* s_dial = this->findChild<QDial*>("sustain_dial");
    QDial* r_dial = this->findChild<QDial*>("release_dial");


    QObject::connect(a_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(d_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(s_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(r_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));

    QList<QComboBox*> boxes = this->findChildren<QComboBox*>();
    for (int i = 0; i < boxes.size(); ++i) {
        QComboBox* box = boxes.at(i);
        box->insertItem(0,"square50");
        box->insertItem(1,"square25");
        box->insertItem(2,"sine");
        box->insertItem(3,"triangle");
        box->insertItem(4,"saw");
    }

    QCheckBox* o1_cb     = this->findChild<QCheckBox*>("o1_cb");
    QSlider*   o1_slider = this->findChild<QSlider*  >("o1_slider");
    QComboBox* o1_combo  = this->findChild<QComboBox*>("o1_combo");

    QCheckBox* o2_cb     = this->findChild<QCheckBox*>("o2_cb");
    QSlider*   o2_slider = this->findChild<QSlider*  >("o2_slider");
    QComboBox* o2_combo  = this->findChild<QComboBox*>("o2_combo");

    QCheckBox* o3_cb     = this->findChild<QCheckBox*>("o3_cb");
    QSlider*   o3_slider = this->findChild<QSlider*  >("o3_slider");
    QComboBox* o3_combo  = this->findChild<QComboBox*>("o3_combo");

    QDial* o1_detune_dial = this->findChild<QDial*>("o1_detune_dial");
    QDial* o2_detune_dial = this->findChild<QDial*>("o2_detune_dial");
    QDial* o3_detune_dial = this->findChild<QDial*>("o3_detune_dial");

    QObject::connect(o1_cb,SIGNAL(stateChanged(int)),this,SLOT(enabledOscillatorsChanged(int)));
    QObject::connect(o2_cb,SIGNAL(stateChanged(int)),this,SLOT(enabledOscillatorsChanged(int)));
    QObject::connect(o3_cb,SIGNAL(stateChanged(int)),this,SLOT(enabledOscillatorsChanged(int)));

    QObject::connect(o1_slider,SIGNAL(valueChanged(int)),this,SLOT(oscillatorAmplitudeChanged(int)));
    QObject::connect(o2_slider,SIGNAL(valueChanged(int)),this,SLOT(oscillatorAmplitudeChanged(int)));
    QObject::connect(o3_slider,SIGNAL(valueChanged(int)),this,SLOT(oscillatorAmplitudeChanged(int)));

    QObject::connect(o1_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(oscillatorShapeChanged(int)));
    QObject::connect(o2_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(oscillatorShapeChanged(int)));
    QObject::connect(o3_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(oscillatorShapeChanged(int)));

    QObject::connect(o1_detune_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorDetuneChanged(int)));
    QObject::connect(o2_detune_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorDetuneChanged(int)));
    QObject::connect(o3_detune_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorDetuneChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    synth.stop();
}
#include <QKeyEvent>
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->isAutoRepeat()){return;}
    std::cout << synth.getNumNotes() << std::endl;
    float* thisoctave = octaves[onOctave];
    int key = event->key();
    switch(key){

    // white keys
    case Qt::Key_A:
        synth.startTone(thisoctave[C], key);
        break;
    case Qt::Key_S:
        synth.startTone(thisoctave[D], key);
        break;
    case Qt::Key_D:
        synth.startTone(thisoctave[E], key);
        break;
    case Qt::Key_F:
        synth.startTone(thisoctave[F], key);
        break;
    case Qt::Key_G:
        synth.startTone(thisoctave[G], key);
        break;
    case Qt::Key_H:
        synth.startTone(thisoctave[A], key);
        break;
    case Qt::Key_J:
        synth.startTone(thisoctave[B], key);
        break;
    case Qt::Key_K:
        synth.startTone(thisoctave[HC], key);
        break;
    ///////////////////////////////////////////////// black keys
    case Qt::Key_W:
        synth.startTone(thisoctave[CS], key);
        break;
    case Qt::Key_E:
        synth.startTone(thisoctave[DS], key);
        break;
    case Qt::Key_T:
        synth.startTone(thisoctave[FS], key);
        break;
    case Qt::Key_Y:
        synth.startTone(thisoctave[GS], key);
        break;
    case Qt::Key_U:
        synth.startTone(thisoctave[AS], key);
        break;
    ///////////////////////////////////////////////// change octave keys
    case Qt::Key_Z:
        onOctave--;
        if (onOctave < 0) {
            onOctave = 0;
        }
        synth.clearOscillators();
        std::cout<<onOctave<<std::endl;
        break;
    case Qt::Key_X:
        onOctave++;
        if (onOctave >= NUM_OCTAVES) {
            onOctave = NUM_OCTAVES-1;
        }
        synth.clearOscillators();
        std::cout<<onOctave<<std::endl;
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    std::cout << synth.getNumNotes() << std::endl;
    if(event->isAutoRepeat()){return;}
    synth.stopTone(event->key());
    //synth.cleanupNotes();
}
#define DIAL_MAX 500.0
void MainWindow::adsrChanged(int val){
    synth.clearOscillators();
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    double scaled_val;
    QLCDNumber* lcd = nullptr;
    if(object_name == "attack_dial"){
        scaled_val = a_lower + ((double)val / DIAL_MAX)*(a_upper-a_lower);
        lcd = this->findChild<QLCDNumber*>("attack_lcd");
        synth.adsr_state.attack = scaled_val;
    }
    else if(object_name == "decay_dial"){
        scaled_val = d_lower + ((double)val / DIAL_MAX)*(d_upper-d_lower);
        lcd = this->findChild<QLCDNumber*>("decay_lcd");
        synth.adsr_state.decay = scaled_val;
    }
    else if(object_name == "sustain_dial"){
        scaled_val = s_lower + ((double)val / DIAL_MAX)*(s_upper-s_lower);
        lcd = this->findChild<QLCDNumber*>("sustain_lcd");
        synth.adsr_state.sustain = scaled_val;
    }
    else if(object_name == "release_dial"){
        scaled_val = r_lower + ((double)val / DIAL_MAX)*(r_upper-r_lower);
        lcd = this->findChild<QLCDNumber*>("release_lcd");
        synth.adsr_state.release = scaled_val;
    }
    else{
        scaled_val = 0;
    }
    std::cout << object_name <<" "<< scaled_val << std::endl;
    if(lcd != nullptr){
        lcd->display(scaled_val);

    }
}
#define OSC_AMP_SLIDER_SIZE 100
void MainWindow::enabledOscillatorsChanged(int b){
    synth.clearOscillators();
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    OscillatorDescription* osc = controls_map[object_name];
    osc->active = (bool)b;
}

void MainWindow::oscillatorAmplitudeChanged(int val){
    synth.clearOscillators();
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    double scaled = (double)val / (double)OSC_AMP_SLIDER_SIZE;
    OscillatorDescription* osc = controls_map[object_name];
    osc->amplitude = scaled;
}
void MainWindow::oscillatorShapeChanged(int index){
    synth.clearOscillators();
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    OscillatorDescription* osc = controls_map[object_name];
    switch(index){
    case 0:
        osc->shape = WAVESHAPE::SQUARE50;
        break;
    case 1:
        osc->shape = WAVESHAPE::SQUARE25;
        break;
    case 2:
        osc->shape = WAVESHAPE::SINE;
        break;
    case 3:
        osc->shape = WAVESHAPE::TRIANGLE;
        break;
    case 4:
        osc->shape = WAVESHAPE::SAWTOOTH;
        break;
    }
}

void MainWindow::oscillatorDetuneChanged(int val){
    float detune_mag = abs((float)detune_dial_max - (float)detune_dial_min)/2.0f;
    float fraction   = (float)val/detune_mag;
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    OscillatorDescription* osc = controls_map[object_name];
    osc->detune = fraction*detune_magnitude;
}
void MainWindow::oscillatorPhaseChanged(int offset){
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    OscillatorDescription* osc = controls_map[object_name];
    float scaled = ((float)offset/(float)100) * (float)TABLE_SIZE;
    osc->phase_offset = scaled;
}
