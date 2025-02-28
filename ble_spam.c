#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <furi_hal_bt.h>
#include <furi_thread.h>

#define DEFAULT_SPAM_RATE 500

static uint32_t spam_rate = DEFAULT_SPAM_RATE;
static bool running = true;

void ble_spam_task(void* p) {
    UNUSED(p);
    while (running) {
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(spam_rate);
    }
}

void input_callback(InputEvent* event, void* context) {
    UNUSED(context);
}

int32_t ble_spam_app(void* p) {
    UNUSED(p);
    Gui* gui = furi_record_open(RECORD_GUI);
    FuriInput* input = furi_record_open(RECORD_INPUT);
    input_set_callback(input, input_callback, NULL);
    
    running = true;
    furi_thread_create(ble_spam_task, NULL, 1024);
    
    while (running) {
        furi_delay_ms(100);
    }
    
    furi_record_close(RECORD_INPUT);
    furi_record_close(RECORD_GUI);
    return 0;
}
