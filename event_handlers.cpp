#include "event_handlers.h"

    std::string EventHandlers::expression;

    void EventHandlers::handle_input(GtkWidget *entry, const char *input) {
        const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (expression != current_text) {
            expression = current_text;
        }
        if (g_strrstr(current_text, "Error:") != nullptr) {
            no_empty_state = true;
        }
        if (g_strcmp0(input, "C") == 0) {
            no_empty_state = false;
            result_shown = false;
            expression.clear();
            gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
        } else if (g_strcmp0(input, "=") == 0) {
            expression.clear();
            if (current_text[0] != '\0') {
                if (no_empty_state) {
                    return;
                }
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
        } else if (g_strcmp0(input, "⌫") == 0) {
            if (no_empty_state) {
                expression.clear();
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                no_empty_state = false;
            } else if (!expression.empty()) {
                icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(expression);
                ustr.truncate(ustr.length() - 1);
                std::string newExpression;
                ustr.toUTF8String(newExpression);
                expression = newExpression;
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
            }
            result_shown = false;
        } else if (g_strcmp0(input, "𝑥!") == 0) {
            if (no_empty_state) {
                expression = "!";
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                no_empty_state = false;
            } else {
                expression+="!";
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                result_shown = false;
            }
        } else if (g_strcmp0(input, "𝑥ⁿ") == 0) {
            if (no_empty_state) {
                expression = "^";
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                no_empty_state = false;
            } else {
                expression+="^";
                gtk_entry_set_text(GTK_ENTRY(entry), expression.c_str());
                result_shown = false;
            }
        } else {
            if (result_shown){
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
    }