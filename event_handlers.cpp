#include "event_handlers.h"

    void EventHandlers::handle_input(GtkWidget *entry, const char *input) {
        static std::string expression;
        if (g_strcmp0(input, "C") == 0) {
            expression.clear();
            gtk_entry_set_text(GTK_ENTRY(entry), "");
        } else if (g_strcmp0(input, "=") == 0) {
            expression.clear();
            if (const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry)); current_text[0] != '\0') {
                const std::string result = CalculatorCore::calculate(current_text);
                gtk_entry_set_text(GTK_ENTRY(entry), result.c_str());
                if (no_empty_state) {
                    gtk_entry_set_text(GTK_ENTRY(entry), result.c_str());
                } else {
                    gchar result_str[64];
                    g_snprintf(result_str, sizeof(result_str), "%.15g", std::stod(result));
                    std::string formatted_result = result_str;
                    size_t dot_pos = formatted_result.find(',');
                    if (dot_pos != std::string::npos) {
                        size_t last_non_zero = formatted_result.find_last_not_of('0');
                        if (last_non_zero > dot_pos) {
                            formatted_result.erase(last_non_zero + 1);
                        } else {
                            formatted_result.erase(dot_pos);
                        }
                    }
                    gtk_entry_set_text(GTK_ENTRY(entry), formatted_result.c_str());
                    result_shown = true;
                }
            } else {
                gtk_entry_set_text(GTK_ENTRY(entry), "No Empty");
                no_empty_state = true;
            }
        } else {
            if (result_shown == 1 && (g_strcmp0(input, "1") == 0 || g_strcmp0(input, "2") == 0 ||
                g_strcmp0(input, "3") == 0 || g_strcmp0(input, "4") == 0 || g_strcmp0(input, "5") == 0 ||
                g_strcmp0(input, "6") == 0 || g_strcmp0(input, "7") == 0 || g_strcmp0(input, "8") == 0 ||
                g_strcmp0(input, "9") == 0 || g_strcmp0(input, "0") == 0)) {
                expression.clear();
                result_shown = false;
                expression += input;
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            } else if (no_empty_state) {
                expression.clear();
                expression += input;
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                no_empty_state = false;
            } else {
                expression += input;
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            }
        }
    }


