#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <furi_hal_bt.h>  // For furi_hal_bt_send_adv

#define DEFAULT_SPAM_RATE 500

static bool running = true;

int32_t ble_spam_app(void* p) {
    UNUSED(p);

    // Open necessary Flipper records
    Gui* gui = furi_record_open(RECORD_GUI);
    FuriInput* input = furi_record_open(RECORD_INPUT);
    // (Optional) set an input callback if you want to handle button presses
    // input_set_callback(input, your_input_callback, NULL);

    // Main loop
    while(running) {
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(DEFAULT_SPAM_RATE);
        // Possibly check for a condition to break out of the loop
    }

    // Cleanup
    furi_record_close(RECORD_INPUT);
    furi_record_close(RECORD_GUI);
    return 0;
}
