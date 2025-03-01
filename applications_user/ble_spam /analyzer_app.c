#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <input/input.h>
#include <furi_hal_bt.h>
#include <furi/core/thread.h>

#define MAX_SIGNALS 10
#define SIGNAL_STR_SIZE 32
#define SCAN_INTERVAL_MS 1000  // Simulated scan interval in milliseconds

static char scanned_signals[MAX_SIGNALS][SIGNAL_STR_SIZE];
static uint32_t num_signals = 0;
static bool running = true;

// Simulated scanning task that logs detected signals
int32_t scan_task(void* p) {
    UNUSED(p);
    FURI_LOG_I("SCAN", "Scan task started");
    while(running) {
        if(num_signals < MAX_SIGNALS) {
            // Use %lu for both values
            snprintf(scanned_signals[num_signals], SIGNAL_STR_SIZE,
                     "Signal %lu @ %lu", (unsigned long)(num_signals + 1), (unsigned long)furi_get_tick());
            num_signals++;
            FURI_LOG_I("SCAN", "Detected signal %lu", (unsigned long)num_signals);
        } else {
            // Shift log entries to remove oldest
            for(uint32_t i = 1; i < MAX_SIGNALS; i++) {
                strncpy(scanned_signals[i - 1], scanned_signals[i], SIGNAL_STR_SIZE);
            }
            snprintf(scanned_signals[MAX_SIGNALS - 1], SIGNAL_STR_SIZE,
                     "Signal %lu @ %lu", (unsigned long)(num_signals + 1), (unsigned long)furi_get_tick());
            num_signals++;
            FURI_LOG_I("SCAN", "Log full; added new signal (oldest dropped)");
        }
        furi_delay_ms(SCAN_INTERVAL_MS);
    }
    FURI_LOG_I("SCAN", "Scan task stopped");
    return 0;
}

// Input callback to allow exiting the app
void input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->type == InputTypeShort && event->key == InputKeyBack) {
        running = false;
        FURI_LOG_I("INPUT", "Back pressed, exiting app");
    }
}

// Render callback: display the log of detected signals
void render_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_clear(canvas);
    canvas_draw_str(canvas, 0, 10, "Multi-Protocol Analyzer");
    canvas_draw_str(canvas, 0, 20, "Detected Signals:");
    
    uint32_t y = 30;
    for(uint32_t i = 0; i < MAX_SIGNALS && i < num_signals; i++) {
        canvas_draw_str(canvas, 0, y, scanned_signals[i]);
        y += 10;
    }
    
    canvas_draw_str(canvas, 0, y + 10, "Press BACK to exit");
}

// Main entry point for the app
int32_t analyzer_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("ANALYZER", "Analyzer App Started");
    
    // Open the GUI record
    Gui* gui = furi_record_open(RECORD_GUI);
    
    // Allocate and configure the viewport
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, NULL);
    
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    
    // Start the simulated scan task in a separate thread
    FuriThread* scan_thread = furi_thread_alloc_ex("Scan_Thread", 1024, scan_task, NULL);
    furi_thread_start(scan_thread);
    
    // Main loop: update UI until the user exits
    while(running) {
        furi_delay_ms(100);
    }
    
    // Cleanup resources
    furi_thread_free(scan_thread);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    
    FURI_LOG_I("ANALYZER", "Analyzer App Exited");
    return 0;
}
