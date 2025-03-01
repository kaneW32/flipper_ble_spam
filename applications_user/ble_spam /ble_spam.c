#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_bt.h>
// Removed: #include <furi_thread.h>

#define DEFAULT_SPAM_RATE 500

// Fallback stub for furi_hal_bt_send_adv.
// If your SDK does not provide this function, the fallback marks parameters as unused.
#ifndef furi_hal_bt_send_adv
void furi_hal_bt_send_adv(const char* name, uint32_t len, bool enable) {
    UNUSED(name);
    UNUSED(len);
    UNUSED(enable);
    // Fallback does nothing.
}
#endif

static bool running = true;

// The BLE spam task: continuously sends BLE advertisements.
void ble_spam_task(void* p) {
    UNUSED(p);
    while(running) {
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(DEFAULT_SPAM_RATE);
    }
}

// The main entry point for the app.
// Note: Running the task directly on the main thread as threading isn't available.
int32_t ble_spam_app(void* p) {
    UNUSED(p);
    Gui* gui = furi_record_open(RECORD_GUI);
    UNUSED(gui);
    
    // Run the BLE spam task directly on the main thread.
    ble_spam_task(NULL);
    
    furi_record_close(RECORD_GUI);
    return 0;
}
