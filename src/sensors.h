/*
 * sensors.h
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#ifndef SENSORS_H_
#define SENSORS_H_

namespace Sensors {

class SensorsWrapper {
private:
    int accelAdjuster;
    int iUAdjuster;
    int iVAdjuster;
    int iWAdjuster;
public:
    SensorsWrapper(void);
    void setup(int samplingCount);
    int readResolver(void);
    void readCurrent(int *u, int *v, int *w);
    int readAccel(void);
};

}

#endif /* SENSORS_H_ */
