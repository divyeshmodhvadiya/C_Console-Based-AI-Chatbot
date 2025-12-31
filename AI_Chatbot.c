#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_RESPONSE_LENGTH 500
#define MAX_INPUT_LENGTH 200
#define MAX_CONVERSATION_HISTORY 50
#define MAX_KEYWORDS 20

// Structure to store conversation history
struct Conversation {
    char userInput[MAX_INPUT_LENGTH];
    char botResponse[MAX_RESPONSE_LENGTH];
    time_t timestamp;
};

// Structure for keyword-response mapping
struct KeywordResponse {
    char keywords[MAX_KEYWORDS][50];
    int keywordCount;
    char responses[5][MAX_RESPONSE_LENGTH];
    int responseCount;
};

// Global variables
struct Conversation history[MAX_CONVERSATION_HISTORY];
int historyCount = 0;
int conversationActive = 1;

// Function prototypes
void displayWelcome();
void displayMenu();
void startConversation();
void processUserInput(char* input);
char* generateResponse(char* input);
char* toLowerCase(char* str);
int containsKeyword(char* input, char* keyword);
void saveConversationToFile();
void displayConversationHistory();
void clearConversationHistory();
void displayTime();
void displayDate();
int performCalculation(char* input);
void clearInputBuffer();
int isValidTopic(char* input);
void displayValidTopics();

int main() {
    int choice;
    srand(time(NULL));
    
    displayWelcome();
    
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                startConversation();
                break;
            case 2:
                displayConversationHistory();
                break;
            case 3:
                clearConversationHistory();
                break;
            case 4:
                saveConversationToFile();
                break;
            case 5:
                printf("\nThank you for chatting! Goodbye!\n");
                printf("-----------------------------------------------------------\n\n");
                break;
            default:
                printf("\nInvalid choice! Please enter a number between 1-5.\n\n");
        }
    } while(choice != 5);
    
    return 0;
}

void displayWelcome() {
    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("             AI CHATBOT - RULE-BASED SYSTEM\n");
    printf("-----------------------------------------------------------\n");
    printf("Welcome! I am your AI assistant.\n");
    printf("I can help you with various topics and conversations.\n");
    printf("\n");
    printf("RULES:\n");
    printf("   - Ask questions related to supported topics only\n");
    printf("   - Type 'help' or 'topics' to see available topics\n");
    printf("   - Type 'quit' or 'exit' to return to main menu\n");
    printf("-----------------------------------------------------------\n\n");
}

void displayMenu() {
    printf("-----------------------------------------------------------\n");
    printf("                       MAIN MENU\n");
    printf("-----------------------------------------------------------\n");
    printf("   1. Start Conversation\n");
    printf("   2. View History\n");
    printf("   3. Delete History\n");
    printf("   4. Export History\n");
    printf("   5. Exit\n");
    printf("-----------------------------------------------------------\n");
}

void startConversation() {
    char input[MAX_INPUT_LENGTH];
    conversationActive = 1;
    
    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("              CONVERSATION MODE ACTIVATED\n");
    printf("-----------------------------------------------------------\n");
    printf("Chatbot: Hello! How can I help you today?\n");
    printf("\n");
    printf("SUPPORTED TOPICS:\n");
    printf("   - Greetings, Goodbyes, Personal Questions\n");
    printf("   - Time and Date\n");
    printf("   - Math Calculations\n");
    printf("   - Programming and Coding\n");
    printf("   - Jokes and Entertainment\n");
    printf("   - Weather (acknowledgment only)\n");
    printf("\n");
    printf("Type 'topics' to see all topics, or 'quit' to exit.\n");
    printf("-----------------------------------------------------------\n\n");
    
    while(conversationActive) {
        printf("You: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if(strlen(input) == 0) {
            printf("Chatbot: Please say something! I am here to chat.\n\n");
            continue;
        }
        
        // Check for exit commands
        char* lowerInput = toLowerCase(strdup(input));
        if(strcmp(lowerInput, "quit") == 0 || strcmp(lowerInput, "exit") == 0) {
            free(lowerInput);
            conversationActive = 0;
            printf("Chatbot: Goodbye! It was nice chatting with you!\n");
            printf("-----------------------------------------------------------\n\n");
            break;
        }
        free(lowerInput);
        
        processUserInput(input);
    }
}

