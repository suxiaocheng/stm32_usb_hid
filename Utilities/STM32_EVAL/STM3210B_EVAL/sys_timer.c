#include "sys_timer.h"

volatile uint32_t tick_count = 0;
volatile uint32_t sys_int_times = 0;
RCC_ClocksTypeDef sys_clk_def;

void key_process(void);

uint8_t init_sys_timer(void)
{
	uint8_t ret;
	RCC_GetClocksFreq(&sys_clk_def);
	ret = SysTick_Config(sys_clk_def.SYSCLK_Frequency / SYS_TIMER_FREQ);

	return ret;
}

void SysTick_Handler(void)
{
	sys_int_times++;
	//stm_printf("Tick->%d\n", sys_int_times);
	if (++tick_count >= 200) {
		tick_count = 0;
		//key_process();
	}
}

void busy_delay_ms(uint32_t time_ms)
{
	uint32_t expect_value;
	__disable_irq();
	expect_value = time_ms * 10 + sys_int_times;
	if (expect_value < sys_int_times) {
		__enable_irq();
		//stm_printf("b0->%x--%x\n", expect_value, sys_int_times);
		while (1) {
			if (sys_int_times <= expect_value)
				break;
		}
	} else {
		__enable_irq();
		//stm_printf("b1->%x--%x\n", expect_value, sys_int_times);
	}
	while (sys_int_times <= expect_value) {
		//stm_printf("b3->%x--%x\n", expect_value, sys_int_times);
	}
}

void busy_delay_100us(uint32_t time_us)
{
	uint32_t expect_value;
	__disable_irq();
	expect_value = time_us + sys_int_times;
	if (expect_value < sys_int_times) {
		__enable_irq();
		//stm_printf("b0->%x--%x\n", expect_value, sys_int_times);
		while (1) {
			if (sys_int_times <= expect_value)
				break;
		}
	} else {
		__enable_irq();
		//stm_printf("b1->%x--%x\n", expect_value, sys_int_times);
	}
	while (sys_int_times <= expect_value) {
		//stm_printf("b3->%x--%x\n", expect_value, sys_int_times);
	}
}

uint32_t get_sys_tick(void)
{
	return sys_int_times;
}
