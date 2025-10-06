#!/bin/bash

# =============================================================================
# SCRIPT FOR STAGE 3, EXPERIMENT 2: EXECUTION TIME vs. DATASET SIZE (N)
# =============================================================================

echo "--- Preparing for Time Measurement Experiment ---"

mkdir -p outputs/experiment2
# --- 1. Define Variables ---
BASE_DATASET="tests/dataset_1067.csv"
QUERY_FILE="tests/test1067.in"
# We can use fewer data points for this, as the trend will be very clear
DATASET_SIZES=(100 200 300 400 500 600 700 800 900 1067)

# --- 2. Create Directory Structure and file to store results ---
mkdir -p analysed/experiment2
RESULTS_CSV="analysed/experiment2/time_results.csv"

# --- 3. Setup Results File ---
# Create a CSV file and write the header row
echo "N,dict1_time_ms,dict2_time_ms" > $RESULTS_CSV

echo "Generating datasets if they don't exist..."
mkdir -p generated_data
for N in "${DATASET_SIZES[@]}"; do
    let L=N+1
    if [ ! -f "generated_data/data_${N}.csv" ]; then
        head -n $L "$BASE_DATASET" > "generated_data/data_${N}.csv"
    fi
done

# --- 4. Run Experiments ---
echo -e "\n--- Running experiments and measuring time... ---"
for N in "${DATASET_SIZES[@]}"; do
    echo "Processing dataset with N = $N"
    
    # Measure time for dict1
    # 'time' is a bash command
    TIME1_MS=$( { time -p ./dict1 1 "generated_data/data_${N}.csv" outputs/experiment2/temp_output.txt < "$QUERY_FILE" > /dev/null; } 2>&1 | awk '/real/ {print $2 * 1000}' )
    echo "  dict1 took: ${TIME1_MS} ms"

    # Measure time for dict2
    TIME2_MS=$( { time -p ./dict2 2 "generated_data/data_${N}.csv" outputs/experiment2/temp_output.txt < "$QUERY_FILE" > /dev/null; } 2>&1 | awk '/real/ {print $2 * 1000}' )
    echo "  dict2 took: ${TIME2_MS} ms"

    # Append the results to our CSV file
    echo "$N,${TIME1_MS},${TIME2_MS}" >> $RESULTS_CSV
done

echo -e "\n--- Time Measurement Finished! ---"
echo "Results have been saved to ${RESULTS_CSV}"