void processUserInput(char* input) {
    static char errorMsg[MAX_RESPONSE_LENGTH];
    char* response;
    
    // Check if input is a valid topic
    if(!isValidTopic(input)) {
        strcpy(errorMsg, "ERROR: I can only answer questions related to supported topics!\n"
                         "   Please ask about: greetings, time/date, calculations, programming, jokes, weather.\n"
                         "   Type 'topics' to see all available topics.");
        response = errorMsg;
    } else {
        response = generateResponse(input);
    }
    
    // Store in history
    if(historyCount < MAX_CONVERSATION_HISTORY) {
        strcpy(history[historyCount].userInput, input);
        strcpy(history[historyCount].botResponse, response);
        history[historyCount].timestamp = time(NULL);
        historyCount++;
    }
    
    printf("Chatbot: %s\n\n", response);
}

int isValidTopic(char* input) {
    char* lowerInput = toLowerCase(strdup(input));
    int valid = 0;
    
    // List of all valid topic keywords
    char* validKeywords[] = {
        // Greetings
        "hello", "hi", "hey", "greetings", "good morning", "good afternoon", "good evening",
        // Goodbye
        "bye", "goodbye", "see you", "farewell", "exit", "quit",
        // Personal
        "how are you", "what is your name", "who are you", "your name", "how old", "your age",
        // Time & Date
        "time", "what time", "date", "what date", "today's date", "current time",
        // Math
        "calculate", "what is", "plus", "minus", "multiply", "divide", "times", "+", "-", "*", "/",
        // Programming
        "programming", "code", "program", "coding", "c language", "programming language",
        // Jokes
        "joke", "funny", "humor", "laugh", "tell me a joke",
        // Weather
        "weather", "temperature", "rain", "sunny", "cloudy",
        // Help
        "help", "what can you do", "topics", "commands",
        // Thanks
        "thank", "thanks", "appreciate",
        // Feelings
        "love", "like you",
        // General questions (limited support)
        "what", "why", "how", "when", "where"
    };
    
    int keywordCount = sizeof(validKeywords) / sizeof(validKeywords[0]);
    int i;
    
    // Check if input contains any valid keyword
    for(i = 0; i < keywordCount; i++) {
        if(containsKeyword(lowerInput, validKeywords[i])) {
            valid = 1;
            break;
        }
    }
    
    // Special case: if it's a math expression (contains numbers and operators)
    if(!valid) {
        int hasNumber = 0, hasOperator = 0;
        int j;
        for(j = 0; lowerInput[j] != '\0'; j++) {
            if(isdigit(lowerInput[j])) hasNumber = 1;
            if(lowerInput[j] == '+' || lowerInput[j] == '-' || 
               lowerInput[j] == '*' || lowerInput[j] == '/' || lowerInput[j] == 'x') {
                hasOperator = 1;
            }
        }
        if(hasNumber && hasOperator) {
            valid = 1;
        }
    }
    
    free(lowerInput);
    return valid;
}

void displayValidTopics() {
    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("              SUPPORTED TOPICS AND KEYWORDS\n");
    printf("-----------------------------------------------------------\n");
    printf("\n");
    printf("1. GREETINGS AND COURTESY:\n");
    printf("   hello, hi, hey, greetings\n");
    printf("   bye, goodbye, see you, farewell\n");
    printf("   thank you, thanks\n\n");
    printf("2. PERSONAL QUESTIONS:\n");
    printf("   how are you, what is your name, who are you\n");
    printf("   how old are you, your age\n\n");
    printf("3. TIME AND DATE:\n");
    printf("   what time, current time, time\n");
    printf("   what date, today's date, date\n\n");
    printf("4. MATHEMATICS:\n");
    printf("   what is 5 + 3, calculate 10 * 2\n");
    printf("   15 - 7, 20 / 4\n");
    printf("   plus, minus, multiply, divide, times\n\n");
    printf("5. PROGRAMMING:\n");
    printf("   programming, code, coding, program\n");
    printf("   C language, programming language\n\n");
    printf("6. ENTERTAINMENT:\n");
    printf("   tell me a joke, joke, funny, humor\n\n");
    printf("7. WEATHER:\n");
    printf("   weather, temperature, rain, sunny\n");
    printf("   (Note: I can acknowledge but do not have real-time data)\n\n");
    printf("8. HELP:\n");
    printf("   help, what can you do, topics, commands\n\n");
    printf("-----------------------------------------------------------\n");
    printf("Questions outside these topics will receive an error message.\n");
    printf("-----------------------------------------------------------\n\n");
}

