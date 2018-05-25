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

package test;

import com.google.common.util.concurrent.RateLimiter;
import movlazy.MovService;
import movlazy.MovWebApi;
import movlazy.dto.SearchItemDto;
import movlazy.model.CastItem;
import movlazy.model.SearchItem;
import org.junit.jupiter.api.Test;
import util.HttpRequest;
import util.IRequest;
import util.Queries;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static util.Queries.filter;
import static util.Queries.skip;


public class MovServiceTestForWarGames {

    @Test
    public void testSearchMovieInSinglePage() {
        MovService movapi = new MovService(new MovWebApi(new HttpRequest()));
        Iterable<SearchItem> movs = movapi.search("War Games");
        SearchItem m = movs.iterator().next();
        assertEquals("War Games: The Dead Code", m.getTitle());
        assertEquals(6, Queries.count(movs));// number of returned movies
    }

    @Test
    public void testSearchMovieManyPages() {
        int[] count = {0};
        IRequest req = new HttpRequest()
                // .compose(System.out::println)
                .compose(__ -> count[0]++);

        MovService movapi = new MovService(new MovWebApi(req));
        Iterable<SearchItem> movs = movapi.search("candle");
        assertEquals(0, count[0]);
        SearchItem candleshoe =
                filter(
                        m -> m.getTitle().equals("Candleshoe"),
                        movs)
                .iterator()
                .next();
        assertEquals(2, count[0]); // Found on 2nd page
        assertEquals(59, Queries.count(movs));// Number of returned movies
        assertEquals(6, count[0]); // 4 requests more to consume all pages
    }

    @Test
    public void testMovieDbApiGetActor() {
        int[] count = {0};
        IRequest req = new HttpRequest()
                // .compose(System.out::println)
                .compose(__ -> count[0]++);

        MovWebApi movWebApi = new MovWebApi(req);
        SearchItemDto[] actorMovs = movWebApi.getPersonCreditsCast(4756);
        assertNotNull(actorMovs);
        assertEquals("Ladyhawke", actorMovs[1].getTitle());
        assertEquals(1, count[0]); // 1 request
    }

    @Test
    public void testSearchMovieThenActorsThenMoviesAgain() {
        final RateLimiter rateLimiter = RateLimiter.create(3.0);
        final int[] count = {0};
        IRequest req = new HttpRequest()
                .compose(__ -> count[0]++)
                .compose(System.out::println)
                .compose(__ -> rateLimiter.acquire());

        MovService movapi = new MovService(new MovWebApi(req));

        Iterable<SearchItem> vs = movapi.search("War Games");
        assertEquals(6, Queries.count(vs));// number of returned movies
        assertEquals(2, count[0]);         // 2 requests to consume all pages
        /**
         * Iterable<SearchItem> is Lazy and without cache.
         */
        SearchItem warGames = filter(
                    m -> m.getTitle().equals("WarGames"),
                    vs)
                .iterator()
                .next();
        assertEquals(3, count[0]); // 1 more request for 1st page
        assertEquals(860, warGames .getId());
        assertEquals("WarGames", warGames.getTitle());
        assertEquals(3, count[0]); // Keep the same number of requests
        /**
         * getDetails() relation SearchItem ---> Movie is Lazy and supported on Supplier<Movie> with Cache
         */
        assertEquals("WarGames", warGames.getDetails().getOriginalTitle());
        assertEquals(4, count[0]); // 1 more request to get the Movie
        assertEquals("Is it a game, or is it real?", warGames.getDetails().getTagline());
        assertEquals(4, count[0]); // NO more request. It is already in cache
        /**
         * getCast() relation Movie --->* CastItem is Lazy and
         * supported on Supplier<List<CastItem>> with Cache
         */
        Iterable<CastItem> warGamesCast = warGames.getDetails().getCast();
        assertEquals(5, count[0]); // 1 more request to get the Movie Cast
        assertEquals("Matthew Broderick",
                warGamesCast.iterator().next().getName());
        assertEquals(5, count[0]); // NO more request. It is already in cache
        assertEquals("Ally Sheedy",
                skip(warGamesCast, 2).iterator().next().getName());
        assertEquals(5, count[0]); // NO more request. It is already in cache
        /**
         * CastItem ---> Actor is Lazy and with Cache for Person but No cache for actor credits
         */
        CastItem broderick = warGames.getDetails().getCast().iterator().next();
        assertEquals(5, count[0]); // NO more request. It is already in cache
        assertEquals("New York City, New York, USA",
                broderick.getActor().getPlaceOfBirth());
        assertEquals(6, count[0]); // 1 more request for Actor Person
        assertEquals("New York City, New York, USA",
                broderick.getActor().getPlaceOfBirth());
        assertEquals(6, count[0]); // NO more request. It is already in cache
        assertEquals("Inspector Gadget",
                broderick.getActor().getMovies().iterator().next().getTitle());
        assertEquals(7, count[0]); // 1 more request for Actor Credits
        assertEquals("Inspector Gadget",
                broderick.getActor().getMovies().iterator().next().getTitle());
        assertEquals(8, count[0]); // 1 more request. Actor Cast is not in cache

        /**
         * Check Cache from the beginning
         */
        assertEquals("New York City, New York, USA",
                movapi.getMovie(860).getCast().iterator().next().getActor().getPlaceOfBirth());
        assertEquals(8, count[0]); // No more requests for the same getMovie.
        /*
         * Now get a new Film
         */
        assertEquals("Predator",
                movapi.getMovie(861).getCast().iterator().next().getActor().getMovies().iterator().next().getTitle()); //FIXME: supplier
        assertEquals(12, count[0]); // 1 request for Movie + 1 for CastItems + 1 Person + 1 Actor Credits
    }

    @Test
    public void testSearchMovieWithManyPages() {
        final RateLimiter rateLimiter = RateLimiter.create(3.0);
        final int[] count = {0};
        IRequest req = new HttpRequest()
                .compose(__ -> count[0]++)
                .compose(System.out::println)
                .compose(__ -> rateLimiter.acquire());

        MovService movapi = new MovService(new MovWebApi(req));

        Iterable<SearchItem> vs = movapi.search("fire");
        assertEquals(1166, Queries.count(vs)); // number of returned movies FIXME
        assertEquals(59, count[0]);         // 2 requests to consume all pages FIXME
    }
}
