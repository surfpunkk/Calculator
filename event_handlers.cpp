#include "event_handlers.h"

    void EventHandlers::handle_input(GtkWidget *entry, const char *input) {
    static std::string expression;
    if (g_strcmp0(input, "C") == 0) {
        expression.clear();
        gtk_entry_set_text(GTK_ENTRY(entry), "");
    } else if (g_strcmp0(input, "=") == 0){
        const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (current_text[0] != '\0') {
            try {
                std::string result = CalculatorCore::calculate(current_text);
                double num_result = std::stod(result);
                gchar result_str[64];
                g_snprintf(result_str, sizeof(result_str), "%.15g", num_result);
                std::string formatted_result = result_str;
                size_t dot_pos = formatted_result.find('.');
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
                no_empty_state = false;

            } catch (const std::exception &e) {
                gtk_entry_set_text(GTK_ENTRY(entry), "Error");
            }
        } else {
            gtk_entry_set_text(GTK_ENTRY(entry), "No Empty");
            no_empty_state = true;
        }
    } else {
        expression += input;
        gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
    }
}