char* generateResponse(char* input) {
    static char response[MAX_RESPONSE_LENGTH];
    char* lowerInput = toLowerCase(strdup(input));
    
    // Topics command
    if(containsKeyword(lowerInput, "topics") || containsKeyword(lowerInput, "show topics")) {
        displayValidTopics();
        strcpy(response, "Above are all the topics I can help you with!");
        free(lowerInput);
        return response;
    }
    
    // Greetings
    if(containsKeyword(lowerInput, "hello") || containsKeyword(lowerInput, "hi") || 
       containsKeyword(lowerInput, "hey") || containsKeyword(lowerInput, "greetings") ||
       containsKeyword(lowerInput, "good morning") || containsKeyword(lowerInput, "good afternoon") ||
       containsKeyword(lowerInput, "good evening")) {
        char* greetings[] = {
            "Hello! Nice to meet you! How can I assist you today?",
            "Hi there! Great to see you! What would you like to talk about?",
            "Hey! Welcome! I am here to help. What is on your mind?",
            "Greetings! I am excited to chat with you. How can I help?"
        };
        strcpy(response, greetings[rand() % 4]);
        free(lowerInput);
        return response;
    }
    
    // Goodbye
    if(containsKeyword(lowerInput, "bye") || containsKeyword(lowerInput, "goodbye") || 
       containsKeyword(lowerInput, "see you") || containsKeyword(lowerInput, "farewell")) {
        char* goodbyes[] = {
            "Goodbye! It was nice talking to you. Take care!",
            "See you later! Have a wonderful day!",
            "Farewell! Feel free to come back anytime!",
            "Bye! Hope to chat with you again soon!"
        };
        strcpy(response, goodbyes[rand() % 4]);
        free(lowerInput);
        return response;
    }
    
    // How are you
    if(containsKeyword(lowerInput, "how are you") || containsKeyword(lowerInput, "how do you feel")) {
        char* responses[] = {
            "I am doing great, thank you for asking! How about you?",
            "I am functioning perfectly! Thanks for checking. How are you doing?",
            "I am excellent! Always ready to help. What about you?",
            "I am doing well! I enjoy chatting with you. How are you feeling?"
        };
        strcpy(response, responses[rand() % 4]);
        free(lowerInput);
        return response;
    }
    
    // Name
    if(containsKeyword(lowerInput, "what is your name") || containsKeyword(lowerInput, "who are you") || 
       containsKeyword(lowerInput, "your name")) {
        strcpy(response, "I am an AI Chatbot, a rule-based intelligent assistant! You can call me ChatBot. What is your name?");
        free(lowerInput);
        return response;
    }
    
    // Time
    if(containsKeyword(lowerInput, "time") || containsKeyword(lowerInput, "what time")) {
        displayTime();
        strcpy(response, "I have displayed the current time above!");
        free(lowerInput);
        return response;
    }
    
    // Date
    if(containsKeyword(lowerInput, "date") || containsKeyword(lowerInput, "what date") || 
       containsKeyword(lowerInput, "today's date")) {
        displayDate();
        strcpy(response, "I have displayed today's date above!");
        free(lowerInput);
        return response;
    }
    
    // Weather
    if(containsKeyword(lowerInput, "weather") || containsKeyword(lowerInput, "temperature") || 
       containsKeyword(lowerInput, "rain") || containsKeyword(lowerInput, "sunny")) {
        char* weatherResponses[] = {
            "I do not have access to real-time weather data, but I hope you have a great day!",
            "I am not connected to weather services, but I wish you pleasant weather!",
            "I cannot check the weather, but I hope it is nice where you are!",
            "Weather data is not available to me, but stay safe and enjoy your day!"
        };
        strcpy(response, weatherResponses[rand() % 4]);
        free(lowerInput);
        return response;
    }
    
    // Math calculations
    if(containsKeyword(lowerInput, "calculate") || containsKeyword(lowerInput, "what is") || 
       containsKeyword(lowerInput, "plus") || containsKeyword(lowerInput, "minus") || 
       containsKeyword(lowerInput, "multiply") || containsKeyword(lowerInput, "divide") || 
       containsKeyword(lowerInput, "times") || containsKeyword(lowerInput, "add") ||
       containsKeyword(lowerInput, "subtract") || containsKeyword(lowerInput, "+") || 
       containsKeyword(lowerInput, "-") || containsKeyword(lowerInput, "*") || 
       containsKeyword(lowerInput, "/") || containsKeyword(lowerInput, "x")) {
        int result = performCalculation(input);
        if(result != -999999) {
            sprintf(response, "The result is: %d", result);
        } else {
            strcpy(response, "I can perform simple calculations! Please use formats like:\n"
                            "   '5 + 3' or 'what is 5 plus 3'\n"
                            "   '10 * 2' or 'calculate 10 times 2'\n"
                            "   '15 - 7' or '20 / 4'");
        }
        free(lowerInput);
        return response;
    }
    
    // Help
    if(containsKeyword(lowerInput, "help") || containsKeyword(lowerInput, "what can you do") ||
       containsKeyword(lowerInput, "commands")) {
        strcpy(response, "I can help you with:\n"
                         "   Greetings and Personal Questions\n"
                         "   Time and Date Information\n"
                         "   Math Calculations (+, -, *, /)\n"
                         "   Programming and Coding Topics\n"
                         "   Jokes and Entertainment\n"
                         "   Weather (acknowledgment)\n"
                         "Type 'topics' to see all supported keywords!");
        free(lowerInput);
        return response;
    }
    
    // Jokes
    if(containsKeyword(lowerInput, "joke") || containsKeyword(lowerInput, "funny") || 
       containsKeyword(lowerInput, "humor") || containsKeyword(lowerInput, "laugh")) {
        char* jokes[] = {
            "Why do not scientists trust atoms? Because they make up everything!",
            "Why did the scarecrow win an award? He was outstanding in his field!",
            "What do you call a fake noodle? An impasta!",
            "Why do not eggs tell jokes? They would crack each other up!",
            "What is the best thing about Switzerland? I do not know, but the flag is a big plus!"
        };
        strcpy(response, jokes[rand() % 5]);
        free(lowerInput);
        return response;
    }
    
    // Thank you
    if(containsKeyword(lowerInput, "thank") || containsKeyword(lowerInput, "thanks")) {
        char* thanks[] = {
            "You are welcome! Happy to help!",
            "My pleasure! Anytime!",
            "No problem! Glad I could assist!",
            "You are very welcome! Feel free to ask more!"
        };
        strcpy(response, thanks[rand() % 4]);
        free(lowerInput);
        return response;
    }
    
    // Age
    if(containsKeyword(lowerInput, "how old") || containsKeyword(lowerInput, "your age")) {
        strcpy(response, "I am an AI, so I do not have an age in the traditional sense! I exist in the digital realm.");
        free(lowerInput);
        return response;
    }
    
    // Programming
    if(containsKeyword(lowerInput, "programming") || containsKeyword(lowerInput, "code") || 
       containsKeyword(lowerInput, "program") || containsKeyword(lowerInput, "coding")) {
        char* programming[] = {
            "Programming is fascinating! I am built using C programming language.",
            "I love talking about programming! I am a rule-based chatbot written in C.",
            "Coding is awesome! I was created using C language with pattern matching.",
            "Programming is my world! I am a chatbot built with C programming."
        };
        strcpy(response, programming[rand() % 4]);
        free(lowerInput);
        return response;
    }
    
    // Love/Feelings
    if(containsKeyword(lowerInput, "love") || containsKeyword(lowerInput, "like you")) {
        strcpy(response, "That is very kind of you! I appreciate the sentiment. I am here to help and chat!");
        free(lowerInput);
        return response;
    }
    
    // General questions (only if they match valid topics)
    if(containsKeyword(lowerInput, "?") || containsKeyword(lowerInput, "what") || 
       containsKeyword(lowerInput, "why") || containsKeyword(lowerInput, "how") || 
       containsKeyword(lowerInput, "when") || containsKeyword(lowerInput, "where")) {
        // Check if it is a question about a valid topic
        if(containsKeyword(lowerInput, "time") || containsKeyword(lowerInput, "date") ||
           containsKeyword(lowerInput, "name") || containsKeyword(lowerInput, "age") ||
           containsKeyword(lowerInput, "programming") || containsKeyword(lowerInput, "code") ||
           containsKeyword(lowerInput, "weather") || containsKeyword(lowerInput, "calculate")) {
            // This will be handled by specific topic handlers above
            // Continue to default
        } else {
            // Generic question response
            char* questionResponses[] = {
                "I am a rule-based chatbot, so I can answer questions about specific topics I know.",
                "I can help with greetings, time/date, calculations, programming, jokes, and more!",
                "I am designed to answer questions about supported topics. Type 'topics' to see what I can help with.",
                "I can assist with various topics! Try asking about time, date, calculations, or programming."
            };
            strcpy(response, questionResponses[rand() % 4]);
            free(lowerInput);
            return response;
        }
    }
    
    // Default responses (only for valid topics that did not match above)
    char* defaultResponses[] = {
        "I understand. Could you be more specific? I can help with time, date, calculations, programming, jokes, and more!",
        "I am here to help! Try asking about: time, date, calculations, programming, or tell me a joke!",
        "I can assist you with various topics. Type 'topics' to see all available options.",
        "Feel free to ask me about supported topics! I can help with time, date, math, programming, and entertainment."
    };
    
    strcpy(response, defaultResponses[rand() % 4]);
    free(lowerInput);
    return response;
}

