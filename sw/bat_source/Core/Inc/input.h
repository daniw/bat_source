/*
 * input.h
 *
 * Abstracts the encoder/buttons so statemachine.c can be driven either from
 * real hardware (TIM4 quadrature encoder + GPIO buttons) or from a CLI
 * keyboard-control session (see cli.c), without caring which.
 */

#ifndef INC_INPUT_H_
#define INC_INPUT_H_

#include "stdint.h"

typedef enum {
	INPUT_SOURCE_HW = 0, INPUT_SOURCE_CLI
} input_source_t;

void input_init(void);
void input_set_source(input_source_t src);
input_source_t input_get_source(void);

/* Same semantics as tim_encoder_read()/tim_encoder_reset(): raw 0-127 count,
 * 2 counts per mechanical detent. */
uint16_t input_encoder_read(void);
void input_encoder_reset(uint8_t value);

/* Same semantics as gpio_readBtnOk/Esc/Out(): level, non-zero while pressed. */
uint8_t input_btn_ok(void);
uint8_t input_btn_esc(void);
uint8_t input_btn_out(void);

/*
 * CLI-simulated backend. Only called from cli.c's keyboard-control-mode byte
 * parser; has no effect while INPUT_SOURCE_HW is active.
 */

/* Moves the simulated encoder by one detent (2 counts): +1 = CW/increment, -1 = CCW/decrement. */
void input_cli_encoder_step(int8_t direction);

/* OK/ESC are momentary keys (Enter/Esc) -> one-shot pulse, consumed by the next input_btn_ok/esc() call. */
void input_cli_pulse_ok(void);
void input_cli_pulse_esc(void);

/* OUT has no key-release event over a terminal, so Space toggles a held level instead of pulsing it. */
void input_cli_toggle_out(void);

#endif /* INC_INPUT_H_ */
