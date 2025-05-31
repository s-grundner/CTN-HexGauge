#include <encoder.h>

#include <stdio.h>
#include <inttypes.h>

typedef char *other_handle_t;

static void sw_cb(void *args)
{
	// external type available through rot arg pointer
	other_handle_t type = (other_handle_t)args;
	printf("%s (from callback)\n", type);
}

static void rot_cb(void *arg)
{
	// external type available through rot arg pointer
	encoder_handle_t ec = (encoder_handle_t)arg; // ec handle is default argument
	printf("my new position is %ld\n", encoder_read(ec));
}

#define PIN_A GPIO_NUM_25
#define PIN_B GPIO_NUM_26
#define PIN_SW GPIO_NUM_19

int app_main()
{

	other_handle_t type = "hello world"; // can contain

	encoder_handle_t ec;
	encoder_config_t ec_cfg = {
			.rot_callback = rot_cb,
			.sw_callback = sw_cb,
			.rot_args = 0,	 // 0 for default args
			.sw_args = type, // void*: can be anything
			.pin_a = PIN_A,
			.pin_b = PIN_B,
			.pin_sw = PIN_SW,
			.sw_max = 40,
			.sw_debounce_time_ms = 100,
	};

	encoder_init(&ec, &ec_cfg);

	while (true)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		printf("switch was pressed %ld times | state: %d\n", encoder_sw_counter(ec), encoder_sw_pressed(ec));
	}
}