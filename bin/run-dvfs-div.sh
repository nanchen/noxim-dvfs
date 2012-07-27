echo $1
for ((  i = 1 ;  i <= $1;  i++  ))
do
./noxim -dimx 4 -dimy 4 -dvfs dvfs/dvfs-div -detailed -buffer $2 -verbose $3  > working/dvfs-div-$i.log	
done

#grep 'nan' working/*.log -B10 -A10 |less