char* toLowerCase(char* str) {
    int i = 0;
    while(str[i]) {
        str[i] = tolower(str[i]);
        i++;
    }
    return str;
}

int containsKeyword(char* input, char* keyword) {
    return (strstr(input, keyword) != NULL);
}

void displayTime() {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeinfo);
    printf("Current Time: %s\n", timeStr);
}

void displayDate() {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char dateStr[100];
    strftime(dateStr, sizeof(dateStr), "%A, %B %d, %Y", timeinfo);
    printf("Today's Date: %s\n", dateStr);
}

int performCalculation(char* input) {
    int num1 = 0, num2 = 0, result = 0;
    char op = '\0';
    char* lowerInput = toLowerCase(strdup(input));
    
    // Try to extract numbers and operator (format: "5 + 3" or "5+3")
    if(sscanf(input, "%d %c %d", &num1, &op, &num2) == 3 || 
       sscanf(input, "%d%c%d", &num1, &op, &num2) == 3) {
        switch(op) {
            case '+':
                result = num1 + num2;
                free(lowerInput);
                return result;
            case '-':
                result = num1 - num2;
                free(lowerInput);
                return result;
            case '*':
            case 'x':
            case 'X':
                result = num1 * num2;
                free(lowerInput);
                return result;
            case '/':
                if(num2 != 0) {
                    result = num1 / num2;
                    free(lowerInput);
                    return result;
                } else {
                    free(lowerInput);
                    return -999999; // Division by zero
                }
            default:
                break;
        }
    }
    
    // Try formats like "what is 5 plus 3" or "calculate 10 times 2"
    if(sscanf(lowerInput, "what is %d", &num1) >= 1 || 
       sscanf(lowerInput, "calculate %d", &num1) >= 1 ||
       sscanf(lowerInput, "%d", &num1) == 1) {
        
        // Addition
        if(containsKeyword(lowerInput, "plus") || containsKeyword(lowerInput, "+") || 
           containsKeyword(lowerInput, "add")) {
            if(sscanf(lowerInput, "%*s %*s %d %*s %d", &num1, &num2) == 2 ||
               sscanf(lowerInput, "%d %*s %d", &num1, &num2) == 2) {
                result = num1 + num2;
                free(lowerInput);
                return result;
            }
        }
        
        // Subtraction
        if(containsKeyword(lowerInput, "minus") || containsKeyword(lowerInput, "-") ||
           containsKeyword(lowerInput, "subtract")) {
            if(sscanf(lowerInput, "%d %*s %d", &num1, &num2) == 2) {
                result = num1 - num2;
                free(lowerInput);
                return result;
            }
        }
        
        // Multiplication
        if(containsKeyword(lowerInput, "multiply") || containsKeyword(lowerInput, "*") || 
           containsKeyword(lowerInput, "times") || containsKeyword(lowerInput, "x")) {
            if(sscanf(lowerInput, "%d %*s %d", &num1, &num2) == 2) {
                result = num1 * num2;
                free(lowerInput);
                return result;
            }
        }
        
        // Division
        if(containsKeyword(lowerInput, "divide") || containsKeyword(lowerInput, "/") ||
           containsKeyword(lowerInput, "divided by")) {
            if(sscanf(lowerInput, "%d %*s %d", &num1, &num2) == 2 && num2 != 0) {
                result = num1 / num2;
                free(lowerInput);
                return result;
            }
        }
    }
    
    free(lowerInput);
    return -999999; // Could not parse
}

