import matplotlib.pyplot as plt
import pandas as pd

# --- 1. Load the Data ---
RESULTS_CSV = "analysed/experiment2/time_results.csv"
try:
    data = pd.read_csv(RESULTS_CSV)
except FileNotFoundError:
    print(f"Error: Could not find the results file '{RESULTS_CSV}'")
    print("Please run the 'run_experiment_time.sh' script first.")
    exit()

# --- 2. Create the Plot ---
plt.figure(figsize=(10, 6))
ax = plt.subplot(1, 1, 1)

ax.plot(data['N'], data['dict1_time_ms'], 'o-', label='Linked List (dict1)', color='blue')
ax.plot(data['N'], data['dict2_time_ms'], 'o-', label='Patricia Tree (dict2)', color='orange')

# --- 3. Style the Plot ---
ax.set_title('Execution Time vs. Dataset Size (N)', fontsize=16)
ax.set_xlabel('Number of Records in Dataset (N)', fontsize=12)
ax.set_ylabel('Total Execution Time (ms)', fontsize=12)
ax.legend()
ax.grid(True, which="both", ls="--", linewidth=0.5)
plt.tight_layout()

# --- 4. Save the Plot ---
output_filename = 'experiment2_graph.png'
plt.savefig(output_filename)
print(f"\nTime comparison graph successfully saved to '{output_filename}'")
