#include "Oscilloscope.h"

Oscilloscope::Oscilloscope()
{

}

void Oscilloscope::init(QGraphicsView *v)
{
    scene = new QGraphicsScene(v);
    view = v;
    v->setScene(scene);

    l_samples.resize(view_width_samples);
    r_samples.resize(view_width_samples);

    memset(&l_samples[0],0,view_width_samples);
    memset(&r_samples[0],0,view_width_samples);
    new_data_flag = true;
}

void Oscilloscope::pushLSample(float s)
{
    static size_t on_sample = 0;
    static float last_s = 99999999999999999;
    if(triggered){
        //std::lock_guard<std::mutex>lock(m);
        l_samples[on_sample] = s;
        on_sample++;
        if(on_sample == view_width_samples){
            new_data_flag = true;
            triggered = false;
            on_sample = 0;
        }
    }
    else{
        if(s > trigger && last_s <= trigger && !new_data_flag){
            triggered = true;
        }
    }
    last_s = s;
}

void Oscilloscope::pushRSample(float s)
{
    static size_t on_sample = 0;
    if(triggered){
        r_samples[on_sample] = s;
        on_sample++;
        if(on_sample == view_width_samples){
            new_data_flag = true;
            triggered = false;
            on_sample = 0;
        }
    }
    else{
        if(s > trigger && !new_data_flag){
            triggered = true;
        }
    }
}
#include <QPainterPath>
void Oscilloscope::update()
{
    if(new_data_flag){
        drawNewData();
        new_data_flag = false;
    }
}

void Oscilloscope::drawNewData()
{
    scene->clear();
    QPainterPath path;
    float x        =  view->width()/2.0f;
    float y_factor = (float)view->height() / 2.0;
    float x_incr   = (float)view->width() / view_width_samples;
    path.moveTo(QPointF(x, 0.0));
    for(size_t i=0; i<view_width_samples; i++){
        x += x_incr;
        float samp = l_samples[i];
        float y    = y_factor*samp;
        path.lineTo(x,y);

    }
    const QPen pen(Qt::black);
    scene->addPath(path,pen);
}
