/*
 * dacvtr.cpp
 *
 *  Created on: 2020/03/18
 *      Author: kosuke
 */

#include "dacvtr.h"
#include "iodefine.h"

using namespace DAConverter;

/*****************
 * Pin Selection *
 *****************/
#define SSL0_DDR PORT3.DDR.BIT.B0
#define SSL1_DDR PORT3.DDR.BIT.B1
#define SSL2_DDR PORT3.DDR.BIT.B2
#define SSL3_DDR PORT3.DDR.BIT.B3
#define SCK_DDR  PORT2.DDR.BIT.B4
#define SDI_DDR  PORT2.DDR.BIT.B3
#define LDAC_DDR PORT2.DDR.BIT.B2
#define SSL0 PORT3.DR.BIT.B0
#define SSL1 PORT3.DR.BIT.B1
#define SSL2 PORT3.DR.BIT.B2
#define SSL3 PORT3.DR.BIT.B3
#define SCK  PORT2.DR.BIT.B4
#define SDI  PORT2.DR.BIT.B3
#define LDAC PORT2.DR.BIT.B2
#define SSL(n) SSL#n

DACHandler::DACHandler(void) {
    this->data[0] = 0;
    this->data[1] = 0;
    this->data[2] = 0;
    this->data[3] = 0;
}

Result::Type DACHandler::setup(void) {
    SSL0_DDR = 1;
    SSL1_DDR = 1;
    SSL2_DDR = 1;
    SSL3_DDR = 1;
    SCK_DDR = 1;
    SDI_DDR = 1;
    LDAC_DDR = 1;
    SSL0 = 1;
    SSL1 = 1;
    SSL2 = 1;
    SSL3 = 1;
    SCK = 0;
    SDI = 0;
    LDAC = 1;

    return Result::OK;
}

DACHandler* DACHandler::setData(DAChannel::Type ch, short data) {
    int dataIdx = static_cast<int>(ch);

    if (data < 0)
        this->data[dataIdx] = 0;
    else if (4095 < data)
        this->data[dataIdx] = 4095;
    else
        this->data[dataIdx] = data;

    return this;
}

void DACHandler::commit(void) {
    SSL0 = 1;
    SSL1 = 1;
    SSL2 = 1;
    SSL3 = 1;
    SCK = 0;
    SDI = 0;
    LDAC = 1;

    for (int i = 0; i < 4; i++) {
        unsigned short payload = 0x0000;
        payload |= (i % 2) << 15; // Channel select: A if i = 0, 2; B otherwise
        payload |= 1 << 14;       // Vref buffered
        payload |= 1 << 13;       // Gain select: 1x
        payload |= 1 << 12;       // Enable output
        payload |= this->data[i]; // Data

        switch (i) {
        case 0:         // Chip 0, Ch.A
        case 1:         // Chip 0, Ch.B
            SSL0 = 0;
            break;
        case 2:         // Chip 1, Ch.A
        case 3:         // Chip 1, Ch.B
            SSL1 = 0;
            break;
        }

        for (int j = 15; 0 <= j; j--) {
            SCK = 0;
            SDI = (payload >> j) & 0x0001;
            SCK = 1;
        }

        switch (i) {
        case 0:
        case 1:
            SSL0 = 1;
            break;
        case 2:
        case 3:
            SSL1 = 1;
            break;
        }

        LDAC = 0;
        LDAC = 1;
    }
}
