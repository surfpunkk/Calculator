#include "event_handlers.h"

bool result_shown = false;
void EventHandlers::handle_input(GtkWidget *entry, const char *input) {
    static std::string expression;

    if (g_strcmp0(input, "C") == 0) {
        expression.clear();
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    } else {
        expression += input;
        gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
    }
}
