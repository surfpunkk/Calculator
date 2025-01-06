#include <gtk/gtk.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdint>
#include <stack>


bool no_empty_state = false;
bool result_shown = false;

void clear_entry(GtkEntry *entry) {
    gtk_entry_set_text(entry, ""); 
}

void on_clear_button_clicked(GtkWidget *button, GtkEntry *entry){
    clear_entry(entry);
}

void on_backspace_button_clicked (GtkWidget *button, GtkEntry *entry){
    gchar *text = g_strdup(gtk_entry_get_text(entry));
    int len = strlen(text);
    if (len>0){
        text[len-1] = '\0';
        gtk_entry_set_text(entry, text);
    }
    if(no_empty_state){
        clear_entry(entry);
        no_empty_state = false;
    }
}

void on_digit_clicked(GtkWidget *button, GtkEntry *entry){
    const gchar *button_text = gtk_button_get_label(GTK_BUTTON(button));
    const gchar *current_text = gtk_entry_get_text(entry);
    gchar *text = g_strconcat(current_text, button_text, NULL);
    if (result_shown || no_empty_state) {
        clear_entry(entry);
        result_shown = false;
        no_empty_state = false;
        gtk_entry_set_text(entry, button_text);
        } else{
        gchar *text = g_strconcat(current_text, button_text, NULL);
        gtk_entry_set_text(entry, text);  
        g_free(text);
    }
}

void on_operator_clicked(GtkWidget *button, GtkEntry *entry){
    const gchar *button_text = gtk_button_get_label(GTK_BUTTON(button));
    const gchar *current_text = gtk_entry_get_text(entry);
    gchar *text = g_strconcat(current_text, button_text, NULL);
    gtk_entry_set_text(entry, text);
    result_shown = false;
    if (no_empty_state){
        clear_entry(entry);
        no_empty_state = false;
        gtk_entry_set_text(entry, button_text);
    }
}

int getPrecedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '^':           return 3;
        default:            return 0;
    }
}

bool isOperator(wchar_t c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%' || c == L'√';
}

std::string applyOperator(double a, double b, const std::string& op) {
    if (op == "+") return std::to_string(a + b);
    if (op == "-") return std::to_string(a - b);
    if (op == "*") return std::to_string(a * b);
    if (op == "/") {
        if (b == 0) {
            no_empty_state = true;        
            return "Error: Division by 0"; 
        } 
        return std::to_string(a / b);
    }
    if (op == "^") return std::to_string(std::pow(a, b));
    if (op == "%") {
        return std::to_string(a * (b / 100.0)); 
    }
    if (op == "√") {
        if (b < 0) {
            return "Error: Negative root"; 
        }
        return std::to_string(std::sqrt(b));
    }
    return "Error: No empty"; 
}

