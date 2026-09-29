/******************************************************************************
 * @file        exit_app.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Exit ECU
 * @mcu         ATmega32
 * @layer       Application Layer
 * @module      Exit Application
 *
 * @path        Exit_ATmega32/App/exit_app.h
 *
 * @brief
 * Public interface for the Exit ECU application state machine.
 ******************************************************************************/

#ifndef EXIT_APP_H_
#define EXIT_APP_H_

void ExitApp_Init(void);
void ExitApp_Update(void);

#endif /* EXIT_APP_H_ */
