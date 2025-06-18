FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y flex bison gcc make && \
    apt-get clean

WORKDIR /app
COPY . /app

# Hacer ejecutables los scripts
RUN chmod +x build_and_test.sh

CMD ["bash"]