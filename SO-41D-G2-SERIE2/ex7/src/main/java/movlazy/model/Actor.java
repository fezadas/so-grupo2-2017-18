package movlazy.model;

import java.util.function.Supplier;

/**
 * @author Miguel Gamboa
 *         created on 04-08-2017
 */
public class Actor {
    private final int id;
    private final String name;
    private final Iterable<SearchItem> movies;
    private final String placeOfBirth;
    private final String biography;

    public Actor(int id, String name, String placeOfBirth, String biography, Iterable<SearchItem> movies) {
        this.id = id;
        this.name = name;
        this.movies = movies;
        this.placeOfBirth = placeOfBirth;
        this.biography = biography;
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getPlaceOfBirth() {
        return placeOfBirth;
    }

    public String getBiography() {
        return biography;
    }


    @Override
    public String toString() {
        return "Actor{" +
                "id=" + id +
                ", name='" + name + '\'' +
                ", movies=" + movies +
                '}';
    }

    public Iterable<SearchItem> getMovies() {
        return movies;
    }
}
