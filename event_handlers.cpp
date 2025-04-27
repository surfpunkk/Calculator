#include "event_handlers.h"

    std::string EventHandlers::expression;
    gint EventHandlers::cursor_pos = 0, EventHandlers::start_pos = 0, EventHandlers::end_pos = 0;

    void EventHandlers::write_history(const std::string &result, const std::string &old_expression) {
        history_stack.emplace(old_expression);
        std::string history_item = old_expression + " = " + result;
        history.push_back(history_item);
        if (history.size() > 50) {
            history.erase(history.begin());
        }
    }

    void EventHandlers::update_expression(GtkEntry *entry) {
        const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (expression != current_text) {
            expression = current_text;
        }
        if (g_strrstr(current_text, "Error:") != nullptr) {
            no_empty_state = true;
        }
    }

    void EventHandlers::handle_clear(GtkEntry *entry){
        no_empty_state = false;
        result_shown = false;
        expression.clear();
        gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
    }

    void EventHandlers::handle_equals(GtkEntry *entry) {
        const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
        std::string old_expression;
        if (expression.find(' ') != std::string::npos) {
            for (char c : expression) {
                if (c != ' ') {
                    old_expression += c;
                }
            }
        } else old_expression = expression;
        expression.clear();
        if (current_text[0] != '\0') {
            if (no_empty_state) return;
            const std::string result = CalculatorCore::calculate(current_text);
            if (no_empty_state) {
                gtk_entry_set_text(GTK_ENTRY(entry), result.c_str());
                no_empty_state = false;
            } else {
                gchar result_str[64];
                g_snprintf(result_str, sizeof(result_str), "%.15g", std::stod(result));
                std::string formatted_result = result_str;
                if (const size_t dot_pos = formatted_result.find(','); dot_pos != std::string::npos) {
                    if (const size_t last_non_zero = formatted_result.find_last_not_of('0'); last_non_zero > dot_pos) {
                        formatted_result.erase(last_non_zero + 1);
                    } else {
                        formatted_result.erase(dot_pos);
                    }
                }
                write_history(formatted_result, old_expression);
                expression += formatted_result;
                if (expression == "inf") expression = "∞";
                if (expression == "-inf") expression = "-∞";
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                result_shown = true;
            }
        } else {
            gtk_entry_set_text(GTK_ENTRY(entry), "Error: No result");
            no_empty_state = true;
        }
    }

    void EventHandlers::handle_backspace(GtkEntry *entry) {
        cursor_pos = gtk_editable_get_position(GTK_EDITABLE(entry));
        update_expression(entry);
        if (gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start_pos, &end_pos)) {
            gtk_editable_delete_selection(GTK_EDITABLE(entry));
        } else if (no_empty_state) {
            expression.clear();
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            no_empty_state = false;
        } else if (!expression.empty()) {
            icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(expression);
            if (cursor_pos > 0) ustr.remove(cursor_pos - 1, 1);
            else ustr.truncate(ustr.length() - 1);
            std::string newExpression;
            ustr.toUTF8String(newExpression);
            expression = newExpression;
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            gint new_cursor_pos = cursor_pos > 0 ? cursor_pos - 1 : -1;
            gtk_editable_set_position(GTK_EDITABLE(entry), new_cursor_pos);
        }
        result_shown = false;
    }

    void EventHandlers::handle_factorial(GtkEntry *entry) {
        if (no_empty_state) {
            expression = "!";
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            no_empty_state = false;
        } else {
            expression += "!";
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            result_shown = false;
        }
    }

    void EventHandlers::handle_power(GtkEntry *entry) {
        if (no_empty_state) {
            expression = "^";
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            no_empty_state = false;
        } else {
            expression += "^";
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            result_shown = false;
        }
    }

    void EventHandlers::handle_default(GtkEntry *entry, const char *input) {
        if (result_shown) {
            if (isdigit(input[0])) {
                result_shown = false;
                expression = input;
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            } else {
                result_shown = false;
                expression += input;
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            }
        } else if (no_empty_state) {
            expression = input;
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            no_empty_state = false;
        } else {
            expression += input;
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
        }
    }

    void EventHandlers::handle_input(GtkWidget *entry, const char *input) {

        EventHandlers::update_expression(GTK_ENTRY(entry));

        if (g_strcmp0(input, "C") == 0) {
            handle_clear(GTK_ENTRY(entry));
        } else if (g_strcmp0(input, "=") == 0) {
            handle_equals(GTK_ENTRY(entry));
        } else if (g_strcmp0(input, "⌫") == 0) {
            handle_backspace(GTK_ENTRY(entry));
        } else if (g_strcmp0(input, "𝑥!") == 0) {
            handle_factorial(GTK_ENTRY(entry));
        } else if (g_strcmp0(input, "𝑥ⁿ") == 0) {
            handle_power(GTK_ENTRY(entry));
        } else {
            handle_default(GTK_ENTRY(entry), input);
        }
    }