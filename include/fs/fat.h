#ifndef FAT_H
# define FAT_H

# include "fs/devfs.h"

typedef struct ebpb16
{
    uint8_t drvnum;
    uint8_t flags;
    uint8_t sig;
    uint32_t serial;
    uint8_t label[11];
    uint8_t sysid[8];
    uint8_t bootcode[448];
    uint16_t partsig;
}__attribute__((packed)) s_ebpb16;

typedef struct ebpb32
{
    uint32_t secperfat;
    uint16_t flags;
    uint16_t fatversion;
    uint32_t rootcluster;
    uint16_t fsinfosec;
    uint16_t backupsec;
    uint8_t reserved[12];
    uint8_t drvnum;
    uint8_t ntflags;
    uint8_t sig;
    uint32_t serial;
    uint8_t label[11];
    uint8_t sysid[8];
    uint8_t bootcode[420];
    uint16_t partsig;
}__attribute__((packed)) s_ebpb32;

typedef union ebpb
{
    s_ebpb32 ebpb32;
    s_ebpb16 ebpb16;
} u_ebpb;

typedef struct bpb
{
    uint8_t bootjp[3];
    uint8_t oemname[8];
    uint16_t bpersec;
    uint8_t nosecperc;
    uint16_t noressec;
    uint8_t nofat;
    uint16_t nodir;
    uint16_t totalsec;
    uint8_t desctype;
    uint16_t nosecperfat;
    uint16_t nosecpertrack;
    uint16_t noheads;
    uint32_t nohidd;
    uint32_t laamosec;
    u_ebpb   ebpb;
}__attribute__((packed)) s_bpb;

typedef struct fat
{
    s_device *device;
    uint16_t seclen;
    uint8_t cluslen;
    uint8_t nbfats;
    uint8_t fatbits;
    uint32_t fatlen;
    uint32_t firstfat;
    uint32_t root;
} s_fat;

#endif /* !FAT_H */
