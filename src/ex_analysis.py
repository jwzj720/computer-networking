import pandas as pd
import matplotlib.pyplot as plt

# Load the speed test results
speed_df = pd.read_csv('speed_tests.csv')

# Plot duration vs send rate
plt.figure(figsize=(10, 6))
plt.plot(speed_df['send_rate'], speed_df['duration'], marker='o')
plt.title('Send Rate vs Duration')
plt.xlabel('Send Rate (msg/s)')
plt.ylabel('Duration (seconds)')
plt.grid(True)
plt.savefig('speed_tests_duration.png')
plt.show()

# Load the error test results
error_df = pd.read_csv('error_tests.csv')

# Plot error rate vs send rate
plt.figure(figsize=(10, 6))
plt.plot(error_df['send_rate'], error_df['errors_detected'], marker='o', color='red')
plt.title('Send Rate vs Errors Detected')
plt.xlabel('Send Rate (msg/s)')
plt.ylabel('Errors Detected')
plt.grid(True)
plt.savefig('error_tests_errors.png')
plt.show()

# Load the network test results
network_df = pd.read_csv('network_tests.csv')

# Plot errors vs send rate for different number of devices
for devices in network_df['num_devices'].unique():
    subset = network_df[network_df['num_devices'] == devices]
    plt.plot(subset['send_rate'], subset['errors_detected'], marker='o', label=f'{devices} Devices')

plt.title('Send Rate vs Errors Detected for Different Number of Devices')
plt.xlabel('Send Rate (msg/s)')
plt.ylabel('Errors Detected')
plt.legend()
plt.grid(True)
plt.savefig('network_tests_errors.png')
plt.show()
