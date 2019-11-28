/*
 * ap.h
 *
 *  Created on: 2018. 8. 25.
 *      Author: Baram
 */

#ifndef SRC_AP_AP_H_
#define SRC_AP_AP_H_


#include "hw.h"

#include "cmd/cmd.h"
#include "boot/boot.h"


void apInit(void);
void apMain(int argc, char *argv[]);


#endif /* SRC_AP_AP_H_ */
