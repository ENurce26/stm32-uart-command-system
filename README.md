#System Overview

This project implements a UART-based command processing system on an STM32 microcontroller designed to operate reliably under real-world conditions such as high data rates and corrupted input.

The system receives a continuous byte stream over UART, reconstructs structured commands using a parser state machine, and executes those commands through an application layer. The architecture is designed to maintain correctness under load by decoupling data reception, parsing, and execution.

Key design goals:

Reliable operation under high input rates
Graceful recovery from corrupted data
Scalable architecture using DMA and event-driven processing

#System Architecture
Data Flow
UART RX → DMA Circular Buffer → Parser → Command Queue → Application Logic
Components

UART + DMA Layer

Uses DMA in circular mode to continuously transfer incoming bytes to memory
Eliminates per-byte interrupt overhead
Uses hardware counter (NDTR) to determine write position

Parser (State Machine)

Processes byte stream into structured packets
Handles:
Start byte detection
Length validation
Checksum verification
Error recovery via reset to known state

Command Queue

Single-producer, single-consumer (SPSC) queue
Decouples parsing from execution
Prevents blocking in parsing path

Application Layer

Executes parsed commands (e.g., LED control)
Provides UART debug output

Error Handling

DMA overwrite detection
Parser reset on invalid data
Command queue overflow detection


#Testing Methodology

Testing focused on validating:

Functional correctness
Behavior under stress
Error detection and recovery

Testing was performed using:

Serial terminal input (manual and burst)
Debugger variable inspection
Instrumentation counters (e.g., processed bytes, overwrite count)

#Test Cases
Test 1 — Basic Command Processing

Procedure:

Send a valid command packet over UART
Observe system response

Expected Result:

Command is parsed correctly
Corresponding action executed (e.g., LED toggles)
Debug message printed

Actual Result:

Command processed successfully
Correct output observed
Test 2 — Back-to-Back Commands

Procedure:

Send multiple valid packets in quick succession

Expected Result:

All commands processed in order
No data loss
Queue handles burst input

Actual Result:

Commands processed correctly
No missed or reordered commands
Test 3 — Corrupted Data Handling

Procedure:

Send invalid packets (bad checksum, invalid length, random bytes)

Expected Result:

Parser detects invalid data
Parser resets to WAIT_START state
System recovers and processes next valid packet

Actual Result:

Corrupted packets ignored
Parser successfully resynchronized
System remained stable
Test 4 — High Input Rate (DMA Stress)

Procedure:

Send large burst of data (paste large string)
Observe DMA buffer behavior

Expected Result:

DMA continues receiving without CPU intervention
Parser processes data in chunks
System remains responsive

Actual Result:

DMA buffer filled and drained correctly
No system crash or lockup
Test 5 — DMA Overwrite Detection

Procedure:

Introduce artificial delay in main loop
Send large burst to exceed processing capacity

Expected Result:

System detects overwrite condition
dma_overwrite_flag triggered
Parser reset
System continues operation

Actual Result:

dma_overwrite_count incremented
Recovery path executed
System remained operational
Test 6 — IDLE Line Detection

Procedure:

Send burst input followed by pause

Expected Result:

UART IDLE interrupt triggers
System processes accumulated data once per burst

Actual Result:

IDLE interrupt observed
Processing triggered correctly
Reduced unnecessary polling


#Observability & Debugging

Instrumentation used:

dma_bytes_processed
last_write_idx
uart_dma_read_idx
last_unread
dma_overwrite_count
command_overflow_flag

These enabled:

Verification of data flow
Detection of backlog buildup
Validation of recovery behavior


#Key Findings
DMA significantly reduces CPU load compared to interrupt-per-byte RX
Circular buffer systems require explicit overwrite detection strategies
Parser-based recovery is essential for handling corrupted streams
IDLE detection improves efficiency for bursty traffic
Separating ISR signaling from processing improves system determinism


#Limitations & Future Improvements
Current overwrite detection is heuristic-based and not fully precise
ISR still relies on flag signaling rather than full event system
No formal unit tests or automated validation
No timing profiling or CPU utilization measurement

Future work:

Add precise overwrite detection using wrap tracking
Introduce RTOS or scheduler for scalability
Extend system to support additional protocols (I2C, SPI)
Integrate with UI system (touchscreen project)
