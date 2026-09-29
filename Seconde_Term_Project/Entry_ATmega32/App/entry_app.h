/******************************************************************************
 * @file        entry_app.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       Application Layer
 * @module      Entry Application
 *
 * @path        Entry_ATmega32/App/entry_app.h
 *
 * @brief
 * Public interface for the Entry ECU application state machine.
 ******************************************************************************/

#ifndef ENTRY_APP_H_
#define ENTRY_APP_H_


/******************************************************************************
 * Public APIs
 ******************************************************************************/

/**
 * @brief Initialize the Entry ECU application.
 */
void EntryApp_Init(void);


/**
 * @brief Execute one iteration of the Entry ECU state machine.
 *
 * This function shall be called continuously from the main loop.
 */
void EntryApp_Update(void);


#endif /* ENTRY_APP_H_ */