void displayConversationHistory() {
    int i;
    
    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("                 CONVERSATION HISTORY\n");
    printf("-----------------------------------------------------------\n");
    
    if(historyCount == 0) {
        printf("No conversation history available.\n");
        printf("Start a conversation to see history here.\n");
    } else {
        for(i = 0; i < historyCount; i++) {
            printf("\n[Exchange %d]\n", i + 1);
            printf("You: %s\n", history[i].userInput);
            printf("Chatbot: %s\n", history[i].botResponse);
            
            char timeStr[100];
            struct tm* timeinfo = localtime(&history[i].timestamp);
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
            printf("Time: %s\n", timeStr);
            printf("-----------------------------------------------------------\n");
        }
    }
    
    printf("-----------------------------------------------------------\n\n");
}

void clearConversationHistory() {
    historyCount = 0;
    printf("\nConversation history has been cleared.\n\n");
}

void saveConversationToFile() {
    FILE* file = fopen("chatbot_conversation.txt", "w");
    int i;
    
    if(file == NULL) {
        printf("\nError: Could not create file to save conversation.\n\n");
        return;
    }
    
    fprintf(file, "-----------------------------------------------------------\n");
    fprintf(file, "              AI CHATBOT CONVERSATION LOG\n");
    fprintf(file, "-----------------------------------------------------------\n\n");
    
    if(historyCount == 0) {
        fprintf(file, "No conversation history to save.\n");
    } else {
        for(i = 0; i < historyCount; i++) {
            fprintf(file, "[Exchange %d]\n", i + 1);
            fprintf(file, "You: %s\n", history[i].userInput);
            fprintf(file, "Chatbot: %s\n", history[i].botResponse);
            
            char timeStr[100];
            struct tm* timeinfo = localtime(&history[i].timestamp);
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
            fprintf(file, "Time: %s\n", timeStr);
            fprintf(file, "-----------------------------------------------------------\n\n");
        }
    }
    
    fprintf(file, "-----------------------------------------------------------\n");
    fprintf(file, "End of Conversation Log\n");
    
    fclose(file);
    printf("\nConversation saved to 'chatbot_conversation.txt' successfully.\n\n");
}

void clearInputBuffer() {
    int c;
    while((c = getchar()) != '\n' && c != EOF) {
        // Empty body - discards characters
    }
}
