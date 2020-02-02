#ifndef __CMOS_H__
#define __CMOS_H__

#include <stdint.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71
#define CURRENT_YEAR 2020

struct date_struct_t {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
};

typedef date_struct_t date_t;

void read_rtc(void);
void register_date_command(void);

#endif