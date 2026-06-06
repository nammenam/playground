#define PS_GPIO_BASE 0xE000A000

#define GPIO_DATA_0  ((volatile uint32_t *)(PS_GPIO_BASE + 0x040))
#define GPIO_DIRM_0  ((volatile uint32_t *)(PS_GPIO_BASE + 0x204))
#define GPIO_OEN_0   ((volatile uint32_t *)(PS_GPIO_BASE + 0x208))

#define LED_PIN 0 // MIO 0
