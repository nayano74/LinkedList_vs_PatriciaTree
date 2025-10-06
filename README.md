# COMP20003 Assignment 2: Patricia Tree Dictionary – Experimentation Guide 

This document outlines all the steps required to install dependencies, run the experiments and generate the final analysis and graphs for the Stage 3 report.

The experiments compare the performance of a Linked List dictionary (```dict1```) with a Patricia Tree dictionary (`dict2`).

## 1. Prerequisites

### 1.1 Package Installation

`Python 3` is required for the analysis and plotting scripts.

Install the following dependencies:
```bash
pip install matplotlib pandas msparser
```
## 2. Compilation

Before running experiments, compile the project to generate both `dict1` and ```dict2``` executables.

### Clean previous builds (ensures a fresh build)
```bash
make clean
```
### Compile both executables
```bash
make all
```
## 3. Running Experiments
#### Make analysis script executable

Use the provided script to parse stdout files and produce readable analysis files.
```bash
chmod +x analyse.pl
```
### 3.1 Experiment 1 – Search Performance vs Dataset Size (N)

This experiment measures the number of node accesses as dataset size N grows, 
illustrating algorithmic complexity (O(N) vs O(K)).

#### Make bash script executable
```bash
chmod +x run_experiment1.sh
```
#### Run the experiment (generates datasets, stdout files and analysed files)
```bash
./run_experiment1.sh
```
### 3.2 Experiment 2 – Execution Time vs Dataset Size (N)

This experiment measures elapsed execution time as N grows, demonstrating real-world
performance differences. Once experiments are run, process and visualize results.

#### Make the script executable
```bash
chmod +x run_experiment2.sh
```
#### Run the experiment (generates time_results.csv)
```bash
./run_experiment2.sh
```
### 3.3 Experiment 3 – Effect of key prefix

Tests the hypothesis that the performance of the Patricia Tree is proportional to the
length of the key (K), not the dataset size (N).

#### Make bash script executable
```bash
chmod +x run_experiment3.sh
```

#### Run the experiment (generates time_results.csv)
```bash
./run_experiment3.sh
```
## 4. Visualization

Once experiments are run, process and visualize results.

### 4.1 Generate Graphs

Use Python scripts to create final plots for inclusion in the report.

#### Generate Experiment 1 (Node Accesses) graph
```bash
python experiment1.py
```
#### Generate Experiment 2 (Execution Time) graph
```bash
python experiment2.py
```

After running these scripts, the following files will be generated in the root directory:

`experiment1_graph.png`

`experiment2_graph.png`

These are ready to be included in your Stage 3 repo
