# UART Command Processing System
### STM32 DMA-Based Reliable Command Parser

---

## Overview

This project implements a UART-based command processing system on an STM32 microcontroller designed to operate reliably under real-world conditions such as high data rates and corrupted input.

The system receives a continuous byte stream over UART, reconstructs structured commands using a parser state machine, and executes those commands through an application layer. The architecture is designed to maintain correctness under load by decoupling data reception, parsing, and execution.

---

## Key Design Goals

- Reliable operation under high input rates
- Graceful recovery from corrupted data
- Scalable architecture using DMA and event-driven processing

---

## System Architecture

### Data Flow

```
UART RX → DMA Circular Buffer → Parser → Command Queue → Application Logic
```

### Components

**UART + DMA Layer**
- Uses DMA in circular mode to continuously transfer incoming bytes to memory
- Eliminates per-byte interrupt overhead
- Uses hardware counter (NDTR) to determine write position

**Parser (State Machine)**
- Processes byte stream into structured packets
- Handles: start byte detection, length validation, checksum verification
- Error recovery via reset to known state

**Command Queue**
- Single-producer, single-consumer (SPSC) queue
- Decouples parsing from execution
- Prevents blocking in parsing path

**Application Layer**
- Executes parsed commands (e.g., LED control)
- Provides UART debug output

### Error Handling

- DMA overwrite detection
- Parser reset on invalid data
- Command queue overflow detection

---

## Testing Methodology

Testing focused on validating functional correctness, behavior under stress, and error detection and recovery.

Testing was performed using serial terminal input (manual and burst), debugger variable inspection, and instrumentation counters (e.g., processed bytes, overwrite count).

### Test Cases

#### Test 1 — Basic Command Processing

| | |
|---|---|
| **Procedure** | Send a valid command packet over UART, observe system response |
| **Expected** | Command parsed correctly; corresponding action executed (e.g., LED toggles); debug message printed |
| **Result** | ✅ Command processed successfully; correct output observed |

#### Test 2 — Back-to-Back Commands

| | |
|---|---|
| **Procedure** | Send multiple valid packets in quick succession |
| **Expected** | All commands processed in order; no data loss; queue handles burst input |
| **Result** | ✅ Commands processed correctly; no missed or reordered commands |

#### Test 3 — Corrupted Data Handling

| | |
|---|---|
| **Procedure** | Send invalid packets (bad checksum, invalid length, random bytes) |
| **Expected** | Parser detects invalid data; resets to `WAIT_START` state; system recovers on next valid packet |
| **Result** | ✅ Corrupted packets ignored; parser resynchronized; system remained stable |

#### Test 4 — High Input Rate (DMA Stress)

| | |
|---|---|
| **Procedure** | Send large burst of data; observe DMA buffer behavior |
| **Expected** | DMA continues receiving without CPU intervention; parser processes in chunks; system remains responsive |
| **Result** | ✅ DMA buffer filled and drained correctly; no crash or lockup |

#### Test 5 — DMA Overwrite Detection

| | |
|---|---|
| **Procedure** | Introduce artificial delay in main loop; send large burst to exceed processing capacity |
| **Expected** | System detects overwrite condition; `dma_overwrite_flag` triggered; parser reset; system continues |
| **Result** | ✅ `dma_overwrite_count` incremented; recovery path executed; system remained operational |

#### Test 6 — IDLE Line Detection

| | |
|---|---|
| **Procedure** | Send burst input followed by pause |
| **Expected** | UART IDLE interrupt triggers; system processes accumulated data once per burst |
| **Result** | ✅ IDLE interrupt observed; processing triggered correctly; reduced unnecessary polling |

---

## Observability & Debugging

### Instrumentation Variables

| Variable | Purpose |
|---|---|
| `dma_bytes_processed` | Verify data flow through the pipeline |
| `last_write_idx` | Track DMA write position |
| `uart_dma_read_idx` | Track parser read position |
| `last_unread` | Detect backlog buildup |
| `dma_overwrite_count` | Count overwrite events |
| `command_overflow_flag` | Flag command queue saturation |

### Enabled Capabilities

- Verification of data flow
- Detection of backlog buildup
- Validation of recovery behavior

---

## Key Findings

- DMA significantly reduces CPU load compared to interrupt-per-byte RX
- Circular buffer systems require explicit overwrite detection strategies
- Parser-based recovery is essential for handling corrupted streams
- IDLE detection improves efficiency for bursty traffic
- Separating ISR signaling from processing improves system determinism

---

## Limitations & Future Improvements

### Current Limitations

- Overwrite detection is heuristic-based and not fully precise
- ISR relies on flag signaling rather than a full event system
- No formal unit tests or automated validation
- No timing profiling or CPU utilization measurement

### Future Work

- Add precise overwrite detection using wrap tracking
- Introduce RTOS or scheduler for scalability
- Extend system to support additional protocols (I2C, SPI)
- Integrate with UI system (touchscreen project)
