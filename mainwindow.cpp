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
    scope.init(ui->scope_view);

    synth.setOscilloscope(&scope);

}
void MainWindow::populateControlsMap(){
    using namespace std;

    OscillatorDescription* o_ptr = &synth.oscillators_state[0];
    controls_map.insert(pair<string,OscillatorDescription*>("o1_cb",          o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o1_combo",       o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o1_slider",      o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o1_offset_dial", o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o1_detune_dial", o_ptr));
    o_ptr++;
    controls_map.insert(pair<string,OscillatorDescription*>("o2_cb",          o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o2_combo",       o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o2_slider",      o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o2_offset_dial", o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o2_detune_dial", o_ptr));
    o_ptr++;
    controls_map.insert(pair<string,OscillatorDescription*>("o3_cb",          o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o3_combo",       o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o3_slider",      o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o3_offset_dial", o_ptr));
    controls_map.insert(pair<string,OscillatorDescription*>("o3_detune_dial", o_ptr));


    lcd_map.insert(pair<string,string>("o1_slider","o1_slider_lcd"));
    lcd_map.insert(pair<string,string>("o1_offset_dial","o1_offset_lcd"));
    lcd_map.insert(pair<string,string>("o1_detune_dial","o1_detune_lcd"));

    lcd_map.insert(pair<string,string>("o2_slider","o2_slider_lcd"));
    lcd_map.insert(pair<string,string>("o2_offset_dial","o2_offset_lcd"));
    lcd_map.insert(pair<string,string>("o2_detune_dial","o2_detune_lcd"));

    lcd_map.insert(pair<string,string>("o3_slider","o3_slider_lcd"));
    lcd_map.insert(pair<string,string>("o3_offset_dial","o3_offset_lcd"));
    lcd_map.insert(pair<string,string>("o3_detune_dial","o3_detune_lcd"));
}
void MainWindow::startSynth(){
    ScopedPaHandler paInit;
    if( paInit.result() != paNoError ){
        std::cout << "error" << std::endl;
    }
    else{
        int apis = Pa_GetHostApiCount();
        PaDeviceIndex index = Pa_GetDefaultOutputDevice();

        for(int i=0; i<apis; i++){
            const PaHostApiInfo* info = Pa_GetHostApiInfo(i);
            if(i == index)
                std::cout<< info->name << std::endl;
            /*
            if(i == Pa_HostApiTypeIdToHostApiIndex(paASIO)){
                index = info->defaultOutputDevice;
            }
            */
        }

        if (synth.open(index)){
            synth.start();
        }
    }
}
void MainWindow::setupWidgets(){

    QList<QComboBox*> boxes = this->findChildren<QComboBox*>();
    for (int i = 0; i < boxes.size(); ++i) {
        QComboBox* box = boxes.at(i);
        box->insertItem(0,"square50");
        box->insertItem(1,"square25");
        box->insertItem(2,"sine");
        box->insertItem(3,"triangle");
        box->insertItem(4,"saw");
    }

    QObject::connect(ui->attack_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(ui->decay_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(ui->sustain_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));
    QObject::connect(ui->release_dial,SIGNAL(valueChanged(int)), this, SLOT(adsrChanged(int)));

    QObject::connect(ui->o1_cb,SIGNAL(stateChanged(int)),this,SLOT(enabledOscillatorsChanged(int)));
    QObject::connect(ui->o2_cb,SIGNAL(stateChanged(int)),this,SLOT(enabledOscillatorsChanged(int)));
    QObject::connect(ui->o3_cb,SIGNAL(stateChanged(int)),this,SLOT(enabledOscillatorsChanged(int)));

    QObject::connect(ui->o1_slider,SIGNAL(valueChanged(int)),this,SLOT(oscillatorAmplitudeChanged(int)));
    QObject::connect(ui->o2_slider,SIGNAL(valueChanged(int)),this,SLOT(oscillatorAmplitudeChanged(int)));
    QObject::connect(ui->o3_slider,SIGNAL(valueChanged(int)),this,SLOT(oscillatorAmplitudeChanged(int)));

    QObject::connect(ui->o1_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(oscillatorShapeChanged(int)));
    QObject::connect(ui->o2_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(oscillatorShapeChanged(int)));
    QObject::connect(ui->o3_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(oscillatorShapeChanged(int)));

    QObject::connect(ui->o1_detune_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorDetuneChanged(int)));
    QObject::connect(ui->o2_detune_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorDetuneChanged(int)));
    QObject::connect(ui->o3_detune_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorDetuneChanged(int)));

    QObject::connect(ui->o1_offset_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorPhaseChanged(int)));
    QObject::connect(ui->o2_offset_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorPhaseChanged(int)));
    QObject::connect(ui->o3_offset_dial, SIGNAL(valueChanged(int)),this,SLOT(oscillatorPhaseChanged(int)));

    QObject::connect(ui->scope_trigger_dial, SIGNAL(valueChanged(int)),this,SLOT(scopeTriggerChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->scope_view)
    {

        if(e->type() == QEvent::Wheel){
            QWheelEvent *q = dynamic_cast<QWheelEvent*>(e);
            q->accept();
            return true;
        }
    }
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
    auto lcd = this->findChild<QLCDNumber*>(lcd_map[object_name].c_str());
    lcd->display(scaled);
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
    auto lcd = this->findChild<QLCDNumber*>(lcd_map[object_name].c_str());
    lcd->display(fraction*detune_magnitude);
}
void MainWindow::oscillatorPhaseChanged(int offset){
    QObject* obj = sender();
    std::string object_name = static_cast<std::string>((const char*)obj->objectName().toUtf8());
    OscillatorDescription* osc = controls_map[object_name];
    float scaled = ((float)offset/(float)100) * (float)TABLE_SIZE;
    osc->phase_offset = scaled;
    auto lcd = this->findChild<QLCDNumber*>(lcd_map[object_name].c_str());
    lcd->display(scaled);
}

void MainWindow::scopeTriggerChanged(int val)
{
    float scaled = (float)val/100.0f;
    scope.trigger = scaled;
    ui->scope_trigger_lcd->display(scaled);
}
