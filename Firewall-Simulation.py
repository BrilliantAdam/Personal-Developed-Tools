#brilliantadam
#26/5/2024
#Firewall Simulation for testing purposes
####################################################
#This is only for testing some features in Firewalls
####################################################
import socket
import threading

class Firewall:
    def __init__(self):
        self.rules = [
            {"protocol": "TCP", "source_ip": "192.168.1.10", "source_port": 12345, "action": "ALLOW"},
            {"protocol": "TCP", "source_ip": "192.168.1.20", "source_port": 54321, "action": "DENY"}
            # Add more rules as needed
        ]

    def filter_packet(self, packet):
        protocol, source_ip, source_port, destination_ip, destination_port = packet

        for rule in self.rules:
            if (rule["protocol"] == protocol and
                rule["source_ip"] == source_ip and
                rule["source_port"] == source_port):
                return rule["action"]

        return "DENY"  # Default action if no matching rule is found

    def start_firewall(self):
        firewall_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
        firewall_socket.bind(('0.0.0.0', 0))

        print("Firewall is running...")

        while True:
            packet, addr = firewall_socket.recvfrom(65535)
            # Parse the packet
            protocol = "TCP"
            source_ip = addr[0]
            source_port = packet[0:2]  # Assuming the first 2 bytes represent the source port
            destination_ip = socket.inet_ntoa(packet[16:20])  # Extract destination IP from the IP header
            destination_port = int.from_bytes(packet[20:22], byteorder='big')  # Extract destination port from the TCP header

            action = self.filter_packet((protocol, source_ip, source_port, destination_ip, destination_port))
            if action == "ALLOW":
                print("Packet allowed:", (source_ip, source_port), "->", (destination_ip, destination_port))
                # Forward the packet to the destination
            else:
                print("Packet blocked:", (source_ip, source_port), "->", (destination_ip, destination_port))
                # Drop the packet

if __name__ == "__main__":
    firewall = Firewall()
    firewall_thread = threading.Thread(target=firewall.start_firewall)
    firewall_thread.start()
