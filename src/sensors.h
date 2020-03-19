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
public:
    SensorsWrapper(void);
    void setup(void);
    int readResolver(void);
    void readCurrent(int *u, int *v, int *w);
    int readAccel(void);
};

}

#endif /* SENSORS_H_ */
