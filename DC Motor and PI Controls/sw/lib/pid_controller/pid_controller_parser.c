/**
 * @file pid_controller_parser.c
 * @author Jared McArthur
 * @brief Serial input parser for the PID controller
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>
#include <stdint.h>

#include "inc/UART.h"

#include "lib/pid_controller/pid_controller_parser.h"

#define MAX_CMD_SIZE    20      // max size for a parser command

// uart buffer to store characters
static char uart_buffer[MAX_CMD_SIZE + 2] = {0};
static uint32_t uart_buffer_index = 0;
static char cmd_buffer[MAX_CMD_SIZE + 2] = {0};
static char param_buffer[MAX_CMD_SIZE + 2] = {0};

bool str_equals(char* a, char* b) {
    while (*a != '\0' && *b != '\0') {
        if (*a != *b) {
            return false;
        }
        ++a;
        ++b;
    }
    if (*a != *b) {
        return false;
    }
    return true;
}

uint32_t str_to_uint(char* str) {
    uint32_t n = 0;
    uint32_t count = 0;
    while(*str != 0) {
        ++count;
        ++str;
    }
    --str;
    for(uint32_t i = 0, j = 1; i < count; ++i, --str) {
        if (*str >= '0' && *str <= '9') {
            n += (*str - '0') * j;
            j *= 10;
        }
    }
    return n;
}

void pid_controller_parser_start(pid_controller_t* pid_controller) {
    // initialize UART on PA1-0
    UART_Init();

    // output the usage of the parser
    UART_OutString(
        "Motor Controller Parser:\n\r"
        "  usage: [help, h] cmd [param]\n\r"
        "  cmd: which coefficient to change\n\r"
        "    kpn - set the kpn\n\r"
        "    kpd - set the kpd\n\r"
        "    kin - set the kin\n\r"
        "    kid - set the kid\n\r"
        "    kdn - set the kdn\n\r"
        "    kdd - set the kdd\n\r"
        "    get - output current coefficients\n\r"
        "  param: unsigned integer to set the coefficient to\n\r"
    );

    // spin and wait for UART inputs
    char character;
    while (1) {
        UART_OutString("Enter command...\n\r");
        do {
            // get a character from the input
            character = UART_InChar();
            // delete an input character
            if (character == DEL && uart_buffer_index) {
                --uart_buffer_index;
                UART_OutChar(character);
            }
            // add a character to buffer
            else if (uart_buffer_index < MAX_CMD_SIZE) {
                uart_buffer[uart_buffer_index++] = character;
                UART_OutChar(character);
            }
        } while (character != CR);
        // null terminate the command
        uart_buffer[--uart_buffer_index] = '\0';
        uart_buffer_index = 0;
        UART_OutString("\n\r");

        // parse the command
        uint32_t i = 0;
        uint32_t cmd_index = 0;
        uint32_t param_index = 0;
        // overlook leading whitespace
        // 0x09, 0x0A, 0x0C, 0x0D, 0x20
        while (uart_buffer[i] == 0x09 || uart_buffer[i] == 0x0A || uart_buffer[i] == 0x0C || uart_buffer[i] == 0x0D || uart_buffer[i] == 0x20 || uart_buffer[i] == '\0') {
            ++i;
        }
        // fill cmd
        while (uart_buffer[i] != 0x09 && uart_buffer[i] != 0x0A && uart_buffer[i] != 0x0C && uart_buffer[i] != 0x0D && uart_buffer[i] != 0x20 && uart_buffer[i] != '\0') {
            cmd_buffer[cmd_index++] = uart_buffer[i++];
        }
        // null terminate cmd
        cmd_buffer[cmd_index] = '\0';
        // overlook whitespace between cmd and param
        while (uart_buffer[i] == 0x09 || uart_buffer[i] == 0x0A || uart_buffer[i] == 0x0C || uart_buffer[i] == 0x0D || uart_buffer[i] == 0x20 || uart_buffer[i] == '\0') {
            ++i;
        }
        // fill param
        while (uart_buffer[i] != 0x09 && uart_buffer[i] != 0x0A && uart_buffer[i] != 0x0C && uart_buffer[i] != 0x0D && uart_buffer[i] != 0x20 && uart_buffer[i] != '\0') {
            param_buffer[param_index++] = uart_buffer[i++];
        }
        // null terminate param
        param_buffer[param_index] = '\0';

        // turn param into an integer
        uint32_t param = str_to_uint(param_buffer);
        
        // execute cmd accordingly
        // set kpn
        if (str_equals(cmd_buffer, "kpn")) {
            pid_controller->kpn = param;
            UART_OutString("kpn set to: ");
            UART_OutUDec(param);
            UART_OutString("\n\r");
        }
        // set kpd
        else if (str_equals(cmd_buffer, "kpd")) {
            pid_controller->kpd = param;
            UART_OutString("kpd set to: ");
            UART_OutUDec(param);
            UART_OutString("\n\r");
        }
        // set kin
        else if (str_equals(cmd_buffer, "kin")) {
            pid_controller->kin = param;
            UART_OutString("kin set to: ");
            UART_OutUDec(param);
            UART_OutString("\n\r");
        }
        // set kid
        else if (str_equals(cmd_buffer, "kid")) {
            pid_controller->kid = param;
            UART_OutString("kid set to: ");
            UART_OutUDec(param);
            UART_OutString("\n\r");
        }
        // set kdn
        else if (str_equals(cmd_buffer, "kdn")) {
            pid_controller->kdn = param;
            UART_OutString("kdn set to: ");
            UART_OutUDec(param);
            UART_OutString("\n\r");
        }
        // set kdd
        else if (str_equals(cmd_buffer, "kdd")) {
            pid_controller->kdd = param;
            UART_OutString("kdd set to: ");
            UART_OutUDec(param);
            UART_OutString("\n\r");
        }
        // output coefficients
        // Current PID Controller Coefficients:
        //   kpn - [kpn]
        //   kpd - [kpd]
        //   kin - [kin]
        //   kid - [kid]
        //   kdn - [kdn]
        //   kdd - [kdd]
        else if (str_equals(cmd_buffer, "get")) {
            UART_OutString("Current PID Controller Coefficients:\n\r");
            UART_OutString("  kpn - ");
            UART_OutUDec(pid_controller->kpn);
            UART_OutString("\n\r");
            UART_OutString("  kpd - ");
            UART_OutUDec(pid_controller->kpd);
            UART_OutString("\n\r");
            UART_OutString("  kin - ");
            UART_OutUDec(pid_controller->kin);
            UART_OutString("\n\r");
            UART_OutString("  kid - ");
            UART_OutUDec(pid_controller->kid);
            UART_OutString("\n\r");
            UART_OutString("  kdn - ");
            UART_OutUDec(pid_controller->kdn);
            UART_OutString("\n\r");
            UART_OutString("  kdd - ");
            UART_OutUDec(pid_controller->kdd);
            UART_OutString("\n\r");
        }
        // print help statement
        // Motor Controller Parser Help:
        //   usage: [help, h] cmd param
        //   cmd: which coefficient to change
        //     kpn - set the kpn
        //     kpd - set the kpd
        //     kin - set the kin
        //     kid - set the kid
        //     kdn - set the kdn
        //     kdd - set the kdd
        //     get - output current coefficients
        //   param: unsigned integer to set the coefficient to
        else if (str_equals(cmd_buffer, "help") || str_equals(cmd_buffer, "h")) {
            UART_OutString(
                "Motor Controller Parser Help:\n\r"
                "  usage: [help, h] cmd param\n\r"
                "  cmd: which coefficient to change\n\r"
                "    kpn - set the kpn\n\r"
                "    kpd - set the kpd\n\r"
                "    kin - set the kin\n\r"
                "    kid - set the kid\n\r"
                "    kdn - set the kdn\n\r"
                "    kdd - set the kdd\n\r"
                "    get - output current coefficients\n\r"
                "  param: unsigned integer to set the coefficient to\n\r"
            );
        }
        // invalid command
        else {
            UART_OutString("Invalid command...\n\r");
        }
    }
}