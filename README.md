# RefSolver

## Environment Setup

### Use Docker
If you have install docker, use `Dockerfile` directory is recommanded. Try to build the image:
```bash
docker build . -t autobc:latest
```
After building is successful, start your container and then attach:
```bash
docker run -itd --rm --name autobc autobc:latest
docker exec -it autobc /bin/bash
```

The docker image is based on `Ubuntu:18.04` and some basic tools(like `gcc`, `g++`, `java`) are automatically installed. Note that you may change `Dockerfile` sometimes if something went wrong in your environment.

### Setup Directly
These components should be installed in your system:
```s
Java Jdk 8 (1.8)  # for likelihood.jar
Java Jdk 16       # for modelcounting.jar
g++11, cmake, make
ABC               # see https://github.com/vlab-cs-ucsb/ABC
nuXmv             # see https://nuxmv.fbk.eu/
```

> Some installed tips have been written in `Dockerfile`

Then, `mkdir -p build` to make building directory. Use following bash codes to build our program:
```bash
cd build
cmake ..
make
```

After all, these binary files have been built in `build`:
```
ref_solver
random_solver
ranking
```
Congratulations, you have deployed the environment needed for the experiment.

## Run Experiment

All you need to run the experiment is use a `sh` that we provided. Try:
```bash
chmod +x *.sh
./experiment.sh
```

The experiment will be running in background. After done, the experiment will create folder `experiment1.3` (if not exists) and our experiment results will be list in it.
