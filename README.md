# slowloris_c
Slowloris tool developped in C

That tool permit to implement a slowloris attack. It was developped in C. 

# Uses of the tool

clone the branch:

<code>
git clone https://github.com/arn0f/slowloris_c

cd slowloris_c
</code>

compile the code:

<code>
make
</code>

launch the binary file:

<code>
./slowloris option=value ...
</code>

# Options
target - target of the attack

test - test the time-out defined on the remote server [false]

connection - number of connection created on the remote host [1000]

timeout - timeout to use on the server (seconds) [100]

port - port where initiate the HTTP connection [80]

fork - number of fork to create for initiate the number of connection choosen [50]

duration - duration of the attack (seconds) [600]
