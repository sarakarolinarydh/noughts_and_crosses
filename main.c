#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "neopixel.h"

#define DEBOUNCE_TIME 25
#define TURN_LED 9
#define START_SCORE_BLUE 10
#define START_SCORE_RED 13
#define CHESS_PIECE_COUNT 9
#define LED_COUNT 10

enum Chess_pieces
{
    BLUE,
    RED,
    FREE,
};

rgb_color colors[LED_COUNT];

int score[] = {0, 0};

void init_ports_mcu();
void reset_chess_board(unsigned char player);
void reset_score();
void update_chess_board(unsigned char player, int pos);
unsigned char game(unsigned char player);
unsigned char button_pressed(int button);
unsigned char chess_move_ok(int pos);
unsigned char three_in_row(int player);
void update_score(unsigned char player);
void little_victory_dance(unsigned char winner);
void big_victory_dance(unsigned char winner);

int winning_row[3];
int board[] = {FREE, FREE, FREE, FREE, FREE, FREE, FREE, FREE, FREE};

int queue[2][3] = {{0, 0, 0}, {0, 0, 0}}; // Queue to keep track of the chess pieces
int queue_size[] = {0, 0};

void init_ports_mcu()
{
    DDRB = 0b101;   // PB0: Output to LED strip. PB3: Input reset, PB1: Button 8
    PORTB = 0b1010; // Internal pull-ups at input reset and button 8
    DDRA = 0b0; // Buttons 0: PA0, 1: PA1, 2: PA2, 3: PA3, 4: PA4, 5: PA5, 6: PA6, 7: PA7
    PORTA = 0b11111111; // Internal pull-ups for the buttons
}

int main(void)
{
    init_ports_mcu();

    // Blue chess pieces start
    unsigned char current_player = 0;
    unsigned char start_player_game = 0;
    unsigned char start_player_round = 1;

    // Play forever
    while (1)
    {
        start_player_round = 1 - start_player_round;
        start_player_game = start_player_round;
        current_player = start_player_game;
        reset_score();

        // Play round
        for (int i = 0; i < 3; i++)
        {
            // Play game
            reset_chess_board(current_player);
            current_player = game(current_player);
            if (i < 2)
            {
                little_victory_dance(current_player);
            }
            start_player_game = 1 - start_player_game;
            current_player = start_player_game;
        }

        big_victory_dance(((score[BLUE] > score[RED]) ? BLUE : RED));
    }
    return 0;
}

unsigned char game(unsigned char player)
{
    unsigned char current_player = player;
    while (1)
    {
        for (int i = 0; i < CHESS_PIECE_COUNT; i++)
        {
            if (button_pressed(i))
            {
                if (chess_move_ok(i) == 1)
                {
                    update_chess_board(current_player, i);
                    if (three_in_row(current_player))
                    {
                        update_score(current_player);
                        return current_player;
                    }
                    current_player = 1 - current_player;
                }
            }
        }
    }
    return 0;
}

unsigned char chess_move_ok(int pos)
{
    return (board[pos] == FREE ? 1 : 0);
}

void reset_chess_board(unsigned char player)
{
    queue_size[BLUE] = 0;
    queue_size[RED] = 0;

    int r = 80;
    int b = 80;

    for (int i = 0; i < CHESS_PIECE_COUNT; i++)
    {
        board[i] = FREE;
    }
    if (player == BLUE)
    {
        colors[TURN_LED] = (rgb_color){0, 0, b};
    }
    else
    {
        colors[TURN_LED] = (rgb_color){r, 0, 0};
    }
    led_strip_write(colors, LED_COUNT);
}

void update_chess_board(unsigned char player, int pos)
{
    int r = 80;
    int b = 80;
    int r_dim_light = 10;
    int b_dim_light = 10;

    if (queue_size[player] == 3)
    {
        // Mark the board position as free
        board[queue[player][0]] = FREE;
        // Turn off the LED for the oldest chess move
        colors[queue[player][0]] = (rgb_color){0, 0, 0};
        // Update the queue
        queue[player][0] = queue[player][1];
        queue[player][1] = queue[player][2];
        queue[player][2] = pos;
    }
    else
    {
        // Add the latest chess move to the queue
        queue[player][queue_size[player]] = pos;
        queue_size[player]++;
    }

    // Update the board position taken map
    board[pos] = player;

    // If three pieces, let the oldest piece shine less
    if (player == 0 && queue_size[player] == 3)
    {
        colors[queue[0][0]] = (rgb_color){0, 0, b_dim_light};
    }
    else if (player == 1 && queue_size[player] == 3)
    {
        colors[queue[1][0]] = (rgb_color){r_dim_light, 0, 0};
    }

    // Turn on the LED fore the piece placed
    if (player == 0)
    {
        colors[pos] = (rgb_color){0, 0, b};
    }
    else
    {
        colors[pos] = (rgb_color){r, 0, 0};
    }
    led_strip_write(colors, LED_COUNT);
    _delay_ms(250);

    // Update the turn to play LED
    if (player == BLUE)
    {
        colors[TURN_LED] = (rgb_color){r, 0, 0};
    }
    else
    {
        colors[TURN_LED] = (rgb_color){0, 0, b};
    }
    led_strip_write(colors, LED_COUNT);
}

void reset_score()
{
    score[BLUE] = 0;
    score[RED] = 0;

    // Score blue
    for (int i = 0; i < 3; i++)
    {
        colors[START_SCORE_BLUE + i] = (rgb_color){0, 0, 0};
    }

    // Score red
    for (int i = 0; i < 3; i++)
    {
        colors[START_SCORE_RED + i] = (rgb_color){0, 0, 0};
    }
}

void update_score(unsigned char player)
{
    score[player]++;

    // Un comment below if score LED is connected
    int r = 80;
    int b = 80;

    // Score blue
    for (int i = 0; i < score[BLUE]; i++)
    {
        colors[START_SCORE_BLUE + i] = (rgb_color){0, 0, b};
    }

    // Score red
    for (int i = 0; i < score[RED]; i++)
    {
        colors[START_SCORE_RED + i] = (rgb_color){r, 0, 0};
    }

    led_strip_write(colors, LED_COUNT);
    _delay_ms(250);
}

unsigned char three_in_row(int player)
{
    // Horizontally
    for (int i = 0; i < 7; i = i + 3)
    {
        if (board[i] == player && board[i + 1] == player && board[i + 2] == player)
        {
            winning_row[0] = i;
            winning_row[1] = i + 1;
            winning_row[2] = i + 2;
            return 1;
        }
    }
    // Vertically
    for (int i = 0; i < 3; i++)
    {
        if (board[i] == player && board[i + 3] == player && board[i + 6] == player)
        {
            winning_row[0] = i;
            winning_row[1] = i + 3;
            winning_row[2] = i + 6;
            return 1;
        }
    }
    // Diagonally
    if (board[0] == player && board[4] == player && board[8] == player)
    {
        winning_row[0] = 0;
        winning_row[1] = 4;
        winning_row[2] = 8;
        return 1;
    }
    if (board[2] == player && board[4] == player && board[6] == player)
    {
        winning_row[0] = 2;
        winning_row[1] = 4;
        winning_row[2] = 6;
        return 1;
    }
    return 0;
}

void little_victory_dance(unsigned char winner)
{
    // Victory colours
    int r = 0;
    int b = 0;

    b = (winner == BLUE) ? 20 : 0;
    r = (winner == RED) ? 20 : 0;

    for (int j = 0; j < 4; j++)
    {
        for (uint16_t i = 0; i < CHESS_PIECE_COUNT; i++)
        {
            colors[i] = (rgb_color){0, 0, 0};
        }
        led_strip_write(colors, LED_COUNT);

        // The winner row
        colors[queue[winner][0]] = (rgb_color){r, 0, b};
        colors[queue[winner][1]] = (rgb_color){r, 0, b};
        colors[queue[winner][2]] = (rgb_color){r, 0, b};
        led_strip_write(colors, LED_COUNT);
        _delay_ms(250);

        for (uint16_t i = 0; i < CHESS_PIECE_COUNT; i++)
        {
            colors[i] = (rgb_color){0, 0, 0};
        }
        led_strip_write(colors, LED_COUNT);
        _delay_ms(250);
    }

    for (uint16_t i = 0; i < CHESS_PIECE_COUNT; i++)
    {
        colors[i] = (rgb_color){0, 0, 0};
    }
    led_strip_write(colors, LED_COUNT);
}

void big_victory_dance(unsigned char winner)
{
    int r = 0;
    int b = 80;

    for (int m = 0; m < 3; m++)
    {
        for (int n = 0; n < 5; n++)
        {
            for (int i = 0; i < CHESS_PIECE_COUNT; i++)
            {
                colors[i] = (rgb_color){r, 0, b};
                r = (r == 80) ? 0 : 80;
                b = (b == 80) ? 0 : 80;
            }
            led_strip_write(colors, LED_COUNT);
            _delay_ms(125);
        }

        b = (winner == BLUE) ? 80 : 0;
        r = (winner == RED) ? 80 : 0;
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < CHESS_PIECE_COUNT; i++)
            {
                colors[i] = (rgb_color){r, 0, b};
            }
            led_strip_write(colors, LED_COUNT);
            _delay_ms(250);
            for (int i = 0; i < CHESS_PIECE_COUNT; i++)
            {
                colors[i] = (rgb_color){0, 0, 0};
            }
            led_strip_write(colors, LED_COUNT);
            _delay_ms(250);
        }
        _delay_ms(125);
    }

    for (int i = 0; i < CHESS_PIECE_COUNT; i++)
    {
        colors[i] = (rgb_color){0, 0, 0};
    }
    led_strip_write(colors, LED_COUNT);
}

// Check if the button is pressed
unsigned char button_pressed(int button)
{
    static char old_button_state[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char button_state[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char read = 0;

    int pin_x = (button == 8) ? PINB : PINA;
    int mask = (button == 8) ? 0b10 : 1 << button;

    read = !(pin_x & mask);
    _delay_ms(DEBOUNCE_TIME);

    if ((!(pin_x & mask)) == read)
    {
        button_state[button] = (!(pin_x & mask));
        if (button_state[button] != old_button_state[button])
        {
            old_button_state[button] = button_state[button];
            if ((!(pin_x & mask))) // Button has changed from 0 to one, and not the other way around
            {
                return 1;
            }
        }
    }
    return 0;
}