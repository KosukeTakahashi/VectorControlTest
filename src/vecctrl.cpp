/*
 * vecdrv_ints.c
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#include "vecctrl.h"

#include <mathf.h>

VectorControl::VectorControl(float gainP, float gainI) {
    this->gainP = gainP;
    this->gainI = gainI;
    this->iu = 0;
    this->iv = 0;
    this->iw = 0;
    this->theta = 0.0f;
    this->accel = 0.0f;
    this->integralDeltaId = 0.0f;
    this->integralDeltaIq = 0.0f;
}

VectorControl* VectorControl::setCurrent(float u, float v, float w) {
    this->iu = u;
    this->iv = v;
    this->iw = w;
    return this;
}

VectorControl* VectorControl::setRotationAngle(float rotAngleRad) {
    this->theta = rotAngleRad;
    return this;
}

VectorControl* VectorControl::setAccelVal(float accel) {
    this->accel = accel;
    return this;
}

void VectorControl::calculate(float *vu, float *vv, float *vw) {
    const float phi = 2.09439510239;
    const float sqrt3 = 1.73205080756;

    float id = this->iu * cosf(this->theta) + this->iv * cosf(this->theta - phi) + this->iw * cosf(this->theta + phi);
    float iq = -(this->iu * sinf(this->theta) + this->iv * sinf(this->theta - phi) + this->iw * sinf(this->theta + phi));
    float idRef = 0.0f;
    float iqRef = this->accel * 100.0f;
    float deltaId = idRef - id;
    float deltaIq = iqRef - iq;

    this->integralDeltaId += deltaId;
    this->integralDeltaIq += deltaIq;

    float vd = this->gainP * deltaId/* + this->gainI * this->integralDeltaId*/;
    float vq = this->gainP * deltaIq/* + this->gainI * this->integralDeltaIq*/;

    float va = vd * cosf(this->theta) - vq * sinf(this->theta);
    float vb = vd * sinf(this->theta) + vq * cosf(this->theta);
    *vu = va * 2 / 3;
    *vv = -va / 3 + vb / sqrt3;
    *vw = -va / 3 - vb / sqrt3;
}
