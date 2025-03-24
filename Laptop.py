import os
import re
import requests
import time

# ESP8266 MAC Address (formatted correctly)
ESP_MAC = "ec-64-c9-d3-bf-d5"

def find_esp_ip(target_mac):
    """
    Continuously scans the ARP table for the ESP8266's IP using its MAC address.
    """
    print("Scanning network for ESP8266...")
    for _ in range(5):  # Retry up to 5 times
        arp_output = os.popen("arp -a").read()
        for line in arp_output.split("\n"):
            if target_mac in line.lower():  # Compare lowercase MAC
                match = re.search(r"\d+\.\d+\.\d+\.\d+", line)
                if match:
                    return match.group()
        print("ESP not found, retrying...")
        time.sleep(2)  # Wait before retrying
    return None

def send_request(waste_type, esp_ip):
    """
    Sends an HTTP request to the ESP8266.
    """
    url = f"http://{esp_ip}/{waste_type}"
    try:
        response = requests.get(url, timeout=3)
        print(f"ESP Response: {response.text}")
    except requests.RequestException as e:
        print(f"Error: {e}")

def main():
    esp_ip = find_esp_ip(ESP_MAC)

    if not esp_ip:
        print("ESP8266 not found on the network. Please check its connection.")
        return

    print(f"ESP8266 found at {esp_ip}")

    while True:
        choice = input("Enter request (b for bio, n for non-bio, /q to quit): ")
        if choice == 'b':
            send_request("bio", esp_ip)
        elif choice == 'n':
            send_request("non-bio", esp_ip)
        elif choice == '/q':
            send_request("q", esp_ip)
            print("Exiting...")
            break
        else:
            print("Invalid input")

if __name__ == "__main__":
    main()
