# REST: Efficient and Accurate Reservoir Sampling based Triangle Counting Estimation over Streaming Graphs

## Introduction
Source codes of the paper: "REST: Efficient and Accurate Reservoir Sampling based Triangle Counting Estimation over Streaming Graphs."

## Requirement
- fastutil-7.2.0.jar
- openJDK version 11.0.19

## Implementation
Based on the "fastutil" library, we implemented REST in Java programming language. 


## Compilation and Running
### Compilation
Download the provided `.jar` files, and then use the command:

```bash
javac -cp .:<your_path_to_fastutil.jar> *.java
```

#### Example:
```bash
javac -cp .:/home/username/fastutil-7.2.0.jar *.java
```

### Running
After compilation, you can run the provided `.sh` file or use the command:

```bash
java -cp .:<your_path_to_fastutil.jar> Main 0.5 100000
```

#### Example:
```bash
java -cp .:/home/username/fastutil-7.2.0.jar Main 0.5 100000
```
When running this code, please remember to fill in the graph file path in the code.