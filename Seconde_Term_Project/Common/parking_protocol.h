#ifndef PARKING_PROTOCOL_H_
#define PARKING_PROTOCOL_H_

#include <stdint.h>
#include "system_types.h"


/* =========================================================
 * UART Protocol Configuration
 * ========================================================= */

#define PARKING_PROTOCOL_SOF             0xAAU
#define PARKING_PROTOCOL_FRAME_SIZE      12U

#define PARKING_PROTOCOL_TYPE_INDEX      1U
#define PARKING_PROTOCOL_SOURCE_INDEX    2U
#define PARKING_PROTOCOL_USER_ID_INDEX   3U
#define PARKING_PROTOCOL_RFID_UID_INDEX  7U
#define PARKING_PROTOCOL_CHECKSUM_INDEX  11U


/* =========================================================
 * Message Types
 * ========================================================= */

typedef enum
{
    MSG_NONE = 0,

    MSG_AUTH_ID_REQ,
    MSG_AUTH_RFID_REQ,

    MSG_ID_VALID,
    MSG_INVALID_ID,

    MSG_WRONG_CARD,

    MSG_ACCESS_GRANTED,
    MSG_ACCESS_DENIED,

    MSG_PARKING_FULL,
    MSG_ALREADY_INSIDE,
    MSG_INVALID_EXIT,

    MSG_ENTRY_COMPLETED,
    MSG_EXIT_COMPLETED

} MessageType_t;


/* =========================================================
 * Logical Application Packet
 * ========================================================= */

typedef struct
{
    MessageType_t type;

    ECU_Source_t source;

    uint32_t userID;
    uint32_t rfidUID;

} ParkingPacket_t;


#endif /* PARKING_PROTOCOL_H_ */