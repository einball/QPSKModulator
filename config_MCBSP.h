/*
 * config_MCBSP.h
 *
 *  Created on: 20.10.2013
 *      Author: einball
 */

#ifndef CONFIG_MCBSP_H_
#define CONFIG_MCBSP_H_

#include <stdint.h>
#include <csl.h>
#include <csl_mcbsp.h>

extern MCBSP_Handle hMcBsp;


void conf_MCBSP();
void start_MCBSP();

#endif /* CONFIG_MCBSP_H_ */
