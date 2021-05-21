#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <avr/pgmspace.h>

extern "C" {
#include "usbdrv.h"
#include "osccal.h"
}

#define RELAY_BIT PB3

constexpr uint8_t TIMER_FREQUENCY_HZ = 100; // that is interrupt every 10 ms
constexpr uint16_t RELAY_SHUTDOWN_TIMEOUT_MS = 5000;

static inline void enableRelay() {
    PORTB |= _BV(RELAY_BIT);
}

static inline void disableRelay() {
    PORTB &= ~_BV(RELAY_BIT);
}

static volatile uint16_t ticksToShutdown = 0;
static volatile uint8_t previousUsbSofCount = 0;

ISR(TIM0_COMPA_vect, ISR_NOBLOCK) {
    if (usbSofCount != previousUsbSofCount) {
        previousUsbSofCount = usbSofCount;
        enableRelay();
        ticksToShutdown = ((double)RELAY_SHUTDOWN_TIMEOUT_MS / 1000.0 * TIMER_FREQUENCY_HZ);
    } else {
        if (ticksToShutdown > 0) {
            --ticksToShutdown;
        } else {
            disableRelay();
        }
    }
}

usbMsgLen_t usbFunctionSetup(uint8_t[8]) {
    return 0;
}

int main() {
    DDRB |= _BV(RELAY_BIT);
    disableRelay();

    wdt_enable(WDTO_1S);

    usbInit();
    usbDeviceDisconnect();

    uint8_t i = 0;
    while (--i) {
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();

    TCCR0A = _BV(WGM01); // CTC-mode
    TCCR0B = _BV(CS02) | _BV(CS00); // prescaler to 1024
    OCR0A = (F_CPU / 1024 / TIMER_FREQUENCY_HZ);
    TIMSK = _BV(OCIE0A);

    sei();

    while (true) {
        wdt_reset();
        usbPoll();
    }
}
