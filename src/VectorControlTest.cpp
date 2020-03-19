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
    MSTP(S12AD) = 0;

    IEN(S12AD1, S12ADI1) = 1;
    IPR(S12AD1, S12ADI1) = 3;

    cpwm = new CPWM::CPWMWrapper(8000, 100);
    sensors = new Sensors::SensorsWrapper();
    vecCtrl = new VectorControl(0.80, 0.20);

    cpwm->setup();
    sensors->setup();
    dachandler->setup();

    // MTU0 setup
    MTU.TSTRA.BIT.CST0 = 0;
    MTU0.TCR.BIT.TPSC = 1; // ICLK / 4
    MTU0.TCR.BIT.CCLR = 1; // TGRAとのコンペアマッチでカウンタクリア
    MTU0.TCR.BIT.CKEG = 0; // 立ち上がりエッジでカウント
    MTU0.TIORH.BYTE = 0;   // TGRA, TGRBのコンペアマッチで出力禁止
    MTU0.TIORL.BYTE = 0;   // TGRB, TGRDのコンペアマッチで出力禁止
    MTU0.TIER.BIT.TTGE = 1; // Allow MTU0 AD conversion start request
    MTU0.TGRA = 2000;

//    MTU.TCSYSTR.BYTE = 0x98; // MTU Ch.0,3,4同期スタート
//    MTU.TSTRA.BIT.CST0 = 1;
    MTU.TSTRA.BYTE = 0xC0;

//    dachandler->setData(DAConverter::DAChannel::CH_0, 0)
//              ->setData(DAConverter::DAChannel::CH_1, 512)
//              ->setData(DAConverter::DAChannel::CH_2, 1024)
//              ->setData(DAConverter::DAChannel::CH_3, 4096)
//              ->commit();

    int counter = 0;
    while (1) {
        if (++counter == 80000)
            counter = 0;

        dachandler->setData(DAConverter::DAChannel::CH_0, counter * 4096 / 80000)
                  ->setData(DAConverter::DAChannel::CH_2, MTU4.TCNT * 4096 / 65536)
                  ->commit();
    }
}

#pragma interrupt int_s12adi1(vect=VECT(S12AD1, S12ADI1))
void int_s12adi1(void) {
//    if (++globalCounter == 80000)
//        globalCounter = 0;
//    dachandler->setData(DAConverter::DAChannel::CH_1, globalCounter * 4096 / 80000)
//              ->commit();

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
