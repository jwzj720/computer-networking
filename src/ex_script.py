import subprocess
import time
import csv
import os

# Configuration
EXE_PATH = './test_program'
PIGPIO_INIT = '0'               # Replace with actual pigpio initialization value if needed
BAUD_RATES = [10, 50, 100, 125, 150, 175, 200, 225, 250]  # BAUD
MESSAGE_SIZE = 100               # Number of messages to send per rate

# Output CSVs
SPEED_TEST_FILE = 'speed_tests.csv'
TIMER_TEST_FILE = 'rtt_log.csv'
ERROR_TEST_FILE = 'error_tests.csv'

# Ensure output directory exists
# os.makedirs(os.path.dirname(SPEED_TEST_FILE), exist_ok=True)
# os.makedirs(os.path.dirname(TIMER_TEST_FILE), exist_ok=True)
# os.makedirs(os.path.dirname(ERROR_TEST_FILE), exist_ok=True)

# Initialize CSV files
with open(SPEED_TEST_FILE, mode='w', newline='') as csv_file:
    fieldnames = ['send_rate', 'message_size', 'start_time', 'end_time', 'duration']
    writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    writer.writeheader()

with open(TIMER_TEST_FILE, mode='w', newline='') as csv_file:
    fieldnames = ['time_to_send']
    writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    writer.writeheader()

with open(ERROR_TEST_FILE, mode='w', newline='') as csv_file:
    fieldnames = ['send_rate', 'message_size', 'errors_detected']
    writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
    writer.writeheader()

# Speed Tests
for rate in BAUD_RATES:
    print(f"Running speed test with send rate: {rate} msg/s")
    start_time = time.time()

    process = subprocess.Popen([EXE_PATH, PIGPIO_INIT, str(rate), str(MESSAGE_SIZE)],
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)

    stdout, stderr = process.communicate()
    end_time = time.time()
    duration = end_time - start_time

    if process.returncode != 0:
        print(f"Error: {stderr.decode()}")
        continue

    # Log speed test
    with open(SPEED_TEST_FILE, mode='a', newline='') as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=['send_rate', 'message_size', 'start_time', 'end_time', 'duration'])
        writer.writerow({
            'send_rate': rate,
            'message_size': MESSAGE_SIZE,
            'start_time': start_time,
            'end_time': end_time,
            'duration': duration
        })

    print(f"Completed speed test in {duration:.2f} seconds\n")

    # Parse errors
    stdout_decoded = stdout.decode()
    errors = stdout_decoded.count("Error detected!")

    # Log error test
    with open(ERROR_TEST_FILE, mode='a', newline='') as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=['send_rate', 'message_size', 'errors_detected'])
        writer.writerow({
            'send_rate': rate,
            'message_size': MESSAGE_SIZE,
            'errors_detected': errors
        })

    print(f"Detected {errors} errors\n")
