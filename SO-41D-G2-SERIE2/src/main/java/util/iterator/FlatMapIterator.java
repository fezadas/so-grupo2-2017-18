package util.iterator;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.function.Function;

public class FlatMapIterator<T,R> implements Iterator<R> {

    final Function<T, Iterable<R>> mapper;
    final Iterator<T> src;
    private Iterator<R> currScr;
    private R currValue;
    private boolean consumed;

    public FlatMapIterator(Iterable<T> src, Function<T, Iterable<R>> mapper) {
        this.src = src.iterator();
        this.mapper = mapper;
        this.consumed = true;
    }

    public boolean hasNext() {
        if (!consumed) return true;
        if (currValue==null){
            if (src.hasNext())
                currScr = mapper.apply(src.next()).iterator();
            else return false;
        }
        if(currScr.hasNext()){
            currValue = currScr.next();
            consumed = false;
        }
        else
            currValue = null;
        return true;
    }

    public R next() {
        if (!hasNext()) throw new NoSuchElementException();
        consumed = true;
        R val = currValue;
        if(!currScr.hasNext()) currValue = null;
        return val;
    }
}
