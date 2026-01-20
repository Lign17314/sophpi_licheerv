/**
 * aicwf_usb.h
 *
 * USB function declarations
 *
 * Copyright (C) AICSemi 2018-2020
 */

#ifndef _AICWF_USB_H_
#define _AICWF_USB_H_

#include <linux/usb.h>
#include <linux/netdevice.h>

#ifdef AICWF_USB_SUPPORT

#ifdef CONFIG_RFTEST
#include "rwnx_cmds.h"
#endif

/* USB Device ID */
#define USB_VENDOR_ID_AIC               0xA69C
#define USB_PRODUCT_ID_AIC              0x88E0

#define USB_PRODUCT_ID_AIC8801          0x8801
#define USB_PRODUCT_ID_AIC8800DC        0x88dc
#define USB_PRODUCT_ID_AIC8800DW        0x88dd
#define USB_PRODUCT_ID_AIC8800D81       0x8d81
#define USB_PRODUCT_ID_AIC8800D41       0x8d41
#define USB_PRODUCT_ID_AIC8800D81X2     0x8d91
#define USB_PRODUCT_ID_AIC8800D89X2     0x8d99

#define AICWF_USB_RX_URBS               (200)
#define AICWF_USB_TX_URBS               (100)
#define AICWF_USB_MSG_RX_URBS           (100)
#define AICWF_USB_TX_LOW_WATER          (AICWF_USB_TX_URBS/4)
#define AICWF_USB_TX_HIGH_WATER         (AICWF_USB_TX_LOW_WATER*3)
#define AICWF_USB_MAX_PKT_SIZE          (2048)
#define AICWF_USB_MSG_MAX_PKT_SIZE      (2048)


typedef enum {
    USB_TYPE_DATA         = 0X00,
    USB_TYPE_CFG          = 0X10,
    USB_TYPE_CFG_CMD_RSP  = 0X11,
    USB_TYPE_CFG_DATA_CFM = 0X12,
    USB_TYPE_CFG_PRINT    = 0x13,
} usb_type;

enum aicwf_usb_state {
    USB_DOWN_ST,
    USB_UP_ST,
    USB_SLEEP_ST
};

struct aicwf_usb_buf {
    struct list_head list;
    struct aic_usb_dev *usbdev;
    struct urb *urb;
    struct sk_buff *skb;
    bool cfm;
};

struct aic_usb_dev {
    struct aicwf_bus *bus_if;
    struct usb_device *udev;
    struct device *dev;
    struct aicwf_rx_priv* rx_priv;
    enum aicwf_usb_state state;

    #ifdef CONFIG_RFTEST
    struct rwnx_hw *rwnx_hw;
    struct rwnx_cmd_mgr cmd_mgr;
    #endif

    struct usb_anchor rx_submitted;
    struct work_struct rx_urb_work;
    #ifdef CONFIG_RFTEST
    struct usb_anchor msg_rx_submitted;
    struct work_struct msg_rx_urb_work;
    #endif

    spinlock_t rx_free_lock;
    spinlock_t tx_free_lock;
    spinlock_t tx_post_lock;
    spinlock_t tx_flow_lock;
    #ifdef CONFIG_RFTEST
    spinlock_t msg_rx_free_lock;
    #endif

    struct list_head rx_free_list;
    struct list_head tx_free_list;
    struct list_head tx_post_list;
    #ifdef CONFIG_RFTEST
    struct list_head msg_rx_free_list;
    #endif

    uint bulk_in_pipe;
    uint bulk_out_pipe;
    #ifdef CONFIG_RFTEST
    uint msg_in_pipe;
    uint msg_out_pipe;
    #endif

    int tx_free_count;
    int tx_post_count;

    struct aicwf_usb_buf usb_tx_buf[AICWF_USB_TX_URBS];
    struct aicwf_usb_buf usb_rx_buf[AICWF_USB_RX_URBS];
    #ifdef CONFIG_RFTEST
    struct aicwf_usb_buf usb_msg_rx_buf[AICWF_USB_MSG_RX_URBS];
    #endif

    int msg_finished;
    wait_queue_head_t msg_wait;
    ulong msg_busy;
    struct urb *msg_out_urb;

    bool tbusy;
    u16 chipid;
};

extern void aicwf_usb_exit(void);
extern void aicwf_usb_register(void);
extern void aicwf_usb_tx_flowctrl(struct net_device *ndev, bool state);
int usb_bustx_thread(void *data);
int usb_busrx_thread(void *data);
#ifdef CONFIG_RFTEST
int usb_msg_busrx_thread(void *data);
#endif
extern void aicwf_hostif_ready(void);

#endif /* AICWF_USB_SUPPORT */
#endif /* _AICWF_USB_H_       */
