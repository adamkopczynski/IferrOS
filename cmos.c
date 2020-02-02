#include "cmos.h"
#include "shell.h"
#include "ports.h"

static int get_update_in_progress_flag();
static unsigned char get_RTC_register(int reg);

int century_register = 0x00;

date_t date;

void read_rtc(void){
    unsigned char century;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned char last_year;
    unsigned char last_century;
    unsigned char registerB;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
    while (get_update_in_progress_flag());  // Make sure an update isn't in progress

    date.second = get_RTC_register(0x00);
    date.minute = get_RTC_register(0x02);
    date.hour = get_RTC_register(0x04);
    date.day = get_RTC_register(0x07);
    date.month = get_RTC_register(0x08);
    date.year = get_RTC_register(0x09);
        
    if(century_register != 0) {
        century = get_RTC_register(century_register);
    }
 
      do {
            last_second = date.second;
            last_minute = date.minute;
            last_hour = date.hour;
            last_day = date.day;
            last_month = date.month;
            last_year = date.year;
            last_century = century;
 
            while (get_update_in_progress_flag());           // Make sure an update isn't in progress

            date.second = get_RTC_register(0x00);
            date.minute = get_RTC_register(0x02);
            date.hour = get_RTC_register(0x04);
            date.day = get_RTC_register(0x07);
            date.month = get_RTC_register(0x08);
            date.year = get_RTC_register(0x09);
            
            if(century_register != 0) {
                  century = get_RTC_register(century_register);
            }

      } while( (last_second != date.second) || (last_minute != date.minute) || (last_hour != date.hour) ||
               (last_day != date.day) || (last_month != date.month) || (last_year != date.year) ||
               (last_century != century) );
 
      registerB = get_RTC_register(0x0B);
 
      // Convert BCD to binary values if necessary
 
      if (!(registerB & 0x04)) {
            date.second = (date.second & 0x0F) + ((date.second / 16) * 10);
            date.minute = (date.minute & 0x0F) + ((date.minute / 16) * 10);
            date.hour = ( (date.hour & 0x0F) + (((date.hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            date.day = (date.day & 0x0F) + ((date.day / 16) * 10);
            date.month = (date.month & 0x0F) + ((date.month / 16) * 10);
            date.year = (date.year & 0x0F) + ((date.year / 16) * 10);
            if(century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }
 
      // Convert 12 hour clock to 24 hour clock if necessary
 
      if (!(registerB & 0x02) && (date.hour & 0x80)) {
            date.hour = ((date.hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
      if(century_register != 0) {
            date.year += century * 100;
      } else {
            date.year += (CURRENT_YEAR / 100) * 100;
            if(date.year < CURRENT_YEAR) date.year += 100;
      }
}

void register_date_command(void);

static int get_update_in_progress_flag() {
      outb(CMOS_ADDRESS, 0x0A);
      return (inb(CMOS_DATA) & 0x80);
}
 
static unsigned char get_RTC_register(int reg) {
      outb(CMOS_ADDRESS, reg);
      return inb(CMOS_DATA);
}