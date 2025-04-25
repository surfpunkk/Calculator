#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include <gtk/gtk.h>
#include "calculator_core.h"

class EventHandlers {
public:
    static void handle_clear(GtkEntry *entry);
    static void handle_equals(GtkEntry *entry);
    static void handle_backspace(GtkEntry* entry);
    static void handle_factorial(GtkEntry *entry);
    static void handle_power(GtkEntry *entry);
    static void handle_default(GtkEntry *entry, const char *input);
    static void handle_input(GtkWidget *entry, const char *input);
private:
    static std::string expression;
    static void update_expression(GtkEntry* entry);
};

#endif //EVENT_HANDLERS_H