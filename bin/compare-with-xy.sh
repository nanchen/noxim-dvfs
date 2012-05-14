echo "compare with xy routing $1 times, dimx = $2, dimy = $3"

# clean 
rm working/*

#simCmd=./sim-$1.sh
#$simCmd ./noxim -dimx $2 -dimy $3 -buffer 500 > working/q.log
#$simCmd ./noxim -dimx $2 -dimy $3 -routing xy -buffer 500 > working/xy.log

# simulation multiple times
for ((  i = 1 ;  i <= $1;  i++  ))
do
  echo "Simulating time $i"
  ./noxim -dimx $2 -dimy $3 -buffer 500 >> working/q.log
  ./noxim -dimx $2 -dimy $3 -routing xy -buffer 500 >> working/xy.log
done

# analyse log
qReport=report/q/avg-$1-experiments-$2*$3.txt
xyReport=report/xy/avg-$1-experiments-$2*$3.txt
java -jar analyser.jar working/q.log > $qReport
java -jar analyser.jar working/xy.log > $xyReport

# compare
#meld $qReport $xyReport
