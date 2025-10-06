#!/bin/bash

# =============================================================================
# SCRIPT FOR STAGE 3, EXPERIMENT 3: EFFECT OF KEY PREFIX LENGTH (K)
# =============================================================================

echo "--- Preparing for Experiment 3: Key Prefix Length ---"

mkdir -p outputs/experiment3
# --- 1. Define Variables ---
EXE="./dict2"
STAGE=2
DATASET="tests/dataset_1067.csv"
OUTPUT_FILE="outputs/experiment3/temp_output.txt" # A temporary file we can overwrite

QUERIES_SHORT_FILE="tests/queries_short.in"
QUERIES_LONG_FILE="tests/queries_long.in"

STDOUT_SHORT_FILE="outputs/experiment3/stdout_short_keys.out"
STDOUT_LONG_FILE="outputs/experiment3/stdout_long_keys.out"

ANALYSIS_SCRIPT="./analyse.pl"
ANALYSIS_OUTPUT_DIR="analysed/experiment3"
ANALYSIS_OUTPUT_FILE="${ANALYSIS_OUTPUT_DIR}/analysis_summary.txt"
mkdir -p "$ANALYSIS_OUTPUT_DIR"

# --- 2. Create the Query Files ---
echo "Creating specialized query files..."

# Create a file with short, partial key prefixes.
# The `>` overwrites the file if it already exists.
cat > "$QUERIES_SHORT_FILE" << EOL
18 PRO
783 SWA
230 GRAT
28S/151 BERKELEY
44 PROFESSORS
170 GRATTAN
3/650 SWANSTON
48 ROYAL
644 SWANSTON
30S/151 BERKELEY
95-129 BARRY
214 BERKELEY
247-249 BOUVERIE
1302/18 LINCOLN
EOL

# Create a file with long, full keys from the dataset.
cat > "$QUERIES_LONG_FILE" << EOL
18 PROFESSORS WALK PARKVILLE 3052
783 SWANSTON STREET PARKVILLE 3052
230 GRATTAN STREET PARKVILLE 3052
28S/151 BERKELEY STREET MELBOURNE 3000
44 PROFESSORS WALK PARKVILLE 3052
170 GRATTAN STREET PARKVILLE 3052
3/650 SWANSTON STREET CARLTON 3053
6/650 SWANSTON STREET CARLTON 3053
7/650 SWANSTON STREET CARLTON 3053
48 ROYAL PARADE PARKVILLE 3052
26/650 SWANSTON STREET CARLTON 3053
644 SWANSTON STREET CARLTON 3053
8/650 SWANSTON STREET CARLTON 3053
30S/151 BERKELEY STREET MELBOURNE 3000
EOL

echo "Query files created: ${QUERIES_SHORT_FILE} and ${QUERIES_LONG_FILE}"

# --- 3. Run the Experiments ---
echo -e "\n--- Running test with SHORT key prefixes... ---"
$EXE $STAGE "$DATASET" "$OUTPUT_FILE" < "$QUERIES_SHORT_FILE" > "$STDOUT_SHORT_FILE"
echo "Short key test complete. Output statistics are in ${STDOUT_SHORT_FILE}"

echo -e "\n--- Running test with LONG, full keys... ---"
$EXE $STAGE "$DATASET" "$OUTPUT_FILE" < "$QUERIES_LONG_FILE" > "$STDOUT_LONG_FILE"
echo "Long key test complete. Output statistics are in ${STDOUT_LONG_FILE}"


# --- 4. Analyze Results and Save to File ---
echo -e "\n--- Analyzing results and saving to ${ANALYSIS_OUTPUT_FILE} ---"

{
    echo "--- Analysis of Results for Experiment 3 ---";
    echo "";
    echo "--- Performance for SHORT Prefixes ---";
} > "$ANALYSIS_OUTPUT_FILE"

if [ -f "$ANALYSIS_SCRIPT" ]; then
    $ANALYSIS_SCRIPT < "$STDOUT_SHORT_FILE" >> "$ANALYSIS_OUTPUT_FILE"
else
    echo "Analysis script not found. Please check for analyse.pl" >> "$ANALYSIS_OUTPUT_FILE"
fi

{
    echo "";
    echo "--- Performance for LONG Keys ---";
} >> "$ANALYSIS_OUTPUT_FILE"

if [ -f "$ANALYSIS_SCRIPT" ]; then
    $ANALYSIS_SCRIPT < "$STDOUT_LONG_FILE" >> "$ANALYSIS_OUTPUT_FILE"
else
    echo "Analysis script not found. Please check for analyse.pl" >> "$ANALYSIS_OUTPUT_FILE"
fi

echo -e "\n--- Experiment 3 Finished! ---"
echo "A full summary of the analysis has been saved to: ${ANALYSIS_OUTPUT_FILE}"
