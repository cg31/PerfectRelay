
Tests for benchmarking the speed of relaying net traffic.

The tests will listen to 127.0.0.1:1090 and forward all data to 127.0.0.1:5201.

1 build

2 test:

./relay &

iperf -s -p 5201 &

iperf -c 127.0.0.1 -p 1090 -n 10G

