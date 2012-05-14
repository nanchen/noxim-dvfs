
for ((  i = 2 ;  i <= 10;  i++  ))
do
	./compare-with-xy.sh 10 $i $i
done

meld report/q report/xy