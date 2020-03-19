/*
 * cpwm.h
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#ifndef CPWM_H_
#define CPWM_H_

namespace CPWM {

namespace Result {
enum Type {
    OK = 0,
    ERROR = 1,
    INVALID = 2
};
}

class CPWMWrapper {
private:
    int carrierCycle;
    int deadtime;
public:
    CPWMWrapper(int carrierCycle, int deadtime);
    void setup(void);
    void startOutput(void);
    void stopOutput(void);
    Result::Type setTGR3D(float value);
    Result::Type setTGR4B(float value);
    Result::Type setTGR4D(float value);
};

}
#endif /* CPWM_H_ */
