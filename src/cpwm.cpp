/*
 * cpwm.cpp
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#include "cpwm.h"
#include "iodefine.h"

using namespace CPWM;

CPWMWrapper::CPWMWrapper(int carrierCycle, int deadtime) {
    this->carrierCycle = carrierCycle;
    this->deadtime = deadtime;
}

void CPWMWrapper::setup(void) {
     // Complementary PWM
    // [1] MTU2, MTU3 カウント動作停止
    MTU.TSTRA.BYTE &= 0x07;

    // [2] カウンタクロック・カウンタクリア要因選択
    MTU3.TCR.BIT.TPSC = 0; // ICLK/1 でカウント
    MTU3.TCR.BIT.CKEG = 0; // 立ち上がりエッジでカウント
    MTU3.TCR.BIT.CCLR = 0; // TCNT クリア禁止
    MTU4.TCR.BIT.TPSC = 0; // ICLK/1 でカウント
    MTU4.TCR.BIT.CKEG = 0; // 立ち上がりエッジでカウント
    MTU4.TCR.BIT.CCLR = 0; // TCNT クリア禁止

    // [3] ブラシレスDCモータ制御設定
    // 120°通電方式？
    MTU.TGCRA.BYTE = 0x80; // 無効

    // [4] TCNT の設定
    MTU3.TCNT = this->deadtime;
    MTU4.TCNT = 0;

    // [5] チャネル間同期設定
    // 設定無し

    // [6] TGR の設定
    MTU3.TGRB = this->carrierCycle / 4;
    MTU4.TGRA = this->carrierCycle / 4;
    MTU4.TGRB = this->carrierCycle / 4;
    MTU3.TGRD = this->carrierCycle / 4;
    MTU4.TGRC = this->carrierCycle / 4;
    MTU4.TGRD = this->carrierCycle / 4;

    // [7] デッドタイムの設定（デッドタイムを生成しない場合は設定）
    // デッドタイムを生成するため設定無し

    // [8] デッドタイム・キャリアの設定
    MTU.TDDRA = this->deadtime;
    MTU.TCDRA = this->carrierCycle / 2;
    MTU.TCBRA = this->carrierCycle / 2;
    MTU3.TGRA = this->carrierCycle / 2 + this->deadtime;
    MTU3.TGRC = this->carrierCycle / 2 + this->deadtime;

    // [9] PWM 出力許可・PWM出力レベル設定
    MTU.TOCR1A.BIT.PSYE = 1; // PWM 同期トグル出力許可
    MTU.TOCR1A.BIT.TOCS = 0; // TOCR1 の設定を適用
    MTU.TOCR1A.BIT.OLSN = 0; // Initial HIGH; Active LOW
    MTU.TOCR1A.BIT.OLSP = 0; // Initial HIGH; Active LOW

    // [10] 相補PWMモード設定
    MTU3.TMDR1.BIT.BFA = 1;
    MTU3.TMDR1.BIT.BFB = 1;
    MTU3.TMDR1.BIT.MD = 0xD;

    // [11] ダブルバッファ機能設定
    // 設定無し

    // [12] 波形出力許可
    MTU.TOERA.BYTE = 0x3F;

    // [13] カウント開始 (TSTRA.CST3, TSTRA.CST4 = 1)
//    MTU.TSTRA.BYTE |= 0xC0;
}

void CPWMWrapper::startOutput(void) {
    MTU.TSTRA.BYTE |= 0xC0;
}

void CPWMWrapper::stopOutput(void) {
    MTU.TSTRA.BYTE &= 0x07;
}

Result::Type CPWMWrapper::setTGR3D(float value) {
    value = value < 0.0 ? 0.0
                        : 1.0 < value ? 1.0
                                      : value;

    int range = this->carrierCycle / 2 - 2 * this->deadtime;
    int adjuster = this->deadtime;
    MTU3.TGRD = range * value + adjuster;

    return Result::OK;
}

Result::Type CPWMWrapper::setTGR4B(float value) {
    value = value < 0.0 ? 0.0
                        : 1.0 < value ? 1.0
                                      : value;

    int range = this->carrierCycle / 2 - 2 * this->deadtime;
    int adjuster = this->deadtime;
    MTU4.TGRB = value * range + adjuster;

    return Result::OK;
}

Result::Type CPWMWrapper::setTGR4D(float value) {
    value = value < 0.0 ? 0.0
                        : 1.0 < value ? 1.0
                                      : value;

    int range = this->carrierCycle / 2 - 2 * this->deadtime;
    int adjuster = this->deadtime;
    MTU4.TGRD = value * range + adjuster;

    return Result::OK;
}
