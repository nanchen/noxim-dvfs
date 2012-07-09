
for ((  i = 2 ;  i <= 6;  i++  ))
do
	./compare-with-xy.sh 3 $i $i
done

meld report/q report/xy
