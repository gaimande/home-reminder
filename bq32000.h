typedef struct time_struct
{
   uint8   seconds;
   uint8   minutes;
   uint8   hours;
   uint8   day;
   uint8   date;
   uint8   month;
   uint8   year;
   uint8   cal_cfg1;
} RTC_TIME;

/*ConfigI2C
* Sets up the I2C interface via USCI
* INPUT: None
* RETURN: None
*/
void ConfigI2C(void);

/*Read_RTC
* Read RTC from specific address
* INPUT: unsigned char
* RETURN: unsigned char
*/
unsigned char Read_RTC(unsigned char);

/*Wirte_RTC
* Write to RTC from specific address
* INPUT: unsigned
* RETURN: None
*/
void Wirte_RTC(unsigned char *);

/*CAL_RTC
* Calibrate the RTC
* INPUT: None
* RETURN: None
*/
void CAL_RTC(void);