#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <furi_hal_bt.h>
#include <furi_thread.h>

#define DEFAULT_SPAM_RATE 500  // Default BLE spam rate (ms)
#define MIN_SPAM_RATE 100      // Fastest speed (ms)
#define MAX_SPAM_RATE 2000     // Slowest speed (ms)

static uint32_t spam_rate = DEFAULT_SPAM_RATE;
static bool running = true;

// Function to handle input (button presses)
void input_callback(InputEvent* event, void* context) {
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyBack) {
            running = false;  // Stop the spam and exit
        } else if(event->key == InputKeyLeft) {
            if(spam_rate < MAX_SPAM_RATE) spam_rate += 100;  // Slower spam
        } else if(event->key == InputKeyRight) {
            if(spam_rate > MIN_SPAM_RATE) spam_rate -= 100;  // Faster spam
        }
    }
}

// The BLE spam task runs in a separate thread
void ble_spam_task(void* p) {
    UNUSED(p);
    while(running) {
        furi_hal_bt_send_adv("Flipper_BLE_Spam", 16, true);
        furi_delay_ms(spam_rate);
    }
}

// UI rendering function
void render_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_draw_str(canvas, 10, 10, "BLE Spam Active");
    
    char spam_text[32];
    snprintf(spam_text, sizeof(spam_text), "Rate: %d ms", spam_rate);
    canvas_draw_str(canvas, 10, 30, spam_text);

    canvas_draw_str(canvas, 10, 50, "⬅ Slower  ➡ Faster");
    canvas_draw_str(canvas, 10, 60, "🔙 Exit");
}

// Main app entry point
int32_t ble_spam_app(void* p) {
    UNUSED(p);

    // Open GUI and input system
    Gui* gui = furi_record_open(RECORD_GUI);
    Input* input = furi_record_open(RECORD_INPUT);
    
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, NULL);
    
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    input_set_callback(input, input_callback, NULL);

    // Start BLE spam in a separate thread
    FuriThread* spam_thread = furi_thread_alloc();
    furi_thread_set_stack_size(spam_thread, 1024);
    furi_thread_set_callback(spam_thread, ble_spam_task);
    furi_thread_start(spam_thread);

    // Main loop
    while(running) {
        furi_delay_ms(100);
    }

    // Cleanup: stop thread, remove UI, and close resources
    furi_thread_free(spam_thread);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT);
    
    return 0;
}
