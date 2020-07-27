#include "mbed.h"
#include "Rtc-Ds1307.h"
#include <telemetry_telecontrol.h>

const char *Rtc_Ds1307::m_weekDays[] = { "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday" };    


Rtc_Ds1307::Rtc_Ds1307(PinName sda, PinName scl)
{
    //  Create a new I2C object
    m_rtc = new I2C(sda, scl);
        
    // Set the frequency to standard 100kHz
    m_rtc->frequency(100000);
}

Rtc_Ds1307::~Rtc_Ds1307()
{
    if (m_rtc != NULL)
        delete m_rtc;
}

bool Rtc_Ds1307::setTime(Time_rtc& time, bool start, bool thm)
{
    char buffer[7];
    if (!read(0,buffer,7)) {
        return false;
    }
    //  Now update only the time part (saving the existing flags)
    if (start) { buffer[0] &= 0x7F; } else { buffer[0] |= 0x80; }
    buffer[0] = (buffer[0]&0x80) | (decimalToBcd(time.sec)& 0x7f);
    buffer[1] = decimalToBcd(time.min);
    if (thm) {
        //  AM PM format
        buffer[2] = (buffer[2]& 196) | (time.hour>12 ? (0x20 | ((decimalToBcd(time.hour-12)))) : decimalToBcd(time.hour));
    }
    else {
        // 24 hours format
        buffer[2] = (buffer[2]& 196) | (decimalToBcd(time.hour) & 0x3F);
    }
    buffer[3] = time.wday;
    buffer[4] = decimalToBcd(time.date);
    buffer[5] = decimalToBcd(time.mon);
    buffer[6] = decimalToBcd(time.year-2000);
    if (!write(0, buffer, 7) ) {
        return false;
    }
    return true;
}

bool Rtc_Ds1307::getTime(Time_rtc& time)
{
    char buffer[7];
    bool thm = false;
    
    if (!read(0, buffer, 7) ) {
        //  Failed to read
        return false;
    }
    thm = ((buffer[2] & 64) == 64);
    time.sec = bcdToDecimal(buffer[0]&0x7F);
    time.min = bcdToDecimal(buffer[1]);
    if (thm) {
        // in 12-hour-mode, we need to add 12 hours if PM bit is set
        time.hour = Rtc_Ds1307::bcdToDecimal( buffer[2] & 31 );
        if ((buffer[2] & 32) == 32)
            time.hour += 12;
    }
    else {
        time.hour = Rtc_Ds1307::bcdToDecimal( buffer[2] & 63 );
    }  
    time.wday = buffer[3]; 
    time.date = Rtc_Ds1307::bcdToDecimal( buffer[4]);
    time.mon = Rtc_Ds1307::bcdToDecimal( buffer[5]);
    time.year = Rtc_Ds1307::bcdToDecimal(buffer[6]) + 2000;   //  plus hundret is because RTC is giving the years since 2000, but std c struct tm needs years since 1900
    
    return true;
}


bool Rtc_Ds1307::startClock()
{
    char strtStop;
    
    if (!read(0, &strtStop, 1)) {
        return false;
    }
    
    strtStop &= 0x7F;
    
    if (!write(0, &strtStop, 1)) {
        return false;
    }
    
    return true;
}

bool Rtc_Ds1307::stopClock()
{
    char strtStop;
    
    if (!read(0, &strtStop, 1)) {
        return false;
    }
    
    strtStop |= 0x80;
    
    if (!write(0, &strtStop, 1)) {
        return false;
    }
    
    return true;
}

bool Rtc_Ds1307::setSquareWaveOutput(bool ena, SqwRateSelect_t rs)
{
    char reg;
    
    if (!read(7,&reg, 1)) {
        return false;
    }
    
    //  preserve the OUT control bit while writing the frequency and enable bits
    reg = (reg & 0x80) | (ena ? 0x10 : 0) | ((char)rs & 0x03);

    
    if (!write(7, &reg, 1)) {
        return false;
    }
    
    return true;
}



bool Rtc_Ds1307::read(int address, char *buffer, int len)
{
    char buffer2[2] = {(char)address, 0};
    
//    m_rtc->start();
    if (m_rtc->write(0xd0, buffer2, 1) != 0) {
        m_rtc->stop();
        return false;
    }
    if (m_rtc->read(0xd0, buffer, len) != 0) {
        return false;
    }
    m_rtc->stop();
    
    return true;
}

bool Rtc_Ds1307::write(int address, char *buffer, int len)
{
    char buffer2[10];
    buffer2[0] = address&0xFF;
    for (int i = 0 ; i < len ; i++)
        buffer2[i+1] = buffer[i];

//    m_rtc->start();        
    if (m_rtc->write(0xd0, buffer2, len+1) != 0) {
        m_rtc->stop();
        return false;
    }
    m_rtc->stop();
    return true;
}




RtcCls::RtcCls(PinName sda, PinName scl, PinName sqw, bool bUseSqw)
    : Rtc_Ds1307(sda, scl), m_sqw(sqw), m_bUseSqw(bUseSqw), m_bAlarmEnabled(false), m_alarmfunc(NULL)
{
    Time_rtc t;
    //  query time from device
    getTime(t);
    //  sync the time with MBED RTC
    struct tm now = {t.sec, t.min, t.hour, t.date, t.mon-1, t.year-1900};
    m_time = mktime(&now);
    set_time(m_time);
    
    //  Only register the callback and start the SQW if requested to do so. Otherwise the system
    //  will use the MBED built-in RTC.
    if (m_bUseSqw) {
        //  start the wave
        setSquareWaveOutput(true, RS1Hz);
        //  register callback from now on the time will be maintained by the square wave input
        m_sqw.rise(this, &RtcCls::_callback);
    }
}

void RtcCls::_callback(void)
{
//    INFO("Tick!");
    //  Simply increase the number of seconds
    m_time++;
//    if (m_bAlarmEnabled && (m_time == m_alarmTime)) {
//        if (m_alarmfunc != NULL)
//            m_alarmfunc();
//        m_bAlarmEnabled = false;
//    }
}

time_t RtcCls::getTime()
{
    //  when not using the HW support, we have to query the RTC chip. Other wise we can just return out stored value
    if (!m_bUseSqw) {
        Time_rtc t;
        getTime(t);
        struct tm now = {t.sec, t.min, t.hour, t.date, t.mon-1, t.year-1900};
        m_time = mktime(&now);
    }
    else {
    }
    return m_time;
}

void RtcCls::setTime(time_t t)
{
    Time_rtc tim;
    struct tm *now;
    now = localtime(&t);

    tim.sec = now->tm_sec;
    tim.min = now->tm_min;
    tim.hour = now->tm_hour;
    tim.date = now->tm_mday;
    tim.mon = now->tm_mon+1;
    tim.year = now->tm_year + 1900;
    tim.wday = now->tm_wday +1;

    setTime( tim, true, true);
    set_time(t);
}
