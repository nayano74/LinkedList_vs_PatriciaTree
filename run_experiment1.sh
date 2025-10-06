#!/bin/bash

# =============================================================================
# SCRIPT FOR STAGE 3, EXPERIMENT 1: EFFECT OF DATASET SIZE (N)
# =============================================================================

echo "--- Preparing for Experiment 1 ---"

# --- 1. Define Variables ---
BASE_DATASET="tests/dataset_1067.csv"
QUERY_FILE="tests/test1067.in"
DATASET_SIZES=(100 250 300 400 500 600 700 800 900 1067)

# --- 2. Create Directory Structure ---
echo "Creating output directories..."
mkdir -p generated_data
mkdir -p outputs/experiment1/dict1/data
mkdir -p outputs/experiment1/dict1/stdout
mkdir -p outputs/experiment1/dict2/data
mkdir -p outputs/experiment1/dict2/stdout
mkdir -p analysed/experiment1/dict1
mkdir -p analysed/experiment1/dict2

# --- 3. Generate Datasets ---
echo "Generating datasets of varying sizes..."
for N in "${DATASET_SIZES[@]}"; do
    let L=N+1
    head -n $L "$BASE_DATASET" > "generated_data/data_${N}.csv"
done
echo "All datasets generated."

# --- 4. Produce dict1 results (Linked List) ---
echo -e "\n--- Running experiments for dict1 (Linked List)... ---"
for N in "${DATASET_SIZES[@]}"; do
    echo "Processing dataset with N = $N"
    ./dict1 1 "generated_data/data_${N}.csv" "outputs/experiment1/dict1/data/output_N${N}.txt" \
        < "$QUERY_FILE" > "outputs/experiment1/dict1/stdout/stdout_N${N}.out"
done
echo "dict1 output files and stdout streams generated."

# --- 5. Apply pearl script onto stdout streams for analysis (Linked List) ---
echo -e "\n--- Analysing dict1 results (Linked List)... ---"
for N in "${DATASET_SIZES[@]}"; do
    echo "Processing stdout stats with N = $N"
    ./analyse.pl < "outputs/experiment1/dict1/stdout/stdout_N${N}.out" \
    > "analysed/experiment1/dict1/analysed1_N${N}.txt" 
done
echo "dict1 analysis complete. Results saved in analysed/dict1 directory."

# --- 6. Produce dict2 results (Patricia Tree) ---
echo -e "\n--- Running experiments for dict2 (Patricia Tree)... ---"
for N in "${DATASET_SIZES[@]}"; do
    echo "Processing dataset with N = $N"
    ./dict2 2 "generated_data/data_${N}.csv" "outputs/experiment1/dict2/data/output_N${N}.txt" \
        < "$QUERY_FILE" > "outputs/experiment1/dict2/stdout/stdout_N${N}.out"
done
echo "dict2 output files and stdout streams generated."

# --- 7. Apply pearl script onto stdout streams for analysis (Patricia Tree) ---
echo -e "\n--- Analysing dict2 results (Patricia Tree)... ---"
for N in "${DATASET_SIZES[@]}"; do
    echo "Processing stdout stats with N = $N"
    ./analyse.pl < "outputs/experiment1/dict2/stdout/stdout_N${N}.out" \
    > "analysed/experiment1/dict2/analysed2_N${N}.txt"
done
echo "dict2 analysis complete. Results saved in analysed/dict2 directory."

echo -e "\n--- Experiment 1 Finished! ---"
echo "All output data is in the 'outputs' directory."
