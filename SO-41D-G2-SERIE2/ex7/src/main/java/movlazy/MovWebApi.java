/*
 * Copyright (c) 2018 Miguel Gamboa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package movlazy;

import com.google.gson.Gson;
import movlazy.dto.CastItemDto;
import movlazy.dto.MovieDto;
import movlazy.dto.PersonDto;
import movlazy.dto.SearchItemDto;
import movlazy.dto.CastDto;
import movlazy.dto.SearchDto;
import util.IRequest;
import util.iterator.InputStreamIterator;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.text.MessageFormat;

import static util.Queries.reduce;

/**
 * @author Miguel Gamboa
 *         created on 16-02-2017
 */

public class MovWebApi {
    /**
     * Constants
     *
     * To format messages URLs use {@link java.text.MessageFormat#format(String, Object...)} method.
     */
    private static final String MOVIE_DB_HOST = "https://api.themoviedb.org/3/";
    private static final String MOVIE_DB_SEARCH = "search/movie?api_key={0}&query={1}&page={2}";
    private static final String MOVIE_DB_MOVIE = "movie/{1}?api_key={0}";
    private static final String MOVIE_DB_MOVIE_CREDITS = "movie/{1}/credits?api_key={0}";
    private static final String MOVIE_DB_PERSON = "person/{1}?api_key={0}";
    private static final String MOVIE_DB_PERSON_CREDITS = "person/{1}/movie_credits?api_key={0}";
    private static String MOVIE_DB_TOKEN;

    private final IRequest req;
    private final Gson gson = new Gson();

    static {
        try{
            URL keyFile= ClassLoader.getSystemResource("movies-key.txt");
            if(keyFile==null){
                throw  new IllegalStateException("NO KEY FOUND");

            }else {
                InputStream keyStream = keyFile.openStream();
                try(BufferedReader reader = new BufferedReader(new InputStreamReader(keyStream))){
                    MOVIE_DB_TOKEN = reader.readLine();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /*
     * Constructors
     */
    public MovWebApi(IRequest req) {
        this.req = req;
    }

    /**
     * E.g. https://api.themoviedb.org/3/search/movie?api_key=9b2f22e97ee512a9d3224d4aa0d8bd39&query=war+games
     */
    public SearchItemDto[] search(String title, int page) {
        String url = MessageFormat.format(MOVIE_DB_HOST + MOVIE_DB_SEARCH, MOVIE_DB_TOKEN,
                title.replaceAll(" ", "+"), Long.toString(page));
        Iterable<String> scr = () -> new InputStreamIterator(() -> req.getBody(url));
        String json = reduce(scr, "", (prev, curr) -> prev+curr );
        SearchDto dto = gson.fromJson(json, SearchDto.class);
        return dto.getResults();
    }

    /**
     * E.g. https://api.themoviedb.org/3/movie/860?api_key=9b2f22e97ee512a9d3224d4aa0d8bd39
     */
    public MovieDto getMovie(int id) {
        String url = MessageFormat.format(MOVIE_DB_HOST + MOVIE_DB_MOVIE, MOVIE_DB_TOKEN, Long.toString(id));
        Iterable<String> scr = () -> new InputStreamIterator(() -> req.getBody(url));
        String json = reduce(scr, "", (prev, curr) -> prev+curr );
        return gson.fromJson(json, MovieDto.class);
    }

    /**
     * E.g. https://api.themoviedb.org/3/movie/860/credits?api_key=9b2f22e97ee512a9d3224d4aa0d8bd39
     */
    public CastItemDto[] getMovieCast(int movieId) {
        String url = MessageFormat.format(MOVIE_DB_HOST + MOVIE_DB_MOVIE_CREDITS, MOVIE_DB_TOKEN, Long.toString(movieId));
        Iterable<String> scr = () -> new InputStreamIterator(() -> req.getBody(url));
        String json = reduce(scr, "", (prev, curr) -> prev+curr );
        CastDto dto = gson.fromJson(json, CastDto.class);
        return dto.getCast();
    }

    /**
     * E.g. https://api.themoviedb.org/3/person/4756?api_key=9b2f22e97ee512a9d3224d4aa0d8bd39
     */
    public PersonDto getPerson(int personId) {
        String url = MessageFormat.format(MOVIE_DB_HOST + MOVIE_DB_PERSON, MOVIE_DB_TOKEN, Long.toString(personId));
        Iterable<String> scr = () -> new InputStreamIterator(() -> req.getBody(url));
        String json = reduce(scr, "", (prev, curr) -> prev+curr );
        return gson.fromJson(json, PersonDto.class);
    }

    /**
     * E.g. https://api.themoviedb.org/3/person/4756/movie_credits?api_key=9b2f22e97ee512a9d3224d4aa0d8bd39
     */
    public SearchItemDto[] getPersonCreditsCast(int personId) {
        String url = MessageFormat.format(MOVIE_DB_HOST + MOVIE_DB_PERSON_CREDITS, MOVIE_DB_TOKEN, Long.toString(personId));
        Iterable<String> scr = () -> new InputStreamIterator(() -> req.getBody(url));
        String json = reduce(scr, "", (prev, curr) -> prev+curr );
        SearchDto dto = gson.fromJson(json, SearchDto.class);
        return dto.getCast();
    }
}