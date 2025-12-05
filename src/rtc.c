#include "rtc.h"
#include "io.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

static uint8_t get_rtc_register(int reg) {
  outb(CMOS_ADDRESS, reg);
  return inb(CMOS_DATA);
}

static uint8_t bcd2bin(uint8_t bcd) { return ((bcd >> 4) * 10) + (bcd & 0x0F); }

void rtc_init() {
  // No initialization needed for basic reading
}

void rtc_get_time(rtc_time_t *time) {
  // Wait for update in progress to finish (Bit 7 of Register A)
  // while (get_rtc_register(0x0A) & 0x80); // Can hang if RTC broken, skip for
  // now

  time->second = get_rtc_register(0x00);
  time->minute = get_rtc_register(0x02);
  time->hour = get_rtc_register(0x04);
  time->day = get_rtc_register(0x07);
  time->month = get_rtc_register(0x08);
  time->year = get_rtc_register(0x09);

  uint8_t registerB = get_rtc_register(0x0B);

  // Convert BCD to binary values if necessary
  if (!(registerB & 0x04)) {
    time->second = bcd2bin(time->second);
    time->minute = bcd2bin(time->minute);
    time->hour = bcd2bin(time->hour);
    time->day = bcd2bin(time->day);
    time->month = bcd2bin(time->month);
    time->year = bcd2bin(time->year);
  }

  // Convert 12 hour clock to 24 hour clock if necessary
  if (!(registerB & 0x02) && (time->hour & 0x80)) {
    time->hour = ((time->hour & 0x7F) + 12) % 24;
  }
}
