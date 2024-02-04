import pandas as pd
import os
import matplotlib.pyplot as plt

# Load the data from the CSV file
data = pd.read_csv('/home/sushen/projects/com_modelling_sim/results/finals/mmcc_find.csv')

# Create a figure with two subplots
fig, (ax1, ax2, ax3) = plt.subplots(3, figsize=(10, 30))

# Plot arrival rate vs blocking probability
ax1.plot(data['Arrival Rate'], data['Theoretical Blocking Probability'], marker='o', color='green')
ax1.set_xlabel('Arrival Rate')
ax1.set_ylabel('Theoretical Blocking Probability')
ax1.set_title('Arrival Rate vs Theoretical Blocking Probability')
ax1.set_xlim(0.01, 0.1)  # Set x-axis limits
ax1.set_yscale('log')  # Set y-axis limits

# Plot arrival rate vs blocking probability
ax2.plot(data['Arrival Rate'], data['Blocked Calls Probability'], marker='o', color='red')
ax2.set_xlabel('Arrival Rate')
ax2.set_ylabel('Blocked Calls Probability')
ax2.set_title('Arrival Rate vs Blocked Calls Probability')
ax2.set_xlim(0.01, 0.1)  # Set x-axis limits
ax2.set_ylim(100, 8000)  # Set y-axis limits



# Plot arrival rate vs server utilisation
ax3.plot(data['Arrival Rate'], data['Server Utilisation'], marker='o')
ax3.set_xlabel('Arrival Rate')
ax3.set_ylabel('Server Utilisation')
ax3.set_title('Arrival Rate vs Server Utilisation')
ax3.set_xlim(0, 0.1)  # Set x-axis limits
ax3.set_ylim(0, 4000)  # Set y-axis limits

current_directory = os.getcwd()
ca_directory = os.path.join(current_directory, 'plots')
os.makedirs(ca_directory, exist_ok=True)


# Save the figure as a PNG file in the "plots" folder
plt.savefig(os.path.join(ca_directory, 'mmccfinals.png'))
