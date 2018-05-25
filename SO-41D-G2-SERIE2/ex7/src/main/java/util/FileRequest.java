package util;

import util.HttpRequest;
import util.IRequest;
import util.iterator.InputStreamIterator;

import java.io.*;
import java.net.URL;
import java.util.Iterator;
import java.util.function.Consumer;

import static util.Queries.reduce;

public class FileRequest implements IRequest {
    @Override
    public InputStream getBody(String path) {
        String p = path
                .replace('/', '-')
                .replace('?', '-')
                .replace('&', '-')
                .replace('=', '-')
                .replace(',', '-')
                .substring(29, path.length());
        URL u = ClassLoader.getSystemResource(p);
        if (u == null) {
            Iterable<String> scr = () -> new InputStreamIterator(() -> new HttpRequest().getBody(path));
            String json = reduce(scr, "", (prev, curr) -> prev+curr );
            try {
                PrintWriter writer = new PrintWriter("src/test/resources/"+p);
                writer.println(json);
                writer.flush();
                writer.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace(); //FIXME
            }
        }
        try {
            u = ClassLoader.getSystemResource(p);
            return u.openStream();
        } catch (IOException e) {
            throw new UncheckedIOException(e);
        }
    }
}

