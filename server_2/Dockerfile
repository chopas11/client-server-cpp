FROM ubuntu:latest
WORKDIR ./server_2
RUN apt update
RUN apt install build-essential -y
# RUN g++ -Wall -o server_2 server_2.cpp
COPY . .
EXPOSE 5002
# CMD ["./server_1"]