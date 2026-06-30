# stm32f4-freertos-sensor-pipeline

FreeRTOS task pipeline on an STM32F411, demonstrating multi-task architecture
with queue-based inter-task communication, mutex-protected shared state, and
interrupt-driven UART output — paired with a Python host monitor that reads
and timestamps the live data stream.

Built this as a companion to my [firmware validation harness](https://github.com/YOUR_USERNAME/embedded-firmware-validation-harness)
project. That one showed bare-metal peripheral drivers; this one sits on top
of an RTOS and focuses on task architecture and timing behaviour.

---

## What it does

Three tasks, one queue, one mutex:

```
[Sensor Task] --queue--> [Processing Task] --mutex--> shared_state
                                                           |
                                              [Reporter Task] --> UART --> host
```

- **Sensor task** (highest priority): fires every 50 ms, reads a simulated
  sensor value (stub — swap in a real I2C read if you have hardware),
  timestamps the reading with the FreeRTOS tick counter, and posts to a queue.

- **Processing task**: blocks on the queue, applies a 16-point moving average
  to filter the raw values, writes the result into a shared state struct under
  a mutex. Also tracks the min/max seen since boot.

- **Reporter task** (lowest priority): every 500 ms takes the mutex, formats
  the current state as a compact CSV line, and sends it over UART2 at 115200.
  The host monitor reads this and timestamps arrival to check actual delivery
  jitter.

A FreeRTOS software timer flashes the onboard LED at 1 Hz as a basic
liveness indicator — if the LED stops blinking, something has blocked
indefinitely.

---

## Hardware

STM32F411 Nucleo or Discovery. UART2 on PA2 (TX) / PA3 (RX) — same pins
as my other project, so the same USB-serial adapter works.

No external sensors required to run it. The sensor stub just generates a
sawtooth waveform so there's something meaningful to filter. If you want
to swap in a real I2C sensor, `sensor_task.c` has a clear substitution
point with a note.

---

## Building

You need the FreeRTOS kernel source. Download from
[freertos.org](https://www.freertos.org/a00104.html) (or `git clone
https://github.com/FreeRTOS/FreeRTOS-Kernel`) and point the Makefile at it:

```sh
cd firmware
make FREERTOS_DIR=/path/to/FreeRTOS-Kernel
```

Or set it permanently at the top of the Makefile. Requires `arm-none-eabi-gcc`
and `st-flash` for flashing. The build uses the `GCC/ARM_CM3` FreeRTOS port
rather than `ARM_CM4F` — no float code here so no reason to pay the FPU
context-save overhead on every context switch. Runs correctly on the CM4 core.

```sh
make flash FREERTOS_DIR=...
```

---

## Running the host monitor

```sh
cd host
pip install -r requirements.txt
python monitor.py --port /dev/ttyUSB0
```

Prints a live table of filtered value, min, max, and measured UART delivery
interval. Use `--log output.csv` to capture for analysis.

---

## A note on interrupt priorities

The most common FreeRTOS Cortex-M gotcha: any ISR that calls a FreeRTOS API
must have a priority numerically >= `configMAX_SYSCALL_INTERRUPT_PRIORITY`
(i.e. lower urgency). On Cortex-M4 the hardware uses an inverted priority
scheme (lower number = higher priority), so this is easy to get backwards.
This project only uses FreeRTOS software timers and the UART TX ISR —
both configured to priority 6, well within the safe range. See
`FreeRTOSConfig.h` for the values and `firmware/src/uart.c` for the ISR
priority setup.

---

## License

MIT
