#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
/*
Oscilloscope class - 2021- Jim Marshall

initialise with the graphics view widget that
will be the oscilloscope, pass all audio samples
to it with pushLSample and regularly call update, change
trigger point field to change trigger point. triggers
on rising value
*/
class Oscilloscope
{
public:
    Oscilloscope();
    void               init(QGraphicsView* view);
    void               pushLSample(float s);
    void               pushRSample(float s);
    void               update();
public:
    float              trigger = 0.3;
private:
    size_t             view_width_samples = 512;
    bool               new_data_flag = false;
    bool               triggered = false;
    std::vector<float> l_samples;
    std::vector<float> r_samples;
    QGraphicsView*     view;
    QGraphicsScene*    scene;
};

#endif // OSCILLOSCOPE_H
