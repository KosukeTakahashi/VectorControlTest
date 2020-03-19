/*
 * da_cvtr.h
 *
 *  Created on: 2020/03/18
 *      Author: kosuke
 */

#ifndef DACVTR_H_
#define DACVTR_H_

namespace DAConverter {

namespace DAChannel {
enum Type {
    CH_0 = 0,
    CH_1 = 1,
    CH_2 = 2,
    CH_3 = 3
};
}

namespace OutputGain {
enum Type {
    GAIN_2X = 0,
    GAIN_1X = 1
};
}

namespace Result {
enum Type {
    OK = 0,
    ERROR = 1,
    INVALID = 2
};
}

class DACHandler {
private:
    short data[4];
public:
    DACHandler(void);
    Result::Type setup(void);
    DACHandler* setData(DAChannel::Type ch, short data);
    void commit(void);
};

}

#endif /* DACVTR_H_ */
