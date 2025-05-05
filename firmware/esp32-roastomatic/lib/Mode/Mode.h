#ifndef MODE_H
#define MODE_H

class Device; // Forward declaration of Device

class Mode
{
public:
    explicit Mode(Device &deviceRef) : device(deviceRef) {}

    virtual void setup();
    virtual void loop();
    virtual ~Mode() {}

protected:
    Device &device;
};

#endif // MODE_H