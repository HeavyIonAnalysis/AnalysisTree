FROM centos:centos7
# install prerequisites
RUN yum install -y centos-release-scl-rh \
	which wget git binutils libX11-devel libXpm-devel libXft-devel libXext-devel python-devel && \
	yum install -y devtoolset-8-toolchain
# load new c++ toolchain
SHELL [ "/usr/bin/scl", "enable", "devtoolset-8"]
# install cmake
RUN wget -qO- "https://cmake.org/files/v3.17/cmake-3.17.0-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local
# install root
RUN wget -q https://root.cern/download/root_v6.20.04.source.tar.gz -O /tmp/root.tar.gz && \
	tar -zxf /tmp/root.tar.gz && \
	mkdir build_root && \
	cd build_root && \
	cmake ../root-6.20.04 -Dcxx17=ON -DMathMore=ON -Dbuiltin_gsl=ON && \
	make -j4