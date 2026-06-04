# TMC&STMC

This is the source code for our paper.



TMC is an algorithm for counting all 3-node motifs in temporal networks, while STMC is designed to count all 3-node motifs over streaming temporal graphs, which requires motif counting to be available at any timestamp. This repository contains the source codes of the proposed algorithms as well as several peer baseline algorithms. Researchers can directly compile this project to quickly verify the correctness and empirical efficiency of the proposed methods.

## Compile

### Compile the project

1.download the project, the project looks like:

```
all-Triadic-Motifs-Counting
│
├── README.md                    # Project documentation
├── example.txt                  # Example input dataset
│
├── TMC/                         # Temporal Motif Counting algorithm
│   ├── ews.cpp                  # Main implementation of TMC
│   ├── graph_ews.h              # Graph data structure and utilities
│   └── TMC.exe                  # Executable program
│
└── STMC/                        # Streaming Temporal Motif Counting algorithm
    ├── es_stream.cpp            # Main implementation of STMC
    ├── graph_es_stream.h        # Streaming graph data structure
    └── STMC.exe                 # Executable program
```

We only elaborate on the compilation procedures of TMC and STMC.

2.Run the `g++` command to compile the project and generate executable files for TMC and STMC。

```
g++ ews.cpp -o TWC
g++ es_stream.cpp -o STMC
```

## Command line arguments

### 1.TMC Command Line Arguments

```
./TMC input delta edge_Sampling_P wedge sampling_Q seed_for_P
```

|     Argument     |  Type  |           Meaning           |
| :--------------: | :----: | :-------------------------: |
|      input       | string | the path of input data file |
|      delta       |  int   |         time window         |
| edge_sampling_P  | float  | the sampling rate for edge  |
| wedge_sampling_Q | float  | the sampling rate for wedge |
|    seed_for_P    |  int   |         random seed         |

\# For example, if you want to count all 3-node motifs in sms-remapped.txt with a time span of 3600, an edge sampling rate of 0.01, a wedge sampling rate of 0.1, and a random seed of 20, and print the results to the terminal, you can run the following command:

```
./TMC sms-remappped.txt 3600 0.01 0.1 20
```

### 2.STMC Command Line Arguments

| Argument          | Type   | Meaning                     |
| ----------------- | ------ | --------------------------- |
| input             | string | the path of input data file |
| delta             | int    | time window                 |
| size_of_reservoir | int    | the size of reservoir       |
| seed_for_P        | int    | random seed                 |

\# For example, if you want to count all 3-node motifs in sms-remapped.txt with a time span of 3600, a reservoir size of 200000, and a random seed of 20, and print the results to the terminal, you can run the following command:

```
./STMC input delta size_of_reservoir seed_for_P
```

## Data file

### Input data file：

Each line represents a temporal edge, which contains three values: node 1, node 2, and timestamp, separated by spaces.



## Experiment

Experimental results are stored in the `experiment` folder.
