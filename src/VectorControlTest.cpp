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

void main(void)
{
    MSTP(MTU) = 0;
    MSTP(S12AD) = 0;

    IEN(S12AD1, S12ADI1) = 1;
    IPR(S12AD1, S12ADI1) = 3;

    cpwm = new CPWM::CPWMWrapper(8000, 100);
    sensors = new Sensors::SensorsWrapper();
    vecCtrl = new VectorControl(0.80, 0.20);

    cpwm->setup();
    sensors->setup();

    // MTU0 setup
    MTU.TSTRA.BIT.CST0 = 0;
    MTU0.TCR.BIT.TPSC = 1; // ICLK / 4
    MTU0.TCR.BIT.CCLR = 1; // TGRAとのコンペアマッチでカウンタクリア
    MTU0.TCR.BIT.CKEG = 0; // 立ち上がりエッジでカウント
    MTU0.TIORH.BYTE = 0;   // TGRA, TGRBのコンペアマッチで出力禁止
    MTU0.TIORL.BYTE = 0;   // TGRB, TGRDのコンペアマッチで出力禁止
    MTU0.TIER.BIT.TTGE = 1; // Allow MTU0 AD conversion start request
    MTU0.TGRA = 2000;

    MTU.TCSYSTR.BYTE = 0x98; // MTU Ch.0,3,4同期スタート
}

#pragma interrupt int_s12adi1(vect=VECT(S12AD1, S12ADI1))
void int_s12adi1(void) {
    const float twoPi = 6.28318530717;

    int iu_raw;
    int iv_raw;
    int iw_raw;
    sensors->readCurrent(&iu_raw, &iv_raw, &iw_raw);
    int resolver_raw = sensors->readResolver();
    int accel_raw = sensors->readAccel();

    float iu = (iu_raw - 2048) * 300.0f / 1229.0f;
    float iv = (iv_raw - 2048) * 300.0f / 1229.0f;
    float iw = (iw_raw - 2048) * 300.0f / 1229.0f;
    float resolver = resolver_raw * twoPi / 1024.0f;
    float accel = accel_raw / 4096.0f;

    float vu;
    float vv;
    float vw;

    vecCtrl->setCurrent(iu, iv, iw)
           ->setRotationAngle(resolver)
           ->setAccelVal(accel)
           ->calculate(&vu, &vv, &vw);

    cpwm->setTGR3D(vu / 200);
    cpwm->setTGR4B(vv / 200);
    cpwm->setTGR4D(vw / 200);
}

#ifdef __cplusplus
void abort(void)
{

}
#endif
