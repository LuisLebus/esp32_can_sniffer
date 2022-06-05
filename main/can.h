/**
 * @file can.h
 * @author
 * @date
 * @brief
 */

#ifndef _CAN_H_
#define _CAN_H_

//=============================================================================
// [Inclusions] ===============================================================

//=============================================================================

//=============================================================================
// [External Defines] =========================================================

//=============================================================================

//=============================================================================
// [External Typedef] =========================================================

//=============================================================================

//=============================================================================
// [External Data Definitions] ================================================

// Const ---------------------------------------------
//----------------------------------------------------

// Vars ----------------------------------------------
//----------------------------------------------------

// Task Handlers -------------------------------------
//----------------------------------------------------

// Queue Handlers ------------------------------------
//----------------------------------------------------

// Event Group Handlers ------------------------------
//----------------------------------------------------

// Semaphore Handlers --------------------------------
//----------------------------------------------------

//=============================================================================

//=============================================================================
// [External Function Declarations] ===========================================

/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
void can_init(void);

/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
bool can_read(twai_message_t * twai_message, TickType_t ticks_to_wait);

//=============================================================================

#endif /* _CAN_H_ */