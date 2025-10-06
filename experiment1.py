import matplotlib.pyplot as plt
import numpy as np
import os
import re

# ======================= CONFIGURATION ==========================
DATASET_SIZES_N = [100, 200, 300, 400, 500, 600, 700, 800, 900, 1067]
DICT1_ANALYSIS_PATH = "analysed/experiment1/dict1/"
DICT2_ANALYSIS_PATH = "analysed/experiment1/dict2/"
# ================================================================

def parse_average_accesses(filepath):
    """ Reads an analysis file and extracts 'Average Node Accesses' value. """
    print(f"  Attempting to read: {filepath}") # DEBUG PRINT
    try:
        with open(filepath, 'r') as f:
            for line in f:
                match = re.search(r'Average Node Accesses: (\d+\.?\d*)', line)
                if match:
                    value = float(match.group(1))
                    print(f"    -> Found value: {value}") # DEBUG PRINT
                    return value
    except FileNotFoundError:
        print(f"    -> ERROR: File not found.")
        return None
    print(f"    -> WARNING: 'Average Node Accesses' not found in file.")
    return None

# --- Data Collection ---
print("--- Reading analysis files for Experiment 1 ---")
linked_list_accesses = []
patricia_tree_accesses = []
valid_dataset_sizes = []

for n in DATASET_SIZES_N:
    print(f"\nProcessing N = {n}")
    dict1_filepath = os.path.join(DICT1_ANALYSIS_PATH, f"analysed1_N{n}.txt")
    dict2_filepath = os.path.join(DICT2_ANALYSIS_PATH, f"analysed2_N{n}.txt")

    ll_accesses = parse_average_accesses(dict1_filepath)
    pt_accesses = parse_average_accesses(dict2_filepath)

    if ll_accesses is not None and pt_accesses is not None:
        valid_dataset_sizes.append(n)
        linked_list_accesses.append(ll_accesses)
        patricia_tree_accesses.append(pt_accesses)
    else:
        print(f"Skipping data point for N={n} due to missing data.")

print("\n--- Data collection complete ---")
print(f"Final Dataset Sizes (N): {valid_dataset_sizes}")
print(f"Final Linked List Accesses: {linked_list_accesses}")
print(f"Final Patricia Tree Accesses: {patricia_tree_accesses}")

# --- Create the Plot ---
print("\n--- Generating plot ---")
plt.figure(figsize=(10, 6))
ax = plt.subplot(1, 1, 1)

ax.plot(valid_dataset_sizes, linked_list_accesses, 'o-', label='Linked List (dict1)', color='blue')
ax.plot(valid_dataset_sizes, patricia_tree_accesses, 'o-', label='Patricia Tree (dict2)', color='orange')

ax.set_title('Search Performance vs. Dataset Size (N)', fontsize=16)
ax.set_xlabel('Number of Records in Dataset (N)', fontsize=12)
ax.set_ylabel('Average Node Accesses (Logarithmic Scale)', fontsize=12)

# --- FIX: Explicitly set scales ---
ax.set_xscale('linear') # Use a standard linear scale for the X-axis
ax.set_yscale('log')    # Use a logarithmic scale for the Y-axis

ax.legend()
ax.grid(True, which="both", ls="--", linewidth=0.5)
plt.tight_layout()

# --- Save the Plot ---
output_filename = 'experiment1_graph.png'
plt.savefig(output_filename)
print(f"\nGraph successfully saved to '{output_filename}'")

