/*
 * DjyOS V1.1.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * Minimal replacement of the black box header for the RVM guest
 * build: the hypervisor's Sftd daemon already captures and recovers
 * every guest fault, so the in-guest black box is compiled out.
 */
#ifndef __BLACKBOX_H__
#define __BLACKBOX_H__

enum EN_BlackBoxAction
{
    EN_BLACKBOX_DEAL_IGNORE = 0,
    EN_BLACKBOX_DEAL_DEFAULT,
    EN_BLACKBOX_DEAL_RECORD,
    EN_BLACKBOX_DEAL_RESET,
    EN_BLACKBOX_DEAL_REBOOT
};

struct BlackBoxThrowPara
{
    void *DecoderName;
    unsigned long BlackBoxAction;
    unsigned char *BlackBoxInfo;
    unsigned long BlackBoxInfoLen;
    unsigned long BlackBoxType;
};

#define EN_BLACKBOX_DEAL_RECORD   0
#define CN_BLACKBOX_TYPE_SCH_DISABLE_INT 0

static inline void BlackBox_ThrowExp(struct BlackBoxThrowPara *para)
{
    (void)para;
}

void DJY_NullFunc(void);

#endif /* __BLACKBOX_H__ */
