/***********************************************************************/
/*                                                                     */
/*  FILE        : Main.cpp                                   */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/

#include "iodefine.h"
#include "cpwm.h"
#include "sensors.h"
#include "vecctrl.h"
#include "dacvtr.h"
#include "math.h"

//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

CPWM::CPWMWrapper *cpwm;
Sensors::SensorsWrapper *sensors;
VectorControl *vecCtrl;
DAConverter::DACHandler *dachandler;

int globalCounter = 0;

void main(void)
{
    MSTP(MTU) = 0;
    MSTP(S12AD0) = 0;
    MSTP(S12AD1) = 0;

    IEN(S12AD1, S12ADI1) = 1;
    IPR(S12AD1, S12ADI1) = 3;

    cpwm = new CPWM::CPWMWrapper(5000, 80);
    sensors = new Sensors::SensorsWrapper();
    vecCtrl = new VectorControl(15.0, 0.0);

    cpwm->setup();
    sensors->setup();
    dachandler->setup();

    // MTU0 setup
    MTU.TSTRA.BIT.CST0 = 0;
    MTU0.TCR.BIT.TPSC = 2; // ICLK / 16
    MTU0.TCR.BIT.CCLR = 1; // TGRAとのコンペアマッチでカウンタクリア
    MTU0.TCR.BIT.CKEG = 0; // 立ち上がりエッジでカウント
    MTU0.TIORH.BYTE = 0;   // TGRA, TGRBのコンペアマッチで出力禁止
    MTU0.TIORL.BYTE = 0;   // TGRB, TGRDのコンペアマッチで出力禁止
    MTU0.TIER.BIT.TTGE = 1; // Allow MTU0 AD conversion start request
    MTU0.TGRA = 625;       // control freq = 5.0 [KHz]

    MTU.TCSYSTR.BYTE = 0x98; // MTU Ch.0,3,4同期スタート

    dachandler->setData(DAConverter::DAChannel::CH_0, 0)
              ->setData(DAConverter::DAChannel::CH_1, 0)
              ->setData(DAConverter::DAChannel::CH_2, 4096)
              ->setData(DAConverter::DAChannel::CH_3, 0)
              ->commit();

    while (1) {
    }
}

