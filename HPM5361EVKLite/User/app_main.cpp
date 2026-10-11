#include "app_main.h"
#include "board.h"
#include "hpm_timebase.hpp"
#include "libxr.hpp"
#include "hpm_i2c.hpp"
#include "hpm_gpio_drv.h"
#include "hpm_uart_drv.h"

extern "C" void app_main(void)
{
  static LibXR::HPMTimebase timebase;
  UNUSED(timebase);
  static LibXR::HPMI2C i2c3(BOARD_APP_I2C_BASE, BOARD_APP_I2C_CLK_NAME, {100000U});
  UNUSED(i2c3);

  // printf() and the byte report use the SDK console, which CMakeLists.txt
  // places on UART3.
  UART_Type* const console = reinterpret_cast<UART_Type*>(BOARD_CONSOLE_UART_BASE);
  constexpr uint32_t poll_ms = 20U;
  uint32_t elapsed_ms = 0U;
  uint32_t heartbeat_ms = 0U;
  uint32_t heartbeat_count = 0U;
  uint8_t last_button_state = gpio_read_pin(
      BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

  printf("HPM5361EVKLite GPIO demo\r\n");
  printf("UART3 PB15 TX / PB14 RX, 115200 8N1\r\n");
  printf("LED: PA10 active low, button: PA03 active high\r\n");
  while (true)
  {
    const uint8_t button_state = gpio_read_pin(
        BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    uint8_t byte;
    while (uart_try_receive_byte(console, &byte) == status_success)
    {
      const char printable = byte >= 0x20U && byte <= 0x7EU ? char(byte) : '.';
      printf("rx 0x%02x '%c'\r\n", static_cast<unsigned>(byte), printable);
    }
    if (button_state != last_button_state)
    {
      printf("button: %s\r\n",
             button_state == BOARD_BUTTON_PRESSED_VALUE ? "pressed" : "released");
      last_button_state = button_state;
    }
    if (elapsed_ms >= 500U)
    {
      board_led_toggle();
      elapsed_ms = 0U;
    }
    if (heartbeat_ms >= 1000U)
    {
      printf("heartbeat %lu, button=%u\r\n", static_cast<unsigned long>(heartbeat_count++),
             static_cast<unsigned>(button_state));
      heartbeat_ms = 0U;
    }
    LibXR::Thread::Sleep(poll_ms);
    elapsed_ms += poll_ms;
    heartbeat_ms += poll_ms;
  }
}
