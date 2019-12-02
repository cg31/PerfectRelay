
go and rust tests for benchmarking the speed of relay net traffic.

1 build go or rust

2 test:

./relay 127.0.0.1:1090 127.0.0.1:5201 &

iperf -s -p 5201 &

iperf -c 127.0.0.1 -p 1090 -n 10G