#pragma interrupt int_s12adi1 (vect=VECT(S12AD1, S12ADI1))
void int_s12adi1(void) {
    MTU0.TSR.BIT.TGFA = 0;

//    int res = sensors->readResolver();
//    dachandler->setData(DAConverter::DAChannel::CH_2, res * 4)
//              ->setData(DAConverter::DAChannel::CH_3, res == 1023 ? 4095 : 0)
//              ->setData(DAConverter::DAChannel::CH_0, PORTB.PORT.BIT.B1 ? 4095 : 0)
//              ->commit();

    /***************
     * 強制回転磁界 *
     ***************/
    /*
    const int threshold = 80000;
    int accel = sensors->readAccel();
    globalCounter += accel / 64;
    if (threshold < globalCounter)
        globalCounter = 0;

    const float phi = 2.09439510239;
    const float twoPi = 6.28318530718;
    float theta = ((float)globalCounter) / ((float)threshold);
    cpwm->setDutyU((sinf(theta * twoPi) + 1.0f) / 2.0f);
    cpwm->setDutyV((sinf(theta * twoPi + phi) + 1.0f) / 2.0f);
    cpwm->setDutyW((sinf(theta * twoPi - phi) + 1.0f) / 2.0f);
    dachandler->setData(DAConverter::DAChannel::CH_0, (int)(theta * 4096))
              ->commit();
              */

    /*************
     * 正弦波駆動 *
     *************/
    /*
    const float phi = 2.09439510239;
//    const float pi = 3.14159265359;
    const float twoPi = 6.28318530718;
    const float sqrt3_2 = 0.86602540378;

    int accel = sensors->readAccel();
    int resolver = sensors->readResolver();

    resolver = 1023 - resolver;

    resolver /= 4;
    float theta = resolver * twoPi / 128.0;
    theta += 530 * twoPi / 4096.0;

    while (twoPi < theta)
        theta -= twoPi;

    float k = accel / 4096.0;
//    float k = 1.0;
    float dutyU = (sinf(theta) * k + 1.0) / 2.0;
    float dutyV = (sinf(theta - phi) * k + 1.0) / 2.0;
    float dutyW = (sinf(theta + phi) * k + 1.0) / 2.0;

    cpwm->setDutyU(dutyU);
    cpwm->setDutyV(dutyV);
    cpwm->setDutyW(dutyW);

    int iu;
    int iv;
    int iw;

    sensors->readCurrent(&iu, &iv, &iw);

    int a = (iv * sqrt3_2) - (iw * sqrt3_2);
    int b = iu - (iv / 2.0) - (iw / 2.0);
    int d = a * sinf(theta) + b * cosf(theta);
    int q = a * cosf(theta) - b * sinf(theta);

    dachandler->setData(DAConverter::DAChannel::CH_0, d * 50 + 2048)
              ->setData(DAConverter::DAChannel::CH_1, q * 50 + 2048)
              ->commit();
    */

    /*
    float u = sinf(theta);
    float v = sinf(theta - phi);
    float w = sinf(theta + phi);
    float a = (v * sqrt3_2) - (w * sqrt3_2);
    float b = u - (v / 2.0) - (w / 2.0);
    float d = a * sinf(theta) + b * cosf(theta);
    float q = a * cosf(theta) - b * sinf(theta);
    float t = atanf(a / b);

    dachandler->setData(DAConverter::DAChannel::CH_0, (t + pi) / twoPi * 4095)
              ->setData(DAConverter::DAChannel::CH_1, theta / twoPi * 4095)
              ->setData(DAConverter::DAChannel::CH_2, q * 500 + 2048)
              ->setData(DAConverter::DAChannel::CH_3, resolver * 8)
              ->commit();
              */

//    const float twoPi = 6.28318530717;
//    const float phi = 2.09439510239;

//    if (globalCounter == 8000)
//        globalCounter = 0;

//    cpwm->setDutyU(sinf(twoPi * globalCounter / 8000.0f) / 2.0f + 0.5f);
//    cpwm->setDutyV(sinf(phi + twoPi * globalCounter / 8000.0f) / 2.0f + 0.5f);
//    cpwm->setDutyW(sinf((2 * phi) + twoPi * globalCounter / 8000.0f) / 2.0f + 0.5f);

//    int accel = sensors->readAccel();
//    dachandler->setData(DAConverter::DAChannel::CH_0, accel)
//              ->commit();
//    globalCounter += accel / 128;

    const float twoPi = 6.28318530717;

    int iu_raw;
    int iv_raw;
    int iw_raw;
    sensors->readCurrent(&iu_raw, &iv_raw, &iw_raw);
    int resolver_raw = sensors->readResolver();
    int accel_raw = sensors->readAccel();

    /*
    resolver_raw -= 70;

    while (resolver_raw < 0)
        resolver_raw += 1024;
    */

    resolver_raw = 1023 - resolver_raw;
//    resolver_raw /= 4;

    float iu = (iu_raw - 2048) * 75.0 / 307.0;
    float iv = (iv_raw - 2048) * 75.0 / 307.0;
    float iw = (iw_raw - 2048) * 75.0 / 307.0;
    float theta = resolver_raw * twoPi / 512.0;
    float accel = accel_raw / 4096.0;

    float vu;
    float vv;
    float vw;

    theta += 530 * twoPi / 4096.0;
    while (twoPi < theta)
        theta -= twoPi;

    vecCtrl->setCurrent(iu, iv, iw)
           ->setRotationAngle(theta)
           ->setAccelVal(accel)
           ->calculate(&vu, &vv, &vw, dachandler);

    cpwm->setDutyU(vu / 10.0);
    cpwm->setDutyV(vv / 10.0);
    cpwm->setDutyW(vw / 10.0);

//    dachandler->setData(DAConverter::DAChannel::CH_0, vu / 1 + 2048)
//              ->setData(DAConverter::DAChannel::CH_1, vv / 1 + 2048)
//              ->setData(DAConverter::DAChannel::CH_2, vw / 1 + 2048)
//              ->commit();

//    dachandler->setData(DAConverter::DAChannel::CH_0, iu * 400)
//              ->setData(DAConverter::DAChannel::CH_1, iv * 400)
//              ->setData(DAConverter::DAChannel::CH_2, iw * 400)
//              ->commit();
}

#ifdef __cplusplus
void abort(void)
{

}
#endif
