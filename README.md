# REST: Efficient and Accurate Reservoir Sampling based Triangle Counting Estimation over Streaming Graphs

## Introduction
Source codes of the paper: "REST: Efficient and Accurate Reservoir Sampling based Triangle Counting Estimation over Streaming Graphs."

## Requirement
- fastutil-7.2.0.jar
- gcc version 7.5.0
- openJDK version 11.0.19

## Implementation
Based on the source code of WRS, we implemented REST and reproduced TRIEST-I, MASCOT in Java programming language. For experimental purposes, we made minor modifications to the source code of FURL, WRS, and RFES.

## Credits
Source code of WRS: [WRS](https://github.com/kijungs/waiting_room)

Source code of RFES: [RFES](https://github.com/BioLab310/RFES)

Source code of FURL: [FURL](https://datalab.snu.ac.kr/furl/)

Source code of MASCOT: [MASCOT](https://datalab.snu.ac.kr/mascot/)

Source code of TRIEST: [TRIEST](https://github.com/aepasto/triest)

## Compilation and Running
### Compilation
Download the provided `.jar` files, and then use the command:

```bash
javac -cp .:<your_path_to_fastutil.jar> *.java
```

#### Example:
```bash
javac -cp .:/home/waiting_room-master/fastutil-7.2.0.jar *.java
```

### Running
After compilation, use the command:

```bash
java -cp .:<your_path_to_fastutil.jar> Main
```

#### Example:
```bash
java -cp .:/home/waiting_room-master/fastutil-7.2.0.jar Main
```