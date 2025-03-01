#include <furi.h>
#include <gui/gui.h>
#include <furi_hal_bt.h>

// Fallback declaration in case furi_hal_bt_send_adv is missing in your SDK.
// You can remove this if your SDK actually provides it.
#ifndef furi_hal_bt_send_adv
void furi_hal_bt_send_adv(const char* name, uint32_t len, bool enable) {
    FURI_LOG_F("BLE", "furi_hal_bt_send_adv called with name: %s", name);
}
#endif

#define DEFAULT_SPAM_RATE 500

static bool running = true;

void ble_spam_task(void* p) {
    UNUSED(p);
    while(running) {
        // Attempt to send a BLE advertisement
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(DEFAULT_SPAM_RATE);
    }
}

int32_t ble_spam_app(void* p) {
    UNUSED(p);
    // Open the GUI record (even if not actively used)
    Gui* gui = furi_record_open(RECORD_GUI);
    
    // Run the BLE spam task directly on the main thread.
    // This is a temporary workaround until proper threading support is integrated.
    ble_spam_task(NULL);
    
    // Cleanup and return (unreachable in this simple infinite loop, but good practice)
    furi_record_close(RECORD_GUI);
    return 0;
}
