#include "app_main.h"
#include "board.h"
#include "hpm_timebase.hpp"
#include "libxr.hpp"
#include "hpm_uart.hpp"
#include "hpm_i2c.hpp"
#include "hpm_spi.hpp"
#include "hpm_mcan.hpp"
#include "app_framework.hpp"

/* User Code Begin 1 */
#include "hpm_gpio_drv.h"
#include "stdio.hpp"
/* User Code End 1 */

ATTR_PLACE_AT_NONCACHEABLE static uint8_t uart3_rx_dma_buffer[256];
ATTR_PLACE_AT_NONCACHEABLE static uint8_t uart3_tx_dma_buffer[512];
static uint8_t spi1_rx_buffer[256];
static uint8_t spi1_tx_buffer[256];

extern "C" void app_main(void)
{
  /* User Code Begin 2 */
  /* User Code End 2 */
  static LibXR::HPMTimebase timebase;
  UNUSED(timebase);
  static LibXR::HPMUART uart3(
      BOARD_APP_UART_BASE, BOARD_APP_UART_CLK_NAME, BOARD_APP_UART_IRQ,
      LibXR::RawData(uart3_rx_dma_buffer, sizeof(uart3_rx_dma_buffer)),
      LibXR::RawData(uart3_tx_dma_buffer, sizeof(uart3_tx_dma_buffer)),
      1, 0, 5,
      {115200U, LibXR::UART::Parity::NO_PARITY, 8U, 1U});
  static LibXR::HPMI2C i2c3(BOARD_APP_I2C_BASE, BOARD_APP_I2C_CLK_NAME, true, {100000U});
  board_init_spi_pins_with_gpio_as_cs(BOARD_APP_SPI_BASE);
  static LibXR::HPMSPI spi1(
      BOARD_APP_SPI_BASE, BOARD_APP_SPI_CLK_NAME,
      LibXR::RawData(spi1_rx_buffer, sizeof(spi1_rx_buffer)),
      LibXR::RawData(spi1_tx_buffer, sizeof(spi1_tx_buffer)), false,
      {LibXR::SPI::ClockPolarity::LOW, LibXR::SPI::ClockPhase::EDGE_1, LibXR::SPI::Prescaler::DIV_1, false},
      +[](bool selected) { board_write_spi_cs(BOARD_SPI_CS_PIN, selected ? BOARD_SPI_CS_ACTIVE_LEVEL : !BOARD_SPI_CS_ACTIVE_LEVEL); });
  board_init_can(HPM_MCAN0);
  board_init_can_clock(HPM_MCAN0);
  static LibXR::HPMCAN mcan0(HPM_MCAN0, clock_can0, 0, IRQn_MCAN0, true, 8);
  LibXR::CAN::Configuration mcan0_config{};
  mcan0_config.bitrate = 1000000U;
  mcan0_config.sample_point = 0.75f;
  ASSERT(mcan0.SetConfig(mcan0_config) == LibXR::ErrorCode::OK);
  board_init_can(HPM_MCAN2);
  board_init_can_clock(HPM_MCAN2);
  static LibXR::HPMCANFD mcan2(HPM_MCAN2, clock_can2, 2, IRQn_MCAN2, true, 8);
  LibXR::FDCAN::Configuration mcan2_config{};
  mcan2_config.bitrate = 1000000U;
  mcan2_config.sample_point = 0.75f;
  mcan2_config.data_bitrate = 2500000U;
  mcan2_config.data_sample_point = 0.75f;
  mcan2_config.fd_mode.fd_enabled = true;
  mcan2_config.fd_mode.brs = true;
  mcan2_config.fd_mode.esi = false;
  ASSERT(mcan2.SetConfig(mcan2_config) == LibXR::ErrorCode::OK);
  static LibXR::HardwareContainer peripherals(
      LibXR::Entry<LibXR::UART>{uart3, {"uart3"}},
      LibXR::Entry<LibXR::I2C>{i2c3, {"i2c3"}},
      LibXR::Entry<LibXR::SPI>{spi1, {"spi1"}},
      LibXR::Entry<LibXR::CAN>{mcan0, {"mcan0"}},
      LibXR::Entry<LibXR::FDCAN>{mcan2, {"mcan2"}});
  /* User Code Begin 3 */
  // UART0 PA00/PA01 belongs to MCAN0 in this configuration. Console traffic
  // shares the declared UART3 owner rather than bypassing its DMA state.
  LibXR::STDIO::read_ = uart3.read_port_;
  LibXR::STDIO::write_ = uart3.write_port_;
  constexpr uint32_t poll_ms = 20U;
  uint32_t elapsed_ms = 0U;
  uint32_t heartbeat_ms = 0U;
  uint32_t heartbeat_count = 0U;
  uint8_t last_button_state = gpio_read_pin(
      BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
  LibXR::ReadOperation read_op;

  LibXR::STDIO::Printf<"HPM5361EVKLite GPIO demo\r\n">();
  LibXR::STDIO::Printf<"UART3 PB15 TX / PB14 RX, 115200 8N1\r\n">();
  LibXR::STDIO::Printf<"LED: PA10 active low, button: PA03 active high\r\n">();
  while (true)
  {
    const uint8_t button_state = gpio_read_pin(
        BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    while (uart3.read_port_->Size() != 0U)
    {
      uint8_t byte;
      const auto result = uart3.Read({&byte, sizeof(byte)}, read_op);
      if (result != LibXR::ErrorCode::OK) { break; }
      const char printable = byte >= 0x20U && byte <= 0x7EU ? char(byte) : '.';
      LibXR::STDIO::Printf<"rx 0x%02x '%c'\r\n">(static_cast<unsigned>(byte), printable);
    }
    if (button_state != last_button_state)
    {
      LibXR::STDIO::Printf<"button: %s\r\n">(
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
      LibXR::STDIO::Printf<"heartbeat %lu, button=%u\r\n">(
          static_cast<unsigned long>(heartbeat_count++), static_cast<unsigned>(button_state));
      heartbeat_ms = 0U;
    }
    LibXR::Thread::Sleep(poll_ms);
    elapsed_ms += poll_ms;
    heartbeat_ms += poll_ms;
  }
  /* User Code End 3 */
}
