import matplotlib.pyplot as plt
import pyshark


pcap_df = pyshark.FileCapture('traffic.pcap')

print(pcap_df[0].sniff_timestamp, pcap_df[0].tcp.window_size)

time_axis = []
congestion_window_axis = []

for packet in pcap_df:
    time = float(packet.sniff_timestamp)
    congestion_window = int(packet.tcp.window_size)

    time_axis.append(time)
    congestion_window_axis.append(congestion_window)


plt.plot(time_axis, congestion_window_axis)
plt.title('Congestion Window Over Time')
plt.xlabel('Time')
plt.ylabel('Congestion Window Size')
plt.show()
