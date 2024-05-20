#brilliantadam
#20/5/2024
#Simple ERD tool to detect and respond to suspisious processes on a Windows System
import psutil
import time

# Function to check for suspicious processes
def check_processes():
    while True:
        for proc in psutil.process_iter():
            try:
                # Check for suspicious process name
                if 'suspicious_name' in proc.name().lower():
                    print(f'Suspicious process detected: {proc.name()}')
                    # Terminate the process
                    proc.kill()
                    # Log the event
                    log_event(f'Suspicious process {proc.name()} terminated')
                # Check for high CPU usage
                if proc.cpu_percent(interval=1) > 90:
                print(f'Process {proc.name()} using high CPU resources')
                    # Log the event
                    log_event(f'Process {proc.name()} using high CPU resources')
            except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
                pass

# Function to log events
def log_event(message):
    with open('edr.log', 'a') as f:
        f.write(f'{time.ctime()}: {message}\n')

# Continuously check for suspicious processes
while True:
    check_processes()
    time.sleep(1)