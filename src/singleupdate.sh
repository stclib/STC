d=$(git rev-parse --show-toplevel)
mkdir -p $d/../stcsingle/c11 $d/../stcsingle/stc
python singleheader.py $d/include/c11/print.h   > $d/../stcsingle/c11/print.h
python singleheader.py $d/include/stc/calgo.h   > $d/../stcsingle/stc/calgo.h
python singleheader.py $d/include/stc/carc.h    > $d/../stcsingle/stc/carc.h
python singleheader.py $d/include/stc/cbits.h   > $d/../stcsingle/stc/cbits.h
python singleheader.py $d/include/stc/cbox.h    > $d/../stcsingle/stc/cbox.h
python singleheader.py $d/include/stc/ccommon.h > $d/../stcsingle/stc/ccommon.h
python singleheader.py $d/include/stc/cdeq.h    > $d/../stcsingle/stc/cdeq.h
python singleheader.py $d/include/stc/clist.h   > $d/../stcsingle/stc/clist.h
python singleheader.py $d/include/stc/cmap.h    > $d/../stcsingle/stc/cmap.h
python singleheader.py $d/include/stc/coption.h > $d/../stcsingle/stc/coption.h
python singleheader.py $d/include/stc/cpque.h   > $d/../stcsingle/stc/cpque.h
python singleheader.py $d/include/stc/cqueue.h  > $d/../stcsingle/stc/cqueue.h
python singleheader.py $d/include/stc/crand.h   > $d/../stcsingle/stc/crand.h
python singleheader.py $d/include/stc/cregex.h  > $d/../stcsingle/stc/cregex.h
python singleheader.py $d/include/stc/cset.h    > $d/../stcsingle/stc/cset.h
python singleheader.py $d/include/stc/csmap.h   > $d/../stcsingle/stc/csmap.h
python singleheader.py $d/include/stc/cspan.h   > $d/../stcsingle/stc/cspan.h
python singleheader.py $d/include/stc/csset.h   > $d/../stcsingle/stc/csset.h
python singleheader.py $d/include/stc/cstack.h  > $d/../stcsingle/stc/cstack.h
python singleheader.py $d/include/stc/cstr.h    > $d/../stcsingle/stc/cstr.h
python singleheader.py $d/include/stc/csview.h  > $d/../stcsingle/stc/csview.h
python singleheader.py $d/include/stc/cvec.h    > $d/../stcsingle/stc/cvec.h
python singleheader.py $d/include/stc/extend.h  > $d/../stcsingle/stc/extend.h
python singleheader.py $d/include/stc/forward.h > $d/../stcsingle/stc/forward.h
echo "stcsingle headers updated"