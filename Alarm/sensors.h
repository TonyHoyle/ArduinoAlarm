#ifndef SENSORS__H
#define SENSORS__H

class SensorDriver
{
  private:
    unsigned int _sensor;
    unsigned int _liveSensor;
    unsigned int _tamper;
    unsigned int _sensorMask;
    unsigned int _tamperMask;
    
  public:
    void begin();
    void maintain();

    void bell(bool trigger);
    void strobe(bool trigger);

    inline unsigned int sensor() { int res = _sensor & _sensorMask; _sensor=0; return res; }
    inline unsigned int liveSensor() { return _liveSensor & _sensorMask; }
    inline unsigned int tamper() { int res = _tamper & _tamperMask; _tamper=0; return res; }
    inline unsigned int sensorMask() { return _sensorMask; }
    inline unsigned int tamperMask() { return _tamperMask; }
};

extern SensorDriver Sensors;

#endif
