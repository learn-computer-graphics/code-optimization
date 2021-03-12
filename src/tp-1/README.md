# TP1 OpenMP

**Guillaume Haerinck - 2021**

## Exercice 1 - Architecture

The code has been processed on an Intel(R) Core(TM) i3-4160 CPU @ 3.60GHz with 2 cores and 4 threads.

## Exercice 2 - Private variables

When using the `private` attribute, the variable is declared local for each threads and is uninitialized. When using `firstprivate` the variable is initialazed with the value of the same name before the thread.

```cpp
int val2 = 2000;

#pragma omp parallel firstprivate(val2)
{
    // val2 == 2000
}

#pragma omp parallel private(val2)
{
    // val2 == ?
}
```

On each case, the variable is scopped by the thread, the val2 declared at the top will have its value unchanged no mater what happened in the threads.

## Exercice 3 - Parallel loops

OpenMP can unroll loops with the argument `for`. By default, we see that the workload will be split in consecutive chunks.

```bash
...
thread: 1 i: 24
thread: 1 i: 25
thread: 2 i: 26
thread: 2 i: 27
...
```

## Exercice 4 - Compute Pi

In order to prevent data races, we must use the `reduction` attribute during the for loop. It will create a local variable for each thread and apply the given operation on these variables at the end of each loop.

On my CPU with 1 000 000 000 steps to compute pi.

| Num. threads | Compute time (seconds) |
| --- | --- |
| 1 | 7.80309 |
| 2 | 6.20524 |
| 3 | 5.31081 |
| 4 | 5.47914 |
| 5 (hyperthreading) | 6.16231 |
| 6 (hyperthreading) | 5.43923 |

We can see that the gain is not substantial as threads have a sync operation at the end of each loop. We also see that we tend to go higher in compute time for the number of threads as the overhead of managing multiple threads go higher.

We try multiple `schedule` type for our best time with 3 threads

| Chunk type | Chunk size | Compute time (seconds) | 
| --- | --- | --- |
| static | 1 | 7.0929 |
| static | 3 | 5.59276 |
| static | 5 | 5.2482 |
| static | 7 | 5.32676 |
| dynamic | 1 | 30.1377 |
| dynamic | 3 | 13.6536 |
| dynamic | 5 | 12.7548 |
| dynamic | 7 | 10.7147 |
| dynamic | 10 | 13.4124 |
| dynamic | 15 | 10.7397 |
| guided | 1 | 5.13345 |
| guided | 3 | 5.25563 |
| guided | 5 | 5.03886 |
| guided | 7 | 5.1455 |
| runtime | - | 5.82924 |

We can see that our best score is around 5, obtained via static, guided and runtime but not close by dynamic. It is possible to view the workload through [Tracy profiler](https://github.com/wolfpld/tracy/releases/tag/v0.7.6) when the program is launched as admin. However it is difficult to recognize the workload patterns from the trace as we cannot put tracing macro on each loops due to data races.