std::vector<std::string> tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string currentToken;
    bool expectOperator = false;  
    bool inExponent = false;

    for (size_t i = 0; i < expression.size(); ++i) {
        wchar_t c = static_cast<unsigned char>(expression[i]);

        if (std::isspace(c)) {
            continue;
        }

        if (std::isdigit(c) || (c == ',' && !currentToken.empty() && currentToken.find(',') == std::string::npos)) {
            currentToken += c;
            expectOperator = true;  
        } else if (c == '-') {
            if (!expectOperator || (tokens.empty() && currentToken.empty())) {
                currentToken += c;
            } else {
                if (!currentToken.empty()) {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                tokens.push_back(std::string(1, c));
                expectOperator = false;
            }
        } else if ((c == 'e' || c == 'E') && !currentToken.empty() && !inExponent && std::isdigit(currentToken.back())) {
            currentToken += c;
            inExponent = true;
        } else if ((c == '+' || c == '-') && inExponent && (currentToken.back() == 'e' || currentToken.back() == 'E')) {
            currentToken += c;
            inExponent = false;
        } else if (isOperator(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(std::string(1, c));
            expectOperator = (c != L'√');  
            inExponent = false;
        } else if (c == '(' || c == ')') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(std::string(1, c));
            expectOperator = (c == ')');
        } else {
            no_empty_state = true;
            return {"Error: Incorrect"};
        }
    }

    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

std::vector<std::string> infixToRPN(const std::vector<std::string>& tokens) {
    std::stack<std::string> operators;
    std::vector<std::string> output;

    for (const auto& token : tokens) {
        if (std::isdigit(token[0]) || (token[0] == '-' && token.size() > 1) || token.find(',') != std::string::npos) {
            output.push_back(token);
        } else if (isOperator(token[0])) {
            while (!operators.empty() && getPrecedence(operators.top()[0]) >= getPrecedence(token[0])) {
                output.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        } else if (token == "(") {
            operators.push(token);
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                output.push_back(operators.top());
                operators.pop();
            }
            if (operators.empty() || operators.top() != "(") {
                no_empty_state = true;
                return {"Error: Mismatched parentheses"}; 
            }
            operators.pop();
        } else {
            no_empty_state = true;
            return {"Error: Unknown element"}; 
        }
    } 
    while (!operators.empty()) {
        if (operators.top() == "(") {
            no_empty_state = true;
            return {"Error: Mismatched parentheses"}; 
        }
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

std::string evaluateRPN(const std::vector<std::string>& rpn) {
    std::stack<double> values;
    for (const auto& token : rpn) {
        if (isOperator(token[0]) && token.size() == 1) {
            if (token[0] == L'√') {
                if (values.empty()) {
                    no_empty_state = true;
                    return "Error: Missing operand for √";
                }
                double operand = values.top(); values.pop();
                if (operand < 0) {
                    no_empty_state = true;
                    return "Error: Negative root";
                }
                values.push(std::sqrt(operand));
            } else {
                if (values.size() < 2) {
                    no_empty_state = true;
                    return "Error: Missing operands";
                }
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();

                std::string result = applyOperator(a, b, token);
                if (result.find("Error") != std::string::npos) {
                    return result;
                }

                values.push(std::stod(result));
            }
        } else {
            try {
                values.push(std::stod(token));
            } catch (...) {
                no_empty_state = true;
                return "Error: Invalid number format";
            }
        }
    }
    if (values.size() != 1) {
        no_empty_state = true;
        return "Error: Incorrect expression";
    }

    return std::to_string(values.top());
}

std::string calculate(const std::string& expression) {
    no_empty_state = false;  
    try {
        auto tokens = tokenize(expression);
        if (tokens.empty() || tokens[0] == "Error: Incorrect") {
            return "Error: Invalid input";
        }
        auto rpn = infixToRPN(tokens);
        if (rpn.empty() || rpn[0] == "Error: Incorrect") {
            return "Error: Incorrect";
        }
        return evaluateRPN(rpn);
    } catch (...) {
        no_empty_state = true;
        return "Error: Incorrect";  
    }
}

void on_equal_button_clicked(GtkWidget *button, GtkEntry *entry) {
    const gchar *current_text = gtk_entry_get_text(entry);
    result_shown = true;
    if (strlen(current_text) > 0) {
        std::string result = calculate(current_text);

        if (no_empty_state) {
            gtk_entry_set_text(entry, result.c_str());
        } else {
            gchar result_str[64];
            g_snprintf(result_str, sizeof(result_str), "%.15g", std::stod(result));

            std::string formatted_result = result_str;

            size_t dot_pos = formatted_result.find('.');
            if (dot_pos != std::string::npos) {
                size_t last_non_zero = formatted_result.find_last_not_of('0');

                if (last_non_zero != std::string::npos && last_non_zero > dot_pos) {
                    formatted_result.erase(last_non_zero + 1);
                } else {
                    formatted_result.erase(dot_pos);
                }
            }
            gtk_entry_set_text(entry, formatted_result.c_str());
        }
    } else {
        gtk_entry_set_text(entry, "Error: No empty");
        no_empty_state = true;
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Калькулятор");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE); 
    gtk_window_set_deletable(GTK_WINDOW(window), TRUE);
    gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), FALSE);

    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    GtkWidget *entry = gtk_entry_new(); 
    gtk_widget_set_size_request(entry, 780, 150); 
    gtk_fixed_put(GTK_FIXED(fixed), entry, 10, 10);
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
   
    GtkWidget *button_backspace = gtk_button_new_with_label("Backspace");
    gtk_widget_set_size_request(button_backspace, 340, 100);
    g_signal_connect(button_backspace, "clicked", G_CALLBACK(on_backspace_button_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_backspace, 450, 170); 
 
    GtkWidget *button_pow = gtk_button_new_with_label("^");
    gtk_widget_set_size_request(button_pow, 100, 100);
    g_signal_connect(button_pow, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_pow, 450, 390);
     
    GtkWidget *button_plus = gtk_button_new_with_label("+");
    gtk_widget_set_size_request(button_plus, 100, 100);
    g_signal_connect(button_plus, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_plus, 340, 170);
     
    GtkWidget *button_minus = gtk_button_new_with_label("-");
    gtk_widget_set_size_request(button_minus, 100, 100);
    g_signal_connect(button_minus, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_minus, 340, 280);
    
    GtkWidget *button_pr = gtk_button_new_with_label("*");
    gtk_widget_set_size_request(button_pr, 100, 100);
    g_signal_connect(button_pr, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_pr, 340, 390);

    GtkWidget *button_del = gtk_button_new_with_label("/");
    gtk_widget_set_size_request(button_del, 100, 90);
    g_signal_connect(button_del, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_del, 340, 500);

    GtkWidget *button_result = gtk_button_new_with_label("=");
    gtk_widget_set_size_request(button_result, 230, 100);
    g_signal_connect(button_result, "clicked", G_CALLBACK(on_equal_button_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_result, 560, 280);

    GtkWidget *button_clear = gtk_button_new_with_label("C");
    gtk_widget_set_size_request(button_clear, 100, 100);
    g_signal_connect(button_clear, "clicked", G_CALLBACK(on_clear_button_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_clear, 450, 280);

    GtkWidget *button_point = gtk_button_new_with_label(",");
    gtk_widget_set_size_request(button_point, 100, 90);
    g_signal_connect(button_point, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_point, 450, 500);

    GtkWidget *button_bracket_left = gtk_button_new_with_label("(");
    gtk_widget_set_size_request(button_bracket_left, 110, 100);
    g_signal_connect(button_bracket_left, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_bracket_left, 560, 390);

    GtkWidget *button_bracket_right = gtk_button_new_with_label(")");
    gtk_widget_set_size_request(button_bracket_right, 110, 100);
    g_signal_connect(button_bracket_right, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_bracket_right, 680, 390);

    GtkWidget *button_procent = gtk_button_new_with_label("%");
    gtk_widget_set_size_request(button_procent, 110, 90);
    g_signal_connect(button_procent, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_procent, 560, 500);

    GtkWidget *button_sqrt = gtk_button_new_with_label("√");
    gtk_widget_set_size_request(button_sqrt , 110, 95);
    g_signal_connect(button_sqrt, "clicked", G_CALLBACK(on_operator_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button_sqrt, 680, 500);

    GtkWidget *button1 = gtk_button_new_with_label("1");
    gtk_widget_set_size_request(button1, 100, 100); 
    g_signal_connect(button1, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button1, 10, 390); 

    GtkWidget *button2 = gtk_button_new_with_label("2");
    gtk_widget_set_size_request(button2, 100, 100); 
    g_signal_connect(button2, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button2, 120, 390);

    GtkWidget *button3 = gtk_button_new_with_label("3");
    gtk_widget_set_size_request(button3, 100, 100); 
    g_signal_connect(button3, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button3, 230, 390);

    GtkWidget *button4 = gtk_button_new_with_label("4");
    gtk_widget_set_size_request(button4,  100, 100); 
    g_signal_connect(button4, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button4, 10, 280);

    GtkWidget *button5 = gtk_button_new_with_label("5");
    gtk_widget_set_size_request(button5, 100, 100); 
    g_signal_connect(button5, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button5, 120, 280);

    GtkWidget *button6 = gtk_button_new_with_label("6");
    gtk_widget_set_size_request(button6, 100, 100); 
    g_signal_connect(button6, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button6, 230, 280);

    GtkWidget *button7 = gtk_button_new_with_label("7");
    gtk_widget_set_size_request(button7, 100, 100); 
    g_signal_connect(button7, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button7, 10, 170);

    GtkWidget *button8 = gtk_button_new_with_label("8");
    gtk_widget_set_size_request(button8, 100, 100); 
    g_signal_connect(button8, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button8, 120, 170);

    GtkWidget *button9 = gtk_button_new_with_label("9");
    gtk_widget_set_size_request(button9, 100, 100); 
    g_signal_connect(button9, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button9, 230, 170);

    GtkWidget *button0 = gtk_button_new_with_label("0");
    gtk_widget_set_size_request(button0, 320, 90); 
    g_signal_connect(button0, "clicked", G_CALLBACK(on_digit_clicked), entry);
    gtk_fixed_put(GTK_FIXED(fixed), button0, 10, 500);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
       "button {font-size: 60px; font-family: \"Roboto\";}"
       "entry {font-size: 40px; font-family: \"Roboto\"; padding-top: 0px; padding-bottom: 80px;}"
       "window {background-color: black;}",
    -1, NULL);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkStyleContext *context_entry = gtk_widget_get_style_context(entry);
    gtk_style_context_add_provider(context_entry,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
                          
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.calculator", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

