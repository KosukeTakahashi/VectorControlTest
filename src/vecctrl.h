/*
 * vecdrv_ints.h
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#ifndef VECCTRL_H_
#define VECCTRL_H_

class VectorControl {
private:
    float gainP;
    float gainI;
    float iu;
    float iv;
    float iw;
    float theta;
    float accel;
    float integralDeltaId;
    float integralDeltaIq;
public:
    VectorControl(float gainP, float gainI);
    VectorControl* setCurrent(float u, float v, float w);
    VectorControl* setRotationAngle(float rotAngleRad);
    VectorControl* setAccelVal(float accel);
    void calculate(float *vu, float *vv, float *vw);
};

#endif /* VECCTRL_H_ */
