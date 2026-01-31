import serial
import struct
import threading

PORT = "COM3"   # ESP32のCOMポートに変更
BAUD = 115200

# シリアル開く
ser = serial.Serial(PORT, BAUD, timeout=0.1)

# 受信スレッド
def rx_thread():
    while True:
        try:
            if ser.in_waiting:
                line = ser.readline().decode(errors="ignore").strip()
                if line:
                    print(f"\nESP32: {line}")
                    print(">>> ", end="", flush=True)
        except:
            break

t = threading.Thread(target=rx_thread, daemon=True)
t.start()

print("sendId,commandId,arg0,arg1 (argは最大2個)")
print("Ctrl+C で終了")

MAX_ARG = 2  # argは2個まで

while True:
    try:
        cmd = input(">>> ")

        parts = [p.strip() for p in cmd.split(",")]
        if len(parts) < 2:
            print("sendId と commandId は必須です")
            continue

        sendId    = int(parts[0]) & 0xFF
        commandId = int(parts[1]) & 0xFF
        args      = [int(p) for p in parts[2:MAX_ARG+2]]  # 最大2個まで

        # 足りない場合は0で埋める
        while len(args) < MAX_ARG:
            args.append(0)

        # パックして送信
        packet = struct.pack("<BB", sendId, commandId)
        packet += struct.pack("<" + "i"*MAX_ARG, *args)

        ser.write(packet)

    except KeyboardInterrupt:
        print("\n終了")
        break

"""
import serial
import struct

PORT = "COM3"      # Linuxなら "/dev/ttyUSB0" など
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)

print("ESP32 UART CUI Sender")
print("sendId commandId arg0 arg1")
print("例: 1 10 100 200")
print("Ctrl+C で終了\n")

while True:
    try:
        line = input(">>> ")

        parts = line.split()
        if len(parts) != 4:
            print("入力エラー: 4つの数値を入れてください")
            continue

        sendId    = int(parts[0]) & 0xFF
        commandId = int(parts[1]) & 0xFF
        arg0      = int(parts[2])
        arg1      = int(parts[3])

        packet = struct.pack("<BBii",
                              sendId,
                              commandId,
                              arg0,
                              arg1)

        ser.write(packet)

        print(f"送信: {packet.hex()}")

    except KeyboardInterrupt:
        print("\n終了")
        break

"""
