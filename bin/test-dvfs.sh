./run-dvfs-off.sh 100 4 0
./run-dvfs-div.sh 100 4 0
cd working
grep 'Total received flit' *
cd ..
