#!/bin/sh
#
#Conversion script used to change the name from viking to player

for i in *; do sed s/VIKING/PLAYER/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/Viking/Player/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/player/player/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vkit/pit/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vpool/ppool/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vik/plr/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vit/plit/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vpos/ppos/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vbar/pbar/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vnum/pnum/g $i > $i.new; mv $i.new $i; done
for i in *; do sed s/vname/pname/g $i > $i.new; mv $i.new $i; done
