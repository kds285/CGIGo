# CGI Go Intelligence (abbr. CGI)

This repository introduces the usage of running the computer Go program CGI (for version 4.0). CGI 4.0 is a version based on the AlphaZero algorithm and improved the training process by the population based training. For the background history of other CGI versions, please refer to https://cgilab.nctu.edu.tw/~icwu/aigames/CGI.html.

## Citing CGI
If you use CGI for research, please consider citing our paper as follows:
```
@inproceedings{wu2020accelerating,
  title={Accelerating and improving alphazero using population based training},
  author={Wu, Ti-Rong and Wei, Ting-Han and Wu, I-Chen},
  booktitle={Proceedings of the AAAI Conference on Artificial Intelligence},
  volume={34},
  number={01},
  pages={1046--1053},
  year={2020}
}
```

## Usage

### Environment
We recommend running CGI via the provided container. Here we provide two ways to set up the environment as follows:

1. You can directly download the built image from the Docker Hub with the following commands:
```
docker pull kds285/cgigo
```
2. You can also build the container via the Dockerfile yourself:
```
docker build cgigo .
```

### Running CGI
We provide an example configuration, named `cgi_example.cfg`, to run CGI. The example configuration will use `1` GPU and run `800` simulation counts for each move. Following are the ways to build and run CGI:
1. Build the program via `./scripts/setup-cmake.sh release caffe2 && make`
2. Run the program via `Release/CGI -conf_file cgi_example.cfg`

After the program is started, you can interact with the program via the GTP protocol.

You can also edit the configuration file for different settings as follows:
* let the program play by simulation count: `SIM_CONTROL=COUNT`
  * adjust the simulation count: `SIM_COUNT_LIMIT=800`
* let the program play by thinking time: `SIM_CONTROL=TIME`
  * adjust the thinking time: `SIM_TIME_LIMIT=1`, note that the number indicates seconds