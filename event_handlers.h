#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include <gtk/gtk.h>
#include "calculator_core.h"

class EventHandlers {
public:
    static void handle_input(GtkWidget *entry, const char *input);
private:
    static std::string expression;
};

#endif //EVENT_HANDLERS_H