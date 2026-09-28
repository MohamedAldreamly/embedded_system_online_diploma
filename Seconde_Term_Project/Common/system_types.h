#ifndef SYSTEM_TYPES_H_
#define SYSTEM_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

/* =========================
 * System Configuration
 * ========================= */

#define PARKING_CAPACITY        3U
#define MAX_REGISTERED_USERS   20U
#define MAX_RFID_ATTEMPTS       3U
#define GATE_TIMEOUT_SEC       10U


/* =========================
 * ECU Source
 * ========================= */

typedef enum
{
    SOURCE_NONE  = 0,
    SOURCE_ENTRY = 1,
    SOURCE_EXIT  = 2

} ECU_Source_t;


/* =========================
 * User Parking State
 * ========================= */

typedef enum
{
    USER_OUTSIDE = 0,
    USER_INSIDE  = 1

} UserState_t;


/* =========================
 * Registered User
 * ========================= */

typedef struct
{
    uint32_t    userID;
    uint32_t    rfidUID;
    UserState_t state;

} User_t;


/* =========================
 * Access Results
 * ========================= */

typedef enum
{
    ACCESS_RESULT_NONE = 0,

    ACCESS_ID_VALID,
    ACCESS_INVALID_ID,

    ACCESS_WRONG_CARD,

    ACCESS_GRANTED,
    ACCESS_DENIED,

    ACCESS_PARKING_FULL,
    ACCESS_ALREADY_INSIDE,
    ACCESS_INVALID_EXIT

} AccessResult_t;

#endif /* SYSTEM_TYPES_H_ */