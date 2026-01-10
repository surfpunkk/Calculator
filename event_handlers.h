#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include <gtk/gtk.h>
#include "calculator_core.h"

inline std::stack<std::string> history_stack;
inline std::vector<std::string> history;

class EventHandlers {
public:
    static void write_history(std::string &result, const std::string &old_expression);
    static void insert_at_cursor(GtkEntry *entry, const std::string &input);
    static void handle_clear(GtkEntry *entry);
    static void handle_equals(GtkEntry *entry);
    static void handle_backspace(GtkEntry* entry);
    static void handle_factorial(GtkEntry *entry);
    static void handle_power(GtkEntry *entry);
    static void handle_default(GtkEntry *entry, const char *input);
    static void handle_input(GtkWidget *entry, const char *input);
private:
    static std::string expression;
    static gint start_pos, end_pos, cursor_pos;
    static void update_expression(GtkEntry* entry);
};

#endif //EVENT_HANDLERS_H