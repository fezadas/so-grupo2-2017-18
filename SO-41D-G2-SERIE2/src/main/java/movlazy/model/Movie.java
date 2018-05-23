package movlazy.model;

import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.function.Supplier;

import static java.util.Arrays.stream;

/**
 * @author Miguel Gamboa
 *         created on 04-08-2017
 */
public class Movie {
    private final int id;
    private final String original_title;
    private final String tagline;
    private final String overview;
    private final double vote_average;
    private final String release_date;
    private final Supplier<List<CastItem>> cast;

    public Movie(
            int id,
            String original_title,
            String tagline,
            String overview,
            double vote_average,
            String release_date,
            Supplier<List<CastItem>> cast)
    {
        this.id = id;
        this.original_title = original_title;
        this.tagline = tagline;
        this.overview = overview;
        this.vote_average = vote_average;
        this.release_date = release_date;
        this.cast = cast;
    }

    public int getId() {
        return id;
    }

    public String getOriginalTitle() {
        return original_title;
    }

    public String getTagline() {
        return tagline;
    }

    public String getOverview() {
        return overview;
    }

    public double getVoteAverage() {
        return vote_average;
    }

    public String getReleaseDate() {
        return release_date;
    }

    public Iterable<CastItem> getCast() {
        return cast.get();
    }

    @Override
    public String toString() {
        return "Movie{" +
                "id=" + id +
                ", original_title='" + original_title + '\'' +
                ", tagline='" + tagline + '\'' +
                ", overview='" + overview + '\'' +
                ", vote_average=" + vote_average +
                ", release_date='" + release_date + '\'' +
                '}';
    }
}
