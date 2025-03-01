#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_bt.h>
#include <furi_thread.h>

#define DEFAULT_SPAM_RATE 500

static bool running = true;

void ble_spam_task(void* p) {
    UNUSED(p);
    while(running) {
        // Send BLE advertisement. (Ensure this API is available in your SDK.)
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(DEFAULT_SPAM_RATE);
    }
}

int32_t ble_spam_app(void* p) {
    UNUSED(p);
    // Open the GUI record (even if we don't actively use it).
    Gui* gui = furi_record_open(RECORD_GUI);

    running = true;
    // Create a new thread to run the BLE spamming task.
    furi_thread_create(ble_spam_task, NULL, 1024);

    // Main loop – you can add conditions to stop the loop if needed.
    while(running) {
        furi_delay_ms(100);
    }
    
    furi_record_close(RECORD_GUI);
    return 0;
}
