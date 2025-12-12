# Scalable Thread Management Library (C++)

This project implements a high-performance, scalable thread pool library 
as part of the Operating Systems course (CSE316) at Lovely Professional University.

## 🚀 Features
- Efficient thread creation using a worker thread pool
- Thread-safe task scheduling queue
- Supports tens of thousands of concurrent tasks
- Active worker and queue monitoring
- Graceful shutdown mechanism
- Performance logging to CSV
- Graph visualization using Python (Colab)

## 📊 Benchmark Summary
- Tasks executed: 20,000
- CPU iterations per task: 50,000
- Total execution time: 0.789321 seconds
- Peak queue size: 17,582 tasks
- Peak active threads: 8
- Throughput: ~25,338 tasks/sec

## 📂 Project Structure

```
.
├── src/
│   └── thread_pool.cpp
├── tests/
│   ├── run_log.txt
│   ├── stats.csv
│   ├── queue.png
│   └── active_threads.png
├── docs/
│   └── report.md
├── colab/
│   └── notebook.ipynb
└── README.md
```

## 🔧 How to Compile
```bash
g++ -std=c++17 -pthread src/thread_pool.cpp -O2 -o thread_pool
```

## ▶️ How to Run
```bash
./thread_pool
```

## 📈 Visualizing Performance
Open `colab/notebook.ipynb` in Google Colab and run all cells to generate performance graphs.

## 👨‍💻 Authors
- Satyam Kumar (RK24PMB39)
- Mushfiqul Alam (RK24PMB40)
- Indulekha S (RK24PMB41)


## 📝 License
Educational use only.
