/*
 * sensors.cpp
 *
 *  Created on: 2020/03/16
 *      Author: kosuke
 */

#include "sensors.h"
#include "iodefine.h"

using namespace Sensors;

SensorsWrapper::SensorsWrapper(void) {
    this->accelAdjuster = 0;
    this->iUAdjuster = 0;
    this->iVAdjuster = 0;
    this->iWAdjuster = 0;
}

void SensorsWrapper::setup(int samplingCount) {
    PORTA.DDR.BYTE = 0x00;
    PORTB.DDR.BYTE &= 0xF0;
    PORTA.ICR.BYTE = 0x3F;
    PORTB.ICR.BYTE |= 0x0F;

    // 12-bit ADC unit 0
    S12AD0.ADCSR.BIT.ADST = 0;      // AD変換停止
    S12AD0.ADCSR.BIT.EXTRG = 0;     // ADCトリガをADSTRGRで選択
    S12AD0.ADCSR.BIT.TRGE = 1;      // 外部・周辺モジュールトリガによるAD変換開始を許可
    S12AD0.ADCSR.BIT.CKS = 0;       // ADCクロック = PCLK/8
    S12AD0.ADCSR.BIT.ADIE = 0;      // AD変換終了時の割り込みを禁止
    S12AD0.ADCSR.BIT.ADCS = 0;      // シングルスキャンモード
    S12AD0.ADANS.BIT.PG000EN = 0;   // Ch.0 PGA動作禁止
    S12AD0.ADANS.BIT.PG001EN = 0;   // Ch.1 PGA動作禁止
    S12AD0.ADANS.BIT.PG002EN = 0;   // Ch.2 PGA動作禁止
    S12AD0.ADANS.BIT.PG000SEL = 0;  // Ch.0 PGAを経由しない
    S12AD0.ADANS.BIT.PG001SEL = 0;  // Ch.1 PGAを経由しない
    S12AD0.ADANS.BIT.PG002SEL = 0;  // Ch.2 PGAを経由しない
    S12AD0.ADANS.BIT.CH = 1;        // AN001をスキャン
    S12AD0.ADCER.BIT.SHBYP = 0;     // サンプル&ホールド回路を使用
    S12AD0.ADCER.BIT.ADPRC = 0;     // 12-bit精度で変換
    S12AD0.ADCER.BIT.ACE = 1;       // ADDRn, ADRDの読み出しによる自動クリアを許可
    S12AD0.ADCER.BIT.DIAGVAL = 2;   // VREFH0×1/2の電圧でADC自己診断を実施
    S12AD0.ADCER.BIT.DIAGLD = 0;    // ADC自己診断に用いる電圧値を自動的にローテーション
    S12AD0.ADCER.BIT.DIAGM = 1;     // ADCの自己診断を実施する
    S12AD0.ADCER.BIT.ADIE2 = 0;     // グループ0・グループ1トリガそれぞれの変換終了時にS12ADIを発生（１サイクルスキャンモードのため無視）
    S12AD0.ADCER.BIT.ADIEW = 0;     // ダブルトリガそれぞれの変換終了時にS12ADIを発生（１サイクルスキャンモードのため無視）
    S12AD0.ADCER.BIT.ADRFMT = 0;    // AD変換データを右詰めで格納
    S12AD0.ADSTRGR.BIT.ADSTRS0 = 1; // MTU0.TCNTとMTU0.TGRAとのコンペアマッチでAD変換開始
    S12AD.ADCMPMD0.WORD = 0x0000;   // コンパレータを使用しない

    // 12-bit ADC unit 1
    S12AD1.ADCSR.BIT.ADST = 0;      // AD変換停止
    S12AD1.ADCSR.BIT.EXTRG = 0;     // ADCトリガをADSTRGRで選択
    S12AD1.ADCSR.BIT.TRGE = 1;      // 外部・周辺モジュールトリガによるAD変換開始を許可
    S12AD1.ADCSR.BIT.CKS = 0;       // ADCクロック = PCLK/8
    S12AD1.ADCSR.BIT.ADIE = 1;      // AD変換終了時の割り込みを許可
    S12AD1.ADCSR.BIT.ADCS = 1;      // 1サイクルスキャンモード
    S12AD1.ADANS.BIT.PG100EN = 0;   // Ch.0 PGA動作禁止
    S12AD1.ADANS.BIT.PG101EN = 0;   // Ch.1 PGA動作禁止
    S12AD1.ADANS.BIT.PG102EN = 0;   // Ch.2 PGA動作禁止
    S12AD1.ADANS.BIT.PG100SEL = 0;  // Ch.0 PGAを経由しない
    S12AD1.ADANS.BIT.PG101SEL = 0;  // Ch.1 PGAを経由しない
    S12AD1.ADANS.BIT.PG102SEL = 0;  // Ch.2 PGAを経由しない
    S12AD1.ADANS.BIT.CH = 3;        // AN000~AN003をスキャン
    S12AD1.ADCER.BIT.SHBYP = 0;     // サンプル&ホールド回路を使用
    S12AD1.ADCER.BIT.ADPRC = 0;     // 12-bit精度で変換
    S12AD1.ADCER.BIT.ACE = 1;       // ADDRn, ADRDの読み出しによる自動クリアを許可
    S12AD1.ADCER.BIT.DIAGVAL = 2;   // VREFH0×1/2の電圧でADC自己診断を実施
    S12AD1.ADCER.BIT.DIAGLD = 0;    // ADC自己診断に用いる電圧値を自動的にローテーション
    S12AD1.ADCER.BIT.DIAGM = 1;     // ADCの自己診断を実施する
    S12AD1.ADCER.BIT.ADIE2 = 1;     // グループ0・グループ1トリガそれぞれの変換終了時にS12ADIを発生（１サイクルスキャンモードのため無視）
    S12AD1.ADCER.BIT.ADIEW = 1;     // ダブルトリガそれぞれの変換終了時にS12ADIを発生（１サイクルスキャンモードのため無視）
    S12AD1.ADCER.BIT.ADRFMT = 0;    // AD変換データを右詰めで格納
    S12AD1.ADSTRGR.BIT.ADSTRS0 = 1; // MTU0.TCNTとMTU0.TGRAとのコンペアマッチでAD変換開始
    S12AD.ADCMPMD0.WORD = 0x0000;   // コンパレータを使用しない

    for (int i = 0; i < samplingCount; i++) {
        S12AD0.ADCSR.BIT.ADST = 1;
        while (S12AD0.ADCSR.BIT.ADST) {
        }

        this->accelAdjuster += S12AD0.ADDR1;
    }
    this->accelAdjuster /= samplingCount;
    this->accelAdjuster -= 2048;

    for (int i = 0; i < samplingCount; i++) {
        S12AD1.ADCSR.BIT.ADST = 1;
        while (S12AD1.ADCSR.BIT.ADST) {
        }

        this->iUAdjuster += S12AD1.ADDR2;
        this->iVAdjuster += S12AD1.ADDR1;
        this->iWAdjuster += S12AD1.ADDR0A;
    }
    this->iUAdjuster /= samplingCount;
    this->iVAdjuster /= samplingCount;
    this->iWAdjuster /= samplingCount;

    this->iUAdjuster -= 2048;
    this->iVAdjuster -= 2048;
    this->iWAdjuster -= 2048;
}

int SensorsWrapper::readResolver(void) {
    return ((PORTB.PORT.BYTE & 0x0F) << 6) | (PORTA.PORT.BYTE & 0x3F);
}

void SensorsWrapper::readCurrent(int *u, int *v, int *w) {
    *u = S12AD1.ADDR2 - this->iUAdjuster;
    *v = S12AD1.ADDR1 - this->iVAdjuster;
    *w = S12AD1.ADDR0A - this->iWAdjuster;
}

int SensorsWrapper::readAccel(void) {
    return S12AD0.ADDR1;
}
