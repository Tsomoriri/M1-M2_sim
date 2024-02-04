import pandas as pd
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
# Read the CSV file

data = pd.read_csv('/home/sushen/projects/com_modelling_sim/results/finals/FindHandoverRATE.csv')
current_directory = os.getcwd()
ca_directory = os.path.join(current_directory, 'CA1_plots') # CA1 is the folder name, it will store the plots
os.makedirs(ca_directory, exist_ok=True)

# Plot arrival rate handover vs  loss
plt.figure()
data.plot(x='ahr', y=['lph', 'tlph'], kind='line')
plt.xlabel('Arrival Rate Handover')
plt.ylabel('Loss')
plt.title('Arrival Rate Handover vs Loss')
plt.xlim([0, 1])  # Set x-axis limits
plt.ylim([0, 0.1])  # Set y-axis limits
plt.legend(['Simulated Loss', 'Theoretical  Loss'])
save_path = os.path.join(ca_directory, 'arrival_rate_handover_vs_loss.png')
plt.savefig(save_path)


# Plot arrival rate newcall vs aggregate loss with theoretical aggregate loss
plt.figure()
data.plot(x='anr', y=['lpn','tlpn'], kind='line')
# data.plot(x='anr', y='tlpa', kind='line')
plt.xlabel('Arrival Rate Newcall')
plt.ylabel(' Loss')
plt.title('Arrival Rate Newcall vs Loss')
plt.legend(['Simulated Loss', 'Theoretical Loss'])
plt.xlim([0, 1])  # Set x-axis limits
plt.ylim([0, 0.1])  # Set y-axis limits
save_path = os.path.join(ca_directory, 'arrival_rate_newcall_vs_loss.png')
plt.savefig(save_path)



# Plot server utilization of handover and time
plt.figure()
data.plot(x='time', y='uh', kind='line')
plt.xlabel('Time')
plt.ylabel('Server Utilization Handover')
plt.title('Server Utilization Handover vs Time')
plt.xlim([0, 1000])  # Set x-axis limits
plt.ylim([0, 1])  # Set y-axis limits
save_path = os.path.join(ca_directory, 'server_utilization_handover_vs_time.png')
plt.savefig(save_path)

# Plot server utilization of newcall and time
plt.figure()
data.plot(x='time', y='un', kind='line')
plt.xlabel('Time')
plt.ylabel('Server Utilization Newcall')
plt.title('Server Utilization Newcall vs Time')
plt.xlim([0, 1000])  # Set x-axis limits
plt.ylim([0, 1])  # Set y-axis limits
save_path = os.path.join(ca_directory, 'server_utilization_newcall_vs_time.png')
plt.savefig(save_path)


# Plot Theoretical Aggregate Loss vs Aggregate Loss
plt.figure()
data.plot(x='time', y=['alp', 'tlpa'], kind='line')
plt.xlabel('Time')
plt.ylabel('Aggregate Loss')
plt.title('Theoretical Aggregate Loss vs Aggregate Loss')
plt.xlim([0, 1000])  # Set x-axis limits
plt.ylim([0, 1])  # Set y-axis limits
plt.legend(['Aggregate Loss', 'Theoretical Aggregate Loss'])
save_path = os.path.join(ca_directory, 'theoretical_loss_vs_loss.png')
plt.savefig(save_path)


# Plot Time vs Aggregate Loss
plt.figure()
data.plot(x='time', y='alp', kind='line')
plt.xlabel('Time')
plt.ylabel('aggregate loss probability per unit time')
plt.title('Aggregate Loss Probability per unit time vs Time')
plt.xlim([0, 1100])  # Set x-axis limits
plt.ylim([0, 0.1])  # Set y-axis limits
save_path = os.path.join(ca_directory, 'aggregate_loss_vs_time.png')
plt.savefig(save_path)

# Plot Time vs Theoretical Aggregate Loss
plt.figure()
data.plot(x='time', y='tlpa', kind='line')
plt.xlabel('Time')
plt.ylabel('Theoretical loss probability per unit time')
plt.title('Theoretical Loss Probability per unit time vs Time')
plt.xlim([0, 1100])  # Set x-axis limits
plt.ylim([0, 35])  # Set y-axis limits
save_path = os.path.join(ca_directory, 'theoretical_loss_vs_time.png')
plt.savefig(save_path)
