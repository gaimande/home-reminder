typedef struct time_struct
{
   unsigned char   seconds;
   unsigned char   minutes;
   unsigned char   hours;
   unsigned char   day;
   unsigned char   date;
   unsigned char   month;
   unsigned char   year;
   unsigned char   cal_cfg1;
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

/*Read_RTC
* Read RTC from specific address
* INPUT: unsigned char
* RETURN: RTC_TIME
*/
void Read_all_RTC(RTC_TIME* time);

/*Write_RTC
* Write to RTC from specific address
* INPUT: unsigned
* RETURN: None
*/
void Write_RTC(RTC_TIME* time);

/*CAL_RTC
* Calibrate the RTC
* INPUT: None
* RETURN: None
*/
void CAL_RTC(void);

const char* month_name(int n);