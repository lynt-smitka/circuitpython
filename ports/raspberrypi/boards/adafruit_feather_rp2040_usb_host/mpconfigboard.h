#define MICROPY_HW_BOARD_NAME "Adafruit Feather RP2040 USB Host"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO21)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO3)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO2)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO14)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO15)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO8)

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)

// #define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
// #define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX

#define CIRCUITPY_USB_HOST_INSTANCE 1
