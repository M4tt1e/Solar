import sys

from serial import Serial



if __name__ == "__main__":
    log_file = sys.argv[1]
    with open(log_file, "a") as log:
        with Serial("COM3", baudrate=28800) as ser:
            while True:
                data = ser.readline().decode()
                print(f"Received: {data}")
                log.write(data)
                log.write("\n")
                log.flush()
