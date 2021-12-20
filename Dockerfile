# This docker file is to build an environment for cgigo
FROM nvidia/cuda:8.0-cudnn5-devel-ubuntu16.04

# install caffe dependency
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        vim \
        cmake \
        unzip \
        libeigen3-dev \
        build-essential

# intall caffe
RUN apt-get install -y --no-install-recommends \
        git \
        doxygen \
        python-pip \
        libboost-all-dev \
        libgoogle-glog-dev \
        libgflags-dev \
        libprotobuf-dev \
        protobuf-compiler \
        liblmdb-dev \
        libhdf5-serial-dev \
        libleveldb-dev \
        libsnappy-dev \
        libopencv-dev \
        libatlas-base-dev && \
    pip install --upgrade "pip < 21.0" && \
    pip install --upgrade \
        setuptools \
        numpy \
        gdown && \
    cd /tmp && \
    gdown https://drive.google.com/uc?id=12ffeeIANEi1Mu7tqPBVpC7tFeXLWbKEg && \
    unzip caffe-master.zip && \
    rm caffe-master.zip && \
    cd caffe-master && \
    cmake . && make -j

# install caffe2
RUN pip install --upgrade \
        future \
        six \
        protobuf \
        hypothesis && \
    pip cache purge && \
    cd /tmp && \
    gdown https://drive.google.com/uc?id=1ugPKIfPL5ATUzfqIirtS0_6J0L9k6zPq && \
    unzip caffe2.zip && \
    rm caffe2.zip && \
    cd caffe2 && \
    make -j && cd build && make -j10 install

# install gogui
RUN apt-get install -y unzip gconf2 default-jre && \
    gdown https://drive.google.com/uc?id=1PPsu9If1sw70-ttcGePO1_qmydVtCAl6 && \
    unzip gogui-1.4.9.zip && \
    echo "exit 0" >> gogui-1.4.9/install.sh && \
    cd gogui-1.4.9 && \
    ./install.sh && \
    cd .. && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf gogui-1.4.9 gogui-1.4.9.zip

# install CGI
RUN mkdir -p /cgi-go && \
    git clone https://github.com/kds285/CGIGo.git /cgi-go && \
    cd /cgi-go/database && \
    unzip radius3_symmetric_index.zip && rm radius3_symmetric_index.zip && \
    cd /cgi-go/database/dcnn && \
    gdown https://drive.google.com/uc?id=1K-U_VFvIhAoIKoYAI6Z9bPdcVhY6z7Ki

WORKDIR /cgi-go
