/**
 ****************************************************************************************
 *
 * @file rwnx_msg_rx.h
 *
 * @brief RX function declarations
 *
 * Copyright (C) RivieraWaves 2012-2019
 *
 ****************************************************************************************
 */

#ifndef _RWNX_MSG_RX_H_
#define _RWNX_MSG_RX_H_

#include "rwnx_defs.h"
#include "ipc_shared.h"

void rwnx_rx_handle_rftest_msg(struct rwnx_hw *rwnx_hw, struct ipc_e2a_msg *msg);

#ifdef AICWF_USB_SUPPORT
#ifdef CONFIG_RFTEST
void rwnx_rx_handle_print(struct rwnx_hw *rwnx_hw, u8 *msg, u32 len);
#endif
#endif

#endif /* _RWNX_MSG_RX_H_ */
