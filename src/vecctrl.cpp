/*
 * vecdrv_ints.c
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#include "vecctrl.h"
#include "dacvtr.h"

#include <mathf.h>

VectorControl::VectorControl(float gainP, float gainI) {
    this->gainP = gainP;
    this->gainI = gainI;
    this->iu = 0;
    this->iv = 0;
    this->iw = 0;
    this->theta = 0.0f;
    this->accel = 0.0f;
    this->integralErrorId = 0.0f;
    this->integralErrorIq = 0.0f;
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

void VectorControl::calculate(float *vu, float *vv, float *vw, DAConverter::DACHandler *dachandler) {
    const float phi = 2.09439510239;
    const float sqrt3 = 1.73205080756;

    float ia = (this->iu) - (this->iv / 2.0) - (this->iw / 2.0);
    float ib = (this->iv * sqrt3 / 2.0) - (this->iw * sqrt3 / 2.0);

    float id = ia * cosf(this->theta) + ib * sinf(this->theta);
    float iq = -ia * sinf(this->theta) + ib * cosf(this->theta);

    float idRef = 0.0f;
    float iqRef = this->accel * 100.0f;
    float errorId = idRef - id;
    float errorIq = iqRef - iq;

    this->integralErrorId += errorId;
    this->integralErrorIq += errorIq;

    float vd = this->gainP * errorId + this->gainI * this->integralErrorId;
    float vq = this->gainP * errorIq + this->gainI * this->integralErrorIq;

    float va = vd * cosf(this->theta) - vq * sinf(this->theta);
    float vb = vd * sinf(this->theta) + vq * cosf(this->theta);

    *vu = va * 2.0 / 3.0;
    *vv = (-va / 3.0) + (vb / sqrt3);
    *vw = (-va / 3.0) - (vb / sqrt3);

    dachandler->setData(DAConverter::DAChannel::CH_0, id * 130)
              ->setData(DAConverter::DAChannel::CH_1, iq * 130)
              ->commit();
}
