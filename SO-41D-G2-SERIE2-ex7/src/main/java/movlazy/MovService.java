/*
 * Copyright (c) 2017, Miguel Gamboa
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

import movlazy.dto.CastItemDto;
import movlazy.dto.MovieDto;
import movlazy.dto.PersonDto;
import movlazy.dto.SearchItemDto;
import movlazy.model.Actor;
import movlazy.model.CastItem;
import movlazy.model.Movie;
import movlazy.model.SearchItem;
import util.Queries;
import util.iterator.ArrayIterator;

import java.lang.reflect.Array;
import java.util.*;

import static java.util.Arrays.asList;
import static util.Queries.*;


/**
 * @author Miguel Gamboa
 *         created on 02-03-2017
 */
public class MovService {

    private final MovWebApi movWebApi;
    private final Map<Integer, Movie> movies = new HashMap<>();
    private final Map<Integer, List<CastItem>> cast = new HashMap<>();
    private final Map<Integer, Actor> actors = new HashMap<>();

    public MovService(MovWebApi movWebApi) {
        this.movWebApi = movWebApi;
    }

    public Iterable<SearchItem> search(String name) {
        return
        map(                     // Iterable<SearchItem>
            this::parseSearchItemDto,
            flatMap(             // Iterable<SearchItemDto>
                movs -> of(movs),
                takeWhile(       // Iterable<SearchItemDto[]>
                    movs -> movs.length != 0,
                    map(         // Iterable<SearchItemDto[]>
                        page -> movWebApi.search(name, page),
                        iterate( // Iterable<Integer>
                                0,
                                prev -> ++prev)
                        )
                          )
                    )
            );
    }

    private SearchItem parseSearchItemDto(SearchItemDto dto) {
        return new SearchItem(
                dto.getId(),
                dto.getTitle(),
                dto.getReleaseDate(),
                dto.getVoteAverage(),
                () -> getMovie(dto.getId()));
    }

    public Movie getMovie(int movId) {
        return movies.computeIfAbsent(movId, id -> {
            MovieDto mov = movWebApi.getMovie(id);
            return new Movie(
                    mov.getId(),
                    mov.getOriginalTitle(),
                    mov.getTagline(),
                    mov.getOverview(),
                    mov.getVoteAverage(),
                    mov.getReleaseDate(),
                    () -> this.getMovieCast(id));
        });
    }

    public List<CastItem> getMovieCast(int movId) {
        return cast.computeIfAbsent(
                movId,
                id -> toList(
                        map(this::parseCastItemDto, of(movWebApi.getMovieCast(id))))
        );
    }
    private CastItem parseCastItemDto(CastItemDto dto) {
        return new CastItem(
                dto.getId(),
                dto.getMovieId(),
                dto.getCharacter(),
                dto.getName(),
                () -> getActor(dto.getId(), dto.getName())
        );
    }

    public Actor getActor(int actorId, String name) {
        return actors.computeIfAbsent(actorId, id -> {
            PersonDto person = movWebApi.getPerson(actorId);
            return new Actor(
                    person.getId(),
                    person.getName(),
                    person.getPlace_of_birth(),
                    person.getBiography(),
                    () -> getActorCreditsCast(actorId).iterator());
        });
    }

    public Iterable<SearchItem> getActorCreditsCast(int actorId) {
        return map(this::parseSearchItemDto, of(movWebApi.getPersonCreditsCast(actorId)));
    }
}
