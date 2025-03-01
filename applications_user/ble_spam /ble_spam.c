#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_bt.h>
// Removed: #include <furi_thread.h>

#define DEFAULT_SPAM_RATE 500

static bool running = true;

void ble_spam_task(void* p) {
    UNUSED(p);
    while(running) {
        // Send BLE advertisement (ensure your SDK supports this function)
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(DEFAULT_SPAM_RATE);
    }
}

int32_t ble_spam_app(void* p) {
    UNUSED(p);
    Gui* gui = furi_record_open(RECORD_GUI);
    
    // Create a new thread to run the BLE spam task.
    // If furi_thread_create is declared in furi.h in your SDK, this should work.
    furi_thread_create(ble_spam_task, NULL, 1024);
    
    // Main loop – you could add conditions to stop the loop
    while(running) {
        furi_delay_ms(100);
    }
    
    furi_record_close(RECORD_GUI);
    return 0;
}
