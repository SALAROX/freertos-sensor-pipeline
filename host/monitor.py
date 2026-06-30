#!/usr/bin/env python3
"""
monitor.py — reads the sensor pipeline output from the STM32F4 board and
displays a live table with timing jitter analysis.

The firmware sends CSV lines every 500 ms:
    tick_ms,filtered,min,max,sample_count

This script measures actual host-side arrival interval and reports deviation
from the expected 500 ms, which is a practical way to validate that the
reporter task is meeting its timing under real scheduling load.

Usage:
    python monitor.py --port /dev/ttyUSB0
    python monitor.py --port COM3 --log run1.csv
"""

import argparse
import csv
import sys
import time
from collections import deque
from dataclasses import dataclass, fields
from typing import Optional

import serial


@dataclass
class PipelineFrame:
    tick_ms:      int
    filtered:     int
    min_val:      int
    max_val:      int
    sample_count: int
    host_ts:      float   # wall-clock time of arrival, not sent by firmware


def parse_line(line: str, host_ts: float) -> Optional[PipelineFrame]:
    parts = line.strip().split(",")
    if len(parts) != 5:
        return None
    try:
        return PipelineFrame(
            tick_ms=int(parts[0]),
            filtered=int(parts[1]),
            min_val=int(parts[2]),
            max_val=int(parts[3]),
            sample_count=int(parts[4]),
            host_ts=host_ts,
        )
    except ValueError:
        return None


def run(port: str, baud: int, log_path: Optional[str]) -> None:
    log_file = None
    writer = None

    if log_path:
        log_file = open(log_path, "w", newline="")
        writer = csv.writer(log_file)
        writer.writerow(["host_ts", "tick_ms", "filtered", "min", "max",
                         "count", "interval_ms", "jitter_ms"])

    try:
        ser = serial.Serial(port=port, baudrate=baud, timeout=2.0)
    except serial.SerialException as e:
        sys.exit(f"Could not open {port}: {e}")

    print(f"Connected to {port} at {baud} baud. Ctrl-C to stop.\n")
    print(f"{'tick_ms':>10}  {'filtered':>8}  {'min':>6}  {'max':>6}  "
          f"{'count':>8}  {'interval':>10}  {'jitter':>8}")
    print("-" * 72)

    # Keep a rolling window to compute average interval
    recent_intervals: deque = deque(maxlen=20)
    prev_host_ts: Optional[float] = None

    try:
        while True:
            try:
                raw = ser.readline().decode("ascii", errors="replace")
            except serial.SerialException as e:
                print(f"\nSerial error: {e}")
                break

            if not raw.strip():
                continue

            host_ts = time.monotonic()
            frame = parse_line(raw, host_ts)

            if frame is None:
                # Non-CSV line — could be a debug print; show it raw
                print(f"  [raw] {raw.strip()}")
                continue

            interval_ms = None
            jitter_ms = None

            if prev_host_ts is not None:
                interval_ms = (host_ts - prev_host_ts) * 1000.0
                recent_intervals.append(interval_ms)
                avg = sum(recent_intervals) / len(recent_intervals)
                jitter_ms = interval_ms - avg

            prev_host_ts = host_ts

            interval_str = f"{interval_ms:8.1f} ms" if interval_ms is not None else "        --"
            jitter_str   = f"{jitter_ms:+6.1f} ms" if jitter_ms is not None else "      --"

            print(f"{frame.tick_ms:>10}  {frame.filtered:>8}  {frame.min_val:>6}  "
                  f"{frame.max_val:>6}  {frame.sample_count:>8}  "
                  f"{interval_str}  {jitter_str}")

            if writer:
                writer.writerow([
                    f"{host_ts:.3f}",
                    frame.tick_ms, frame.filtered, frame.min_val,
                    frame.max_val, frame.sample_count,
                    f"{interval_ms:.2f}" if interval_ms is not None else "",
                    f"{jitter_ms:.2f}" if jitter_ms is not None else "",
                ])
                log_file.flush()

    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        ser.close()
        if log_file:
            log_file.close()
            print(f"Log saved to {log_path}")


def main() -> None:
    ap = argparse.ArgumentParser(description="STM32F4 sensor pipeline monitor")
    ap.add_argument("--port", default="/dev/ttyUSB0",
                    help="Serial port (default: /dev/ttyUSB0)")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--log", metavar="FILE",
                    help="Optional CSV log output file")
    args = ap.parse_args()

    run(args.port, args.baud, args.log)


if __name__ == "__main__":
    main()